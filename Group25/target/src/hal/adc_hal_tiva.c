//*****************************************************************************
//
// adc_hal_tiva.c - Hardware Abstraction Layer for adc to encapsulate tiva hardware
//
// Author:  Sehyun
// Last modified:	13/08/2024
//
//*****************************************************************************



#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "circBufT.h"
#include "adc_hal.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ADC_BUF_SIZE 10

//*****************************************************************************
// Global variables
//*****************************************************************************

static circBuf_t ADC_inBuffer_adc;		// Buffer of size BUF_SIZE integers (sample values)
static circBuf_t ADC_inBuffer_temp;		// Buffer of size BUF_SIZE integers (sample values)
// enum ADC_ID adc_id = 1;
uint32_t ADC_BASE;
uint32_t SYSCTL_PERIPH;
uint32_t ADC_CTL;



void adc_hal_start_conversion(ADC_ID adc_id) 
{
    //
    // Initiate a conversion
    //
    if (adc_id == 1) 
    {
        ADCProcessorTrigger(ADC0_BASE, 3);
    } else if (adc_id == 2) {
        ADCProcessorTrigger(ADC1_BASE, 3);
    } // Invalid ID --> no ADC is triggered
    

}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void ADCIntHandler()
{
	uint32_t ulValue;
    

    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
       

    writeCircBuf (&ADC_inBuffer_adc, ulValue);
    
    ADCIntClear(ADC0_BASE, 3); 
                        
}

void TempIntHandler()
{
    uint32_t ulValue;

    ADCSequenceDataGet(ADC1_BASE, 3, &ulValue);   

    writeCircBuf (&ADC_inBuffer_temp, ulValue);

    ADCIntClear(ADC1_BASE, 3); 
}


void adc_hal_register(ADC_ID adc_id, callback cb) {
    // adc_id = id;
    

    
    if (adc_id == 1) {
        ADC_BASE = ADC0_BASE;
        SYSCTL_PERIPH = SYSCTL_PERIPH_ADC0;
        ADC_CTL = ADC_CTL_CH0;
        initCircBuf (&ADC_inBuffer_adc, ADC_BUF_SIZE);
        cb = ADCIntHandler;
    } else if (adc_id == 2) { // TEMPERATURE TRACKING 
        ADC_BASE = ADC1_BASE;
        SYSCTL_PERIPH = SYSCTL_PERIPH_ADC1;
        ADC_CTL = ADC_CTL_TS;
        initCircBuf (&ADC_inBuffer_temp, ADC_BUF_SIZE);
        cb = TempIntHandler;
    } 

    
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH)); // For use only when using Tivaware (Gets stuck in infinite loop during test)
    
   
    ADCSequenceConfigure(ADC_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  
    
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC_BASE, 3, 0, ADC_CTL | ADC_CTL_IE |
                             ADC_CTL_END);    
                             
    
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC_BASE, 3);
  
    
    // Register the interrupt handler
    ADCIntRegister (ADC_BASE, 3, cb);
    // ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
    // ADCIntRegister (ADC1_BASE, 3, TempIntHandler);
  
    
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC_BASE, 3);
}

uint32_t readADC() {
    uint32_t sum = 0;
    for (uint16_t i = 0; i < ADC_BUF_SIZE; i++)
        sum += readCircBuf (&ADC_inBuffer_adc);

    return (sum + (ADC_BUF_SIZE / 2 ))/ADC_BUF_SIZE;
}

uint32_t readTemp() {
    uint32_t sum = 0;
    for (uint16_t i = 0; i < ADC_BUF_SIZE; i++)
        sum += readCircBuf (&ADC_inBuffer_temp);

    return (sum + (ADC_BUF_SIZE / 2 ))/ADC_BUF_SIZE;

}