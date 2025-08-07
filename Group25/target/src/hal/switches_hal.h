#ifndef SWITCHES_H_
#define SWITCHES_H_

// *******************************************************
// Header file for switch reading module
// Daniel Rabbidge
//
// FitnessThur9-1
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>


#define SW1_PERIPH  SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE  GPIO_PORTA_BASE
#define SW1_PIN  GPIO_PIN_7
#define SW1_NORMAL  false

#define NUM_SW_POLLS 3

enum SWStates {SW_UP = 0, SW_DOWN, SW_NO_CHANGE};
void initSwitch (void);
void updateSwitch (void);
bool isSwitchUp(void);

#endif /*SWITCHES_H_*/
