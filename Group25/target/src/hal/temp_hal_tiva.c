//*****************************************************************************
//
// temp_hal_tiva.c - Hardware Abstraction Layer for temp sensor to encapsulate tiva hardware
//
// Author:  Sehyun, Michael
// Last modified:	19/08/2024
//
//*****************************************************************************

#include "temp_track.h"
#include "adc_hal.h"
#include "circBufT.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "i2c_driver.h"
#include "temp_hal.h"


#define TEMP_ADDR 0x4F

void tempHalRegister(void) 
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    char configData[] = {0x01, 0x60};   
    I2CGenTransmit(configData, 2, WRITE, TEMP_ADDR);

}

void tempHalReadRawData(char* sensorData)
{
    char command[1] = {0x00};
    I2CGenTransmit(command, 1, WRITE, TEMP_ADDR);
    I2CGenTransmit(sensorData, 2, READ, TEMP_ADDR);
}

