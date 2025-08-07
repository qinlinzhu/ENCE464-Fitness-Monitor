#include "fff.h"
#include "circBufF.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_CIRCBUFF_FAKES_LIST(FUNC)         \
    FUNC(initFloatCircBuf)                  \
    FUNC(writeFloatCircBuf)                 \
    FUNC(readFloatCircBuf)                  \
    FUNC(freeFloatCircBuf)

VALUE_FUNC(float*, initFloatCircBuf, circBufFloat_t*, uint32_t);
VOID_FUNC(writeFloatCircBuf, circBufFloat_t*, float);
VALUE_FUNC(float, readFloatCircBuf, circBufFloat_t*);
VOID_FUNC(freeFloatCircBuf, circBufFloat_t*);

