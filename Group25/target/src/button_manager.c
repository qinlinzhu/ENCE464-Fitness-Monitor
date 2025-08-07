/*
 * Button_management.c
 *
 * Modifies the device's state according to the user's button and switch input
 *
 *  Created on: 31/03/2022
 *      Author: Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"
#include "buttons_hal.h"
#include "display_manager.h"
#include "button_manager.h"
#include "switches_hal.h"

#define LONG_PRESS_CYCLES 20

static uint16_t longPressCount = 0;
static bool allowLongPress = true;


void btnInit(void)
{
    initButtons();
    initSwitch();
}

void btnUpdateState(deviceStateInfo_t* deviceStateInfo)
{
    updateButtons();
    updateSwitch();

    displayMode_t currentDisplayMode = deviceStateInfo ->displayMode;

    // Changing screens
    if (checkButton(LEFT) == PUSHED) {
        deviceStateInfo -> displayMode = (deviceStateInfo -> displayMode + 1) % DISPLAY_NUM_STATES;      //flicker when pressing button

    } else if (checkButton(RIGHT) == PUSHED) {
        // Can't use mod, as enums behave like an unsigned int, so (0-1)%n != n-1
        if (deviceStateInfo -> displayMode > 0) {
            deviceStateInfo -> displayMode--;
        } else {
            deviceStateInfo -> displayMode = DISPLAY_NUM_STATES-1;
        }
    }

    // Enable/Disable test mode
    if (isSwitchUp()) {
        deviceStateInfo -> debugMode = true;
    } else {
        deviceStateInfo -> debugMode = false;
    }


    // Usage of UP and DOWN buttons
    if (deviceStateInfo -> debugMode) {
        // TEST MODE OPERATION
        if (checkButton(UP) == PUSHED) {
            deviceStateInfo -> stepsTaken = deviceStateInfo -> stepsTaken + DEBUG_STEP_INCREMENT;
        }

        if (checkButton(DOWN) == PUSHED) {
            if (deviceStateInfo -> stepsTaken >= DEBUG_STEP_DECREMENT) {
                deviceStateInfo -> stepsTaken = deviceStateInfo -> stepsTaken - DEBUG_STEP_DECREMENT;
            } else {
                deviceStateInfo -> stepsTaken = 0;
            }
        }


    } else {
        if (checkButton(UP) == PUSHED) {
            if (deviceStateInfo -> displayUnits == UNITS_SI) {
                deviceStateInfo -> displayUnits = UNITS_ALTERNATE;
            } else {
                deviceStateInfo -> displayUnits = UNITS_SI;
            }
        }

        // Resetting steps and updating goal with long and short presses
        if ((isDown(DOWN) == true) && (currentDisplayMode != DISPLAY_SET_GOAL) && (allowLongPress)) {
            longPressCount++;
            if (longPressCount >= LONG_PRESS_CYCLES) {
                deviceStateInfo -> stepsTaken = 0;
                deviceStateInfo -> paused = false;
                deviceStateInfo -> workoutGoTick = 0;
                deviceStateInfo -> workoutPauseTick = 0;
                deviceStateInfo -> cals = 0;

                flashMessage("Reset!");
            }
        } else {
            if ((currentDisplayMode == DISPLAY_SET_GOAL) && checkButton(DOWN) == PUSHED) {
                deviceStateInfo -> currentGoal = deviceStateInfo -> newGoal;
                deviceStateInfo -> displayMode = DISPLAY_STEPS;
                

                allowLongPress = false; // Hacky solution: Protection against double-registering as a short press then a long press
            }
            longPressCount = 0;
        }

        if (checkButton(DOWN) == RELEASED) {
            allowLongPress = true;
        }


    }

}


