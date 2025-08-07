#ifndef CIRCBUFF_H_
#define CIRCBUFF_H_

// *******************************************************
// 
// circBufF.h
//
// Support for a circular buffer of float values on the
//  Tiva processor.
// Michael
// 
//
// FitnessThur9-1
// 
// *******************************************************
#include <stdint.h>
#include <stdbool.h>
typedef struct circBufFloat_t{
    uint32_t size;
    uint32_t windex;
    uint32_t rindex;
    float *data;
}circBufFloat_t;


float* initFloatCircBuf (circBufFloat_t *buffer, uint32_t size);
void writeFloatCircBuf (circBufFloat_t *buffer, float entry);
float readFloatCircBuf (circBufFloat_t *buffer);
void freeFloatCircBuf (circBufFloat_t *buffer);

#endif /*CIRCBUFT_H_*/
