#ifndef ADC_HAL_H
#define ADC_HAL_H

#include <stdint.h>

//*****************************************************************************
//
// The header file for Hardware Abstraction Layer for adc to encapsulate tiva hardware
//
//*****************************************************************************


typedef enum {
    ADC_ID_1 = 1,
    ADC_ID_2,
    ADC_ID_3
} ADC_ID;

typedef void(*callback)(void);


void ADCIntHandler(void);
void TempIntHandler(void);
void adc_hal_start_conversion(ADC_ID); 
void adc_hal_register(ADC_ID, callback);
uint32_t readADC(void);     // Call to read the averaged readings from the buffer
uint32_t readTemp(void);
#endif //ADC_HAL_H
