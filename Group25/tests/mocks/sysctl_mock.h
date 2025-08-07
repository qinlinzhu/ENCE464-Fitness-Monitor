#include "fff.h"
#include <stdbool.h>
#include "driverlib/sysctl.h"
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

#define FFF_SYSCTL_FAKES_LIST(FUNC)     \
    FUNC(SysCtlPeripheralEnable)        \
    FUNC(SysCtlPeripheralReady)         


VOID_FUNC(SysCtlPeripheralEnable, uint32_t);

VALUE_FUNC(bool, SysCtlPeripheralReady, uint32_t);