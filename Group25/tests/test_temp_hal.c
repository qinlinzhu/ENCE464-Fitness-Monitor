#include "unity.h"
#include "temp_hal.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#include "circBufT_mock.h"
#include "tiva_mocks/adc_mock.h"
#include "sysctl_mock.h"
#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/i2c_driver_mock.h"
#include "i2c_driver.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"



void reset_fff(void)
{
    FFF_ADC_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_CIRCBUFT_FAKES_LIST(RESET_FAKE);
    FFF_GPIO_FAKES_LIST(RESET_FAKE);
    FFF_I2C_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
}

void tearDown(void)
{
    
}

void test_temp_hal_enable_peripheral(void) 
{
    // Act
    tempHalRegister();

    // Arrange
    TEST_ASSERT_EQUAL(1, SysCtlPeripheralEnable_fake.call_count);

}

void test_temp_hal_GPIO_type(void)
{
    // Act 
    tempHalRegister();

    // Arrange
    TEST_ASSERT_EQUAL(1, GPIOPinTypeI2C_fake.call_count);
    TEST_ASSERT_EQUAL(I2CSDAPort, GPIOPinTypeI2C_fake.arg0_val);
    TEST_ASSERT_EQUAL(I2CSDA_PIN, GPIOPinTypeI2C_fake.arg1_val);
    TEST_ASSERT_EQUAL(1, GPIOPinTypeI2CSCL_fake.call_count);
    TEST_ASSERT_EQUAL(I2CSCLPort, GPIOPinTypeI2CSCL_fake.arg0_val);
    TEST_ASSERT_EQUAL(I2CSCL_PIN, GPIOPinTypeI2CSCL_fake.arg1_val);
}

void test_temp_hal_GPIO_configure(void)
{
    // Act
    tempHalRegister();

    // Arrange
    TEST_ASSERT_EQUAL(1, GPIOPinConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(I2CSCL, GPIOPinConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(I2CSDA, GPIOPinConfigure_fake.arg0_val);
}

void test_temp_hal_I2C_initialise() 
{
    // Act
    tempHalRegister();

    // Arrange
    TEST_ASSERT_EQUAL(1, I2CMasterInitExpClk_fake.call_count);
    TEST_ASSERT_EQUAL(I2C0_BASE, I2CMasterInitExpClk_fake.arg0_val);
    TEST_ASSERT_EQUAL(1, I2CMasterInitExpClk_fake.arg2_val);
}

void test_temp_hal_I2C_clear()
{
    // Act
    tempHalRegister();

    // Arrange
    TEST_ASSERT_EQUAL(1, I2CMasterControl_fake.call_count);
    TEST_ASSERT_EQUAL(I2C0_BASE, I2CMasterControl_fake.arg0_val);
    TEST_ASSERT_EQUAL(I2C_MASTER_CMD_BURST_RECEIVE_FINISH, I2CMasterControl_fake.arg1_val);
}

void test_temp_hal_I2C_transmit()
{
    TEST_ASSERT_EQUAL(1, I2CGenTransmit_fake.call_count);
    TEST_ASSERT_EQUAL(WRITE, I2CGenTransmit_fake.arg1_val);
    TEST_ASSERT_EQUAL(0x4F, I2CGenTransmit_fake.arg2_val);
}

