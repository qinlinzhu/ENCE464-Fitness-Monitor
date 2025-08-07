/*
 * Tivaware+FreeRTOS pulse timer
 *
 * Harry Mander 2022
 *
 * Measures the time of a high pulse on PC4 and prints to the UART.
 *
 * There are two FreeRTOS tasks:
 * - Pulse width task (priority = 1)
 * - UART task (priority = 0)
 *
 * Uses a GPIO rising/falling edge pin interrupt and a general purpose timer. On
 * a rising/falling edge, the GPIO interrupt gets the current timer value and
 * determines whether a falling or rising edge triggered the interrupt; it sends
 * this information to the pulse width task via a queue. The timer interrupt is
 * generated on a timer timeout; in the ISR, a counting semaphore is
 * incremented. When the pulse width task receives a new timer value from the
 * queue, it decrements the counting semaphore to 0 to determine how many timer
 * overflows have occurred since the last time it received a timer value from
 * the queue. It then uses this information along with the time of the previous
 * edge trigger to calculate the pulse width in milliseconds. If the calculated
 * pulse time differs from the previous one by a threshold (MILLIS_MIN_DELTA),
 * it sends this pulse width via another queue to the pulse width task which
 * prints the time to the UART.
 *
 * This program is fairly rough and probably buggy but it should give you some
 * idea how you might begin to write code to measure the frequency and duty
 * cycle of a PWM signal.
 *
 * Note: the pin is prone to signal bounce, so if you are manually generating
 * the pulses by connecting and disconnecting a jumper you will get a lot of
 * false pulse readings. It's best to use a signal generator/PWM output to
 * generate the pulses (or you could a simple passive filter to filter out the
 * signal bounce).
 */

#include <stdint.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <semphr.h>

#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <utils/ustdlib.h>

#include "printf.h"

#define ERROR() do {} while (1)

#define PULSE_SYSCTL_PERIPH SYSCTL_PERIPH_GPIOC
#define PULSE_GPIO_PORT_BASE GPIO_PORTC_BASE
#define PULSE_GPIO_INT_PIN GPIO_INT_PIN_4
#define PULSE_GPIO_PIN GPIO_PIN_4
#define PULSE_INT INT_GPIOC

#define TIMER_SYSCTL_PERIPH SYSCTL_PERIPH_TIMER0
#define TIMER_BASE TIMER0_BASE
#define TIMER_MODULE TIMER_A
#define TIMER_INT INT_TIMER0A
#define TIMER_INT_TYPE TIMER_TIMA_TIMEOUT

#define MILLIS_MIN_DELTA 0.01

static void uart_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART0_BASE);
    UARTEnable(UART0_BASE);
}

static QueueHandle_t pulse_time_q;

// Needed by printf.c
void putchar_(char c) {
    UARTCharPut(UART0_BASE, c);
}

static void uart_task(void *args)
{
    (void) args;

    while (1) {
        double val;
        while (xQueueReceive(pulse_time_q, &val, portMAX_DELAY) == pdTRUE)
            printf_("Pulse time: %.4lf millis\r\n", val);
    }
}

static SemaphoreHandle_t timer_timeout_sem;

static void timer_interrupt(void)
{
    TimerIntClear(TIMER_BASE, TIMER_INT_TYPE);

    /* We don't need to yield to another task since no tasks are blocked on this
       sempahore */
    xSemaphoreGiveFromISR(timer_timeout_sem, NULL);
}

// Setup periodic countdown timer with timeout interrupt
static void timer_init(void)
{
    SysCtlPeripheralEnable(TIMER_SYSCTL_PERIPH);
    while (!SysCtlPeripheralReady(TIMER_SYSCTL_PERIPH)) {}
    TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC);

    // For a 1 second timer period use a load value equal to timer frequency
    TimerLoadSet(TIMER_BASE, TIMER_MODULE, SysCtlClockGet());

    TimerIntRegister(TIMER_BASE, TIMER_MODULE, timer_interrupt);

    /*
     * IMPORTANT: must set the interrupt to be greater than or equal to
     * configMAX_SYSCALL_INTERRUPT_PRIORITY, otherwise will cause an assertion
     * failure
     */
    IntPrioritySet(TIMER_INT, pdTM4C_RTOS_INTERRUPT_PRIORITY(1));

    TimerIntEnable(TIMER_BASE, TIMER_INT_TYPE);
    IntEnable(TIMER_INT);
    TimerEnable(TIMER_BASE, TIMER_MODULE);
}

struct timer_msg {
    uint32_t time;
    bool rising;
};

QueueHandle_t timer_val_q;

static void gpio_interrupt(void)
{
    BaseType_t higher_pri_woken = pdFALSE;

    GPIOIntClear(PULSE_GPIO_PORT_BASE, PULSE_GPIO_INT_PIN);

    const struct timer_msg msg = {
        .time = TimerValueGet(TIMER_BASE, TIMER_MODULE),
        .rising = GPIOPinRead(PULSE_GPIO_PORT_BASE, PULSE_GPIO_PIN),
    };

    xQueueSendFromISR(timer_val_q, &msg, &higher_pri_woken);

    /* If higher_pri_woken is true, this causes FreeRTOS to switch context to the
    higher priority task that was woken by writing to the queue after the ISR
    finishes. Because the pulse timer task has a higher priority than the UART
    task, we want the pulse timer task to preempt the UART task when we write to
    the queue.*/
    portYIELD_FROM_ISR(higher_pri_woken);
}

// Setup PC4 as input with pull-down and rising/falling edge interrupts
static void gpio_setup(void)
{
    SysCtlPeripheralEnable(PULSE_SYSCTL_PERIPH);
    while (!SysCtlPeripheralReady(PULSE_SYSCTL_PERIPH)) {}

    GPIOPinTypeGPIOInput(PULSE_GPIO_PORT_BASE, PULSE_GPIO_PIN);
    GPIOPadConfigSet(PULSE_GPIO_PORT_BASE, PULSE_GPIO_PIN,
        GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);

    GPIOIntTypeSet(PULSE_GPIO_PORT_BASE, PULSE_GPIO_PIN, GPIO_BOTH_EDGES);
    GPIOIntRegister(PULSE_GPIO_PORT_BASE, gpio_interrupt);

    // See note in timer_init
    IntPrioritySet(PULSE_INT, pdTM4C_RTOS_INTERRUPT_PRIORITY(1));

    GPIOIntEnable(PULSE_GPIO_PORT_BASE, PULSE_GPIO_INT_PIN);
    IntEnable(PULSE_INT);
}

static inline double lfabs(const double v)
{
    return v < 0 ? -v : v;
}

static void pulse_timer_task(void *arg)
{
    (void) arg;

    uint32_t rising_time = 0;
    double prev_millis = -1;

    while (1) {
        struct timer_msg msg;
        if (xQueueReceive(timer_val_q, &msg, portMAX_DELAY) == pdTRUE) {
            /* Found out how many times the timer overflowed by taking from the
               semaphore (non-blocking) until we cannot take anymore.*/
            uint32_t overflows = 0;
            while (xSemaphoreTake(timer_timeout_sem, 0) == pdTRUE)
                overflows++;

            if (msg.rising) {
                rising_time = msg.time;
            } else {
                /* Assume that a rising edge message will have been received
                   before this falling edge (which should be the case)*/
                uint64_t ticks;

                if (overflows) {
                    ticks = rising_time + SysCtlClockGet() - msg.time;

                    // The first overflow is a partial cycle
                    overflows -= 1;
                } else {
                    // No overflow so rising time should be higher
                    ticks = rising_time - msg.time;
                }

                // All complete timer cycles correspond to 1000 ms
                const double millis = 1000.0 *
                    (overflows + ((double) ticks) / SysCtlClockGet());

                if (lfabs(millis - prev_millis) > MILLIS_MIN_DELTA) {
                    prev_millis = millis;
                    xQueueSend(pulse_time_q, &millis, portMAX_DELAY);
                }
            }
        }
    }
}

int main(void)
{
    SysCtlClockSet(
        SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL |
        SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ
    );

    IntMasterDisable();

    uart_init();
    gpio_setup();
    timer_init();

    pulse_time_q = xQueueCreate(10, sizeof(double));
    if (!pulse_time_q)
        ERROR();

    timer_val_q = xQueueCreate(10, sizeof(struct timer_msg));
    if (!timer_val_q)
        ERROR();

    timer_timeout_sem = xQueueCreateCountingSemaphore(100, 0);
    if (!timer_timeout_sem)
        ERROR();

    if (xTaskCreate(&uart_task, "uart", 256, NULL, 0, NULL) != pdPASS)
        ERROR();

    // Give this task a higher priority than the uart task
    if (xTaskCreate(&pulse_timer_task, "pulse-timer", 256, NULL, 1, NULL) != pdPASS)
        ERROR();

    IntMasterEnable();
    vTaskStartScheduler();

    return 0;
}

void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
    (void) pcFile;
    (void) ulLine;
    while (1) ;
}
