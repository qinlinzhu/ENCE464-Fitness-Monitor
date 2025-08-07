#include "fff.h"
#include <stdbool.h>
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "i2c_driver.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_I2C_FAKES_LIST(FUNC)       \
    FUNC(I2CMasterInitExpClk)           \
    FUNC(I2CMasterControl)              \
    FUNC(I2CGenTransmit)                \
    
typedef void(*void_function_ptr_type)(void);


VOID_FUNC(I2CMasterInitExpClk, uint32_t, uint32_t, bool);
VOID_FUNC(I2CMasterControl, uint32_t, uint32_t);
VALUE_FUNC(char, I2CGenTransmit, char *, int32_t, bool, char);
