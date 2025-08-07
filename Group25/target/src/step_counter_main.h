/*
 * display_manager.h
 *
 *  Created on: 10/04/2022
 *      Author: Matthew Suter, Tim Preston-Marshall, Daniel Rabbidge
 *
 *  FitnessThur9-1
 */

#ifndef STEP_COUNTER_MAIN_H_
#define STEP_COUNTER_MAIN_H_

#define M_PER_STEP 9/10
#define MAX_STR_LEN 16

#define DEBUG_STEP_INCREMENT 100
#define DEBUG_STEP_DECREMENT 500
typedef enum {
    DISPLAY_STEPS = 0,
    DISPLAY_DISTANCE,
    DISPLAY_SET_GOAL,
    DISPLAY_TEMPERATURE,
    DISPLAY_CALS,
    DISPLAY_STOPWATCH_LAPS,
    DISPLAY_NUM_STATES, 
} displayMode_t;


typedef enum {
    UNITS_SI = 0,     
    UNITS_ALTERNATE,   
    UNITS_NUM_TYPES,
} displayUnits_t;


typedef struct {
    uint32_t stepsTaken;
    uint32_t currentGoal;  
    uint32_t newGoal;          
    uint32_t temp;            
    float cals;
    bool debugMode;             
    displayMode_t displayMode;
    displayUnits_t displayUnits;
    unsigned long workoutStartTick;
    unsigned long workoutPauseTick;
    unsigned long workoutGoTick;
    unsigned long workoutPausedVal;
    uint8_t laps;
    bool paused;
    char *flashMessage;
    unsigned long flashTicksLeft;
} deviceStateInfo_t;



void flashMessage(char* toShow);
void initClock (void);
void initDisplay (void);
void initAccl (void);
void newFeatureTask(void);

#endif /* STEP_COUNTER_MAIN_H_ */
