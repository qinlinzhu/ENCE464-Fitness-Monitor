#include "unity.h"
#include "accl_hal.h"
#include "accl_manager.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#include "circBufT_mock.h"
#include "tiva_mocks/adc_mock.h"
#include "tiva_mocks/sysctl_mock.h"
#include "tiva_mocks/gpio_mock.h"
#include "tiva_mocks/i2c_driver_mock.h"
#include "i2c_driver.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"