/*
 * Tivaware+FreeRTOS timer interrupt example
 *
 * Harry Mander 2022
 *
 * Demostrates using timer interrupts with FreeRTOS: see IMPORTANT note below
 *
 * A timer generates an interrupt every 1 second which writes an integer to
 * a queue, a FreeRTOS task receives the integers from this queue and writes
 * them to the UART.
 */

#include <stdint.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <utils/ustdlib.h>


#define ERROR() do {} while (1)

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

static QueueHandle_t queue;

static void uart_puts(char* str) {
    while (*str) {
        UARTCharPut(UART0_BASE, *str++);
    }
}

static void uart_task(void *args)
{
    (void) args;

    while (1) {
        uint32_t val;
        while (xQueueReceive(queue, &val, portMAX_DELAY) == pdTRUE) {
            char greeting[100];
            usnprintf(greeting, sizeof(greeting), "Hello %i!\r\n", val);
            uart_puts(greeting);
        }
    }
}

static uint32_t timer_val = 0;

static void timer_interrupt(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    xQueueSendFromISR(queue, &timer_val, NULL);
    timer_val++;
}

static void timer_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
    TimerIntRegister(TIMER0_BASE, TIMER_A, timer_interrupt);

    /*
     * IMPORTANT: must set the interrupt to be greater than or equal to
     * configMAX_SYSCALL_INTERRUPT_PRIORITY, otherwise will cause an assertion
     * failure
     */
    IntPrioritySet(INT_TIMER0A, pdTM4C_RTOS_INTERRUPT_PRIORITY(1));

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
}

int main(void)
{
    SysCtlClockSet(
        SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL |
        SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ
    );

    uart_init();

    queue = xQueueCreate(10, sizeof(uint32_t));
    if (!queue)
        ERROR();

    timer_init();

    if (xTaskCreate(&uart_task, "uart", 256, &queue, 0, NULL) != pdPASS)
        ERROR();

    vTaskStartScheduler();

    return 0;
}

void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
    (void) pcFile;
    (void) ulLine;
    while (1) ;
}
