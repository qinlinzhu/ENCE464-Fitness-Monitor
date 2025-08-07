// *******************************************************
// 
// circBufT.c
//
// Support for a circular buffer of uint32_t values on the 
//  Tiva processor.
// P.J. Bones UCECE
// Last modified:  8.3.2017
// 
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circBufT.h"


int32_t *
initCircBuf (circBuf_t *buffer, uint32_t size)
{
	if (size == 0 || size > MAX_BUFFER_CAPACITY)
    {
        return NULL;
    }

	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = 
        (int32_t *) calloc (size, sizeof(int32_t));
	buffer->full = false;
	return buffer->data;
}

void
writeCircBuf (circBuf_t *buffer, int32_t entry)
{
	if (buffer->full)
    {
        return;
    }

	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
	
	if (buffer->windex == buffer->rindex)
    {
        buffer->full = true;
    }
}


int32_t
readCircBuf (circBuf_t *buffer)
{	
	int32_t entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	{
		buffer->rindex = 0;
	}
	   
	buffer->full = false;
    return entry;
}


void
freeCircBuf (circBuf_t * buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
	buffer->full = false;
}

