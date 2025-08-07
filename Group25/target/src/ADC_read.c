//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones	UCECE
// Last modified:	8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************



#include "ADC_read.h"
#include "adc_hal.h"


void dummy_callback(void) {
    
}
void initADC (void)
{
    ADC_ID id = ADC_ID_1;
    adc_hal_register(id, dummy_callback);
}

