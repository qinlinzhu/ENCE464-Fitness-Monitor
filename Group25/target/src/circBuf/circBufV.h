#ifndef CIRCBUFT_H_
#define CIRCBUFT_H_

// *******************************************************
// 
// circBufV.h
//
// Support for a circular buffer of vector3 (3*int16_t) values on the
//  Tiva processor.
// P.J. Bones UCECE, Tim Preston-Marshall
// Last modified:  3.5.2022
//
// FitnessThur9-1
// 
// *******************************************************
#include <stdint.h>

typedef struct{
    float x;
    float y;
    float z;
} vector3_t;

// *******************************************************
// Buffer structure
typedef struct {
	uint32_t size;		// Number of entries in buffer
	uint32_t windex;	// index for writing, mod(size)
	uint32_t rindex;	// index for reading, mod(size)
	vector3_t *data;	// pointer to the data
} circBufVec_t;


vector3_t* initVecCircBuf (circBufVec_t *buffer, uint32_t size);
void writeVecCircBuf (circBufVec_t *buffer, vector3_t entry);
vector3_t readVecCircBuf (circBufVec_t *buffer);
void freeVecCircBuf (circBufVec_t *buffer);

#endif /*CIRCBUFT_H_*/
