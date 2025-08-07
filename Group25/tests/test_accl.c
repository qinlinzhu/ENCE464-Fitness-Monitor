#include "unity.h"
#include "accl_manager.h"
#include "accl_hal.h"


#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL 

#include "circBufV_mock.h"
#include "circBufF_mock.h"
#include "tiva_mocks/adc_mock.h"
#include "sysctl_mock.h"
#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/i2c_driver_mock.h"

#define TEST_BUF_SIZE 2


void reset_fff(void)
{
    FFF_CIRCBUFV_FAKES_LIST(RESET_FAKE);
    FFF_CIRCBUFF_FAKES_LIST(RESET_FAKE);
    FFF_GPIO_FAKES_LIST(RESET_FAKE);
    FFF_I2C_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

void setUp(void)
{
    reset_fff();
    acclInit(); 
}

void tearDown(void)
{
  
}

void test_acclInit_initializes_buffers(void)
{
    TEST_ASSERT_EQUAL(1, initVecCircBuf_fake.call_count);
    TEST_ASSERT_EQUAL(TEST_BUF_SIZE, initVecCircBuf_fake.arg1_val);
    TEST_ASSERT_EQUAL(1, initFloatCircBuf_fake.call_count);
    TEST_ASSERT_EQUAL(TEST_BUF_SIZE, initFloatCircBuf_fake.arg1_val);
}

void test_acclProcess_stores_data_in_buffer(void)
{
 
}

void test_IIRFilter_corrects_data(void)
{

}


