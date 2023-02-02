#include "include/circularBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef enum status
{
    SUCCESS,
    FAIL
} status;

static status buffer_pushback(circular_buffer *b, double pr_reading)
{
    size_t next = b->head + 1; //pointer after write
    if (next >= b->historySize)
        next = 0;
    
    if (next == b->tail)
        return FAIL;

    b->historyBuffer[b->head] = pr_reading;
    b->head = next;
    return SUCCESS;
}

void CircBuff_buffInit(circular_buffer *buffer, size_t size)
{
    buffer->head = 0;
    buffer->tail = 0; 
    buffer->historySize = 0; 
    buffer->maxBufferSize = size;
    buffer->historyBuffer = (double*)malloc(size * sizeof(double));   //dynamic array for circular buffer
    memset(buffer->historyBuffer, -DBL_MAX, size);
    printf("successfully initialized buffer of size %d\n", size);
}

void CircBuff_buffFree(circular_buffer *buffer)
{
    buffer->head = 0;
    buffer->tail = 0; 
    buffer->historySize = 0; 
    buffer->maxBufferSize = 0;
    free(buffer->historyBuffer);
    printf("successfully freed buffer\n", );
}

void CircBuff_buffResize(circular_buffer *buffer, size_t size)
{
    double *oldBuf = buffer->historyBuffer;
    double *newBuf = (double*)malloc(size * sizeof(double));
    memset(newBuf, -DBL_MAX, size);
    size_t oldBufSize = buffer->maxBufferSize;

    for (size_t i = 0; i < oldBufSize; i++)
    {
        newBuf[i] = oldBuf[i];
    }
    free(oldBuf);
}

void CircBuff_addData(circular_buffer *buffer, double pr_reading)
{
    if (buffer_pushback(buffer,pr_reading)==FAIL)
    {
        printf("Error loading to buffer\n");
        printf("Final array size: %i\n",Sampler_getNumSamplesInHistory());
    }
}
