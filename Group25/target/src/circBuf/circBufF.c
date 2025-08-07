// *******************************************************
// 
// circBufF.c
//
// Support for a circular buffer of float values on the
//  Tiva processor.
// Michael
// 
// 
// 
//
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circBufF.h"

float* initFloatCircBuf (circBufFloat_t *buffer, uint32_t size)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = 
        (float *) calloc (size, sizeof(float));
	return buffer->data;
}

void writeFloatCircBuf (circBufFloat_t *buffer, float entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}

float readFloatCircBuf (circBufFloat_t *buffer)
{
    float entry;
	
	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	   buffer->rindex = 0;
    return entry;
}

void freeFloatCircBuf (circBufFloat_t *buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
}

