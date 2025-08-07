#ifndef TEMP_HAL_H
#define TEMP_HAL_H

#include <stdint.h>


void tempHalRegister(void);
void tempHalReadRawData(char* sensorData);

#endif //TEMP_HAL_H
