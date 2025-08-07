//*****************************************************************************
//
// temp_track.c - Temperature Tracking Implementation
//
// Author:  Sehyun, Michael
// Last modified:	19/08/2024
//
//*****************************************************************************
#include "temp_track.h"
#include "temp_hal.h"
#include <string.h>


#define TEMP_CONVERSION_FACTOR 0.0625
#define TEMP_RESOLUTION 16
#define SHIFT_CONVERSION 16
#define TEMP_CALIBRAION 3.5

void initTemp(void) 
{
    tempHalRegister();
}

float temperatureData(void)
{
    char sensorData[2] = {0,0};
    tempHalReadRawData(sensorData);
    float temperature = (sensorData[1] << 8) / SHIFT_CONVERSION;
    temperature = temperature * TEMP_CONVERSION_FACTOR;
    temperature = temperature - TEMP_CALIBRAION;
    return temperature;
}
   
