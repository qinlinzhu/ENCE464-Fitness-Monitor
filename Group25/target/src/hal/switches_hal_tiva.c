/*
 * Module to read values from the Orbit Board toggle switches
 * Created 21/4/22
 * Author: Daniel Rabbidge
 * Based on the structure of buttons4.c by P.J Bones
 *
 * FitnessThur9-1
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "switches_hal.h"



static bool SW_state;	// Corresponds to the electrical state
static uint8_t SW_count;
static bool SW_flag;
//static bool SW_normal;   // Corresponds to the electrical state

void initSwitch (void)
{

    SysCtlPeripheralEnable (SW1_PERIPH);
    GPIOPinTypeGPIOInput (SW1_PORT_BASE, SW1_PIN);
    GPIOPadConfigSet (SW1_PORT_BASE, SW1_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    SW_state = SW1_NORMAL;
    SW_count = 0;
    SW_flag = false;
}

void updateSwitch (void)
{
    bool SW_value;
    SW_value = (GPIOPinRead (SW1_PORT_BASE, SW1_PIN) == SW1_PIN);


    if (SW_value != SW_state)
    {
        SW_count++;
        if (SW_count >= NUM_SW_POLLS)
        {
            SW_state = SW_value;
            SW_flag = true;    
            SW_count = 0;
        }
    }
    else
        SW_count = 0;

}


bool isSwitchUp(void) {
    return SW_state;
}
