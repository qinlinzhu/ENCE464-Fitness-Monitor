/*
 * Step_Counter_Main.c
 *
 *  Created on: 23/03/2022
 *      Authors: Matthew Suter, Daniel Rabbidge, Timothy Preston-Marshall
 *
 *  Main code for the ENCE361 step counter project
 *
 *  FitnessThur9-1
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "utils/ustdlib.h"
#include "math.h"
#include "circBufV.h"
#include "ADC_read.h"
#include "adc_hal.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "temp_track.h"
#include "buttons_hal.h"
#include "accl_manager.h"
#include "accl_hal.h"
#include "display_manager.h"
#include "button_manager.h"
#include "semphr.h"
#include "queue.h"
#include "step_counter_main.h"


#define ACCEL_TASK_MS 10
#define TEMP_TASK_MS 100
#define OLED_TASK_MS 100
#define TIME_MS 1000
#define POLL_TASK_MS 10
#define RATE_ACCL_MS 200
#define RATE_DISPLAY_UPDATE_MS 50
#define FLASH_MESSAGE_TIME 3/2 
#define ACCELERATION_PERIOD_MS 10
#define STEP_GOAL_ROUNDING 100
#define STEP_THRESHOLD_HIGH 200000
#define STEP_THRESHOLD_LOW -100000
#define TARGET_DISTANCE_DEFAULT 10
#define POT_SCALE_COEFF 20000/4095 



vector3_t getAcclData (void);
static deviceStateInfo_t deviceState; 
static SemaphoreHandle_t i2cMutex;
static QueueHandle_t tempQueue;
static QueueHandle_t calsQueue;


void initClock (void)
{
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}


unsigned long readCurrentTick(void)
{
    return xTaskGetTickCount();

}


void flashMessage(char* toShow)
{
    deviceState.flashTicksLeft = RATE_DISPLAY_UPDATE_MS * FLASH_MESSAGE_TIME;
    uint8_t i = 0;
    while (toShow[i] != '\0' && i < MAX_STR_LEN) {
        (deviceState.flashMessage)[i] = toShow[i];
        i++;
    }

    deviceState.flashMessage[i] = '\0';
}


void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
    while (true);
}

void PollButPotTask(void* args) 
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = POLL_TASK_MS;
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        btnUpdateState(&deviceState);
        updateButtons();
        uint16_t secondsElapsed = (xLastWakeTime - deviceState.workoutStartTick)/TIME_MS;
        if (checkButton(UP) == PUSHED && deviceState.paused == true) { // START TIMER 
            deviceState.workoutGoTick = secondsElapsed;
            deviceState.paused = false;
        } else if (checkButton(DOWN) == PUSHED && deviceState.paused == false) { // PAUSE TIMER
            deviceState.workoutPausedVal = secondsElapsed - abs(deviceState.workoutPauseTick - deviceState.workoutGoTick);
            deviceState.workoutPauseTick = secondsElapsed;
            deviceState.paused = true;
        }
        
    }
}

void AccelReadProcessTask(void* args) 
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = ACCEL_TASK_MS;
    float a_t;
    float steps;
    
    bool stepHigh = false;
    
    for (;;) {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        adc_hal_start_conversion(1);

        deviceState.newGoal = readADC() * POT_SCALE_COEFF; // Set the new goal value, scaling to give the desired range
        deviceState.newGoal = (deviceState.newGoal / STEP_GOAL_ROUNDING) * STEP_GOAL_ROUNDING; // Round to the nearest 100 steps
        if (deviceState.newGoal == 0) { // Prevent a goal of zero, instead setting to the minimum goal (this also makes it easier to test the goal-reaching code on a small but non-zero target)
            deviceState.newGoal = STEP_GOAL_ROUNDING;
        }
        xSemaphoreTake(i2cMutex, portMAX_DELAY);
        acclProcess();
        xSemaphoreGive(i2cMutex);

        a_t = IIRFilter();
        if ((a_t >= STEP_THRESHOLD_HIGH) && (!stepHigh)) {
            deviceState.stepsTaken++;
            steps = deviceState.stepsTaken;
            xQueueSend(calsQueue, &steps, portMAX_DELAY);
            stepHigh = true;

            if (deviceState.stepsTaken == deviceState.currentGoal && deviceState.flashTicksLeft == 0) {
                flashMessage("Goal reached!");
            }
        } 
        
        if ((a_t <= STEP_THRESHOLD_LOW)&&(stepHigh)) {
            stepHigh = false;
        }

        if (deviceState.stepsTaken == 0) {
            deviceState.workoutStartTick = xLastWakeTime;
        }
    }
}


void TempReadTask(void* args)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = TEMP_TASK_MS;
    float temp;
    for (;;) {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
        xSemaphoreTake(i2cMutex, portMAX_DELAY);
        temp = temperatureData();
        xSemaphoreGive(i2cMutex);
        xQueueSend(tempQueue, &temp, portMAX_DELAY);
        newFeatureTask();
    }
}


void newFeatureTask(void) 
{
    float cals;
    float steps = 0;
    float temperature = 0;
    if (xQueueReceive(tempQueue, &temperature, 0) == pdTRUE) {
        deviceState.temp = temperature;
    }
         
    if (xQueueReceive(calsQueue, &steps, 0) == pdTRUE) {
        cals = steps * 0.04;
        deviceState.cals = cals;
    }
}


void DisplayTask(void* args) 
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = OLED_TASK_MS;

    for (;;) {
        xLastWakeTime = xTaskGetTickCount();
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        if (deviceState.flashTicksLeft > 0) {
            deviceState.flashTicksLeft--;
        }

        
        uint16_t secondsElapsed = (xLastWakeTime - deviceState.workoutStartTick)/TIME_MS;
        deviceState.laps = deviceState.stepsTaken / deviceState.currentGoal;
        displayUpdate(deviceState, secondsElapsed);
        
    } 
    
}


int main(void)
{

    deviceState.displayMode = DISPLAY_STEPS;
    deviceState.stepsTaken = 0;
    deviceState.temp = 0;
    deviceState.currentGoal = TARGET_DISTANCE_DEFAULT;
    deviceState.debugMode = false;
    deviceState.displayUnits= UNITS_SI;
    deviceState.workoutStartTick = 0;
    deviceState.flashTicksLeft = 0;
    deviceState.flashMessage = calloc(MAX_STR_LEN + 1, sizeof(char));
    deviceState.workoutPauseTick = 0;
    deviceState.workoutPausedVal = 0;
    deviceState.workoutGoTick = 0;
    deviceState.paused = false;
    deviceState.laps = 0;
    
    // Init libs
    initClock();
    displayInit();
    btnInit();
    acclInit();
    initADC();
    initTemp();

    i2cMutex = xSemaphoreCreateMutex();
    tempQueue = xQueueCreate(10, sizeof(float));
    calsQueue = xQueueCreate(10, sizeof(u_int32_t));

    if( i2cMutex != NULL )
	{
        xTaskCreate(&PollButPotTask, "PollButPotTask", 128, NULL, 1, NULL);
        xTaskCreate(&TempReadTask, "TempReadTask", 128, NULL, 3, NULL);
        xTaskCreate(&AccelReadProcessTask, "AccelReadProcessTask", 256, NULL, 1, NULL);
        xTaskCreate(&DisplayTask, "DisplayTask", 512, NULL, 1, NULL);
    }
    vTaskStartScheduler();
    vQueueDelete(tempQueue);
    vQueueDelete(calsQueue);
    vSemaphoreDelete(i2cMutex);
    return 0; 
}






