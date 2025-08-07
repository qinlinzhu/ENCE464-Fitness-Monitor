#ifndef CIRCBV_MOCK_H_
#define CIRCBV_MOCK_H_

#include "fff.h"
#include "circBufV.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_CIRCBUFV_FAKES_LIST(FUNC)          \
    FUNC(initVecCircBuf)                    \
    FUNC(writeVecCircBuf)                   \
    FUNC(readVecCircBuf)                    \
    FUNC(freeVecCircBuf)                   

VALUE_FUNC(vector3_t *, initVecCircBuf, circBufVec_t *, uint32_t);
VOID_FUNC(writeVecCircBuf, circBufVec_t *, vector3_t);
VALUE_FUNC(vector3_t, readVecCircBuf, circBufVec_t *);
VOID_FUNC(freeVecCircBuf, circBufVec_t *);

#endif /* CIRCBV_MOCK_H_ */
