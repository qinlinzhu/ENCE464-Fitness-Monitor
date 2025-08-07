/*
 * button_manager.h
 *
 *  Created on: 1/04/2022
 *      Author: Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#ifndef BUTTON_MANAGER_H_
#define BUTTON_MANAGER_H_
#include "step_counter_main.h"


void btnInit(void);
void btnUpdateState(deviceStateInfo_t* deviceStateInfo);

#endif /* BUTTON_MANAGER_H_ */
