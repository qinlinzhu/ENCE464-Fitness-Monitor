#ifndef CIRCBUFT_H_
#define CIRCBUFT_H_

// *******************************************************
// 
// circBufT.h
//
// Support for a circular buffer of uint32_t values on the 
//  Tiva processor.
// P.J. Bones UCECE
// Last modified:  7.3.2017
// 
// *******************************************************
#include <stdint.h>
#include <stdbool.h>

#define MIN_CAPACITY 0
#define MAX_BUFFER_CAPACITY 1024

typedef struct circBuf_t {
    uint32_t size;
    uint32_t windex;
    uint32_t rindex;
    uint32_t *data;
    bool full;
} circBuf_t;

int32_t *
initCircBuf (circBuf_t *buffer, uint32_t size);

void writeCircBuf (circBuf_t *buffer, int32_t entry);
int32_t readCircBuf (circBuf_t *buffer);
void freeCircBuf (circBuf_t *buffer);


#endif /*CIRCBUFT_H_*/
