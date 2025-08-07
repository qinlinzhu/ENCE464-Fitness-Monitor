// *******************************************************
// 
// circBufV.c
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
#include "stdlib.h"
#include "circBufV.h"

vector3_t* initVecCircBuf (circBufVec_t *buffer, uint32_t size)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = 
        (vector3_t *) calloc (size, sizeof(vector3_t));
	return buffer->data;
}

void writeVecCircBuf (circBufVec_t *buffer, vector3_t entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}


vector3_t readVecCircBuf (circBufVec_t *buffer)
{
    vector3_t entry;
	
	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	   buffer->rindex = 0;
    return entry;
}


void freeVecCircBuf (circBufVec_t * buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
}

