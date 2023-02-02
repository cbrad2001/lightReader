#include "include/circularBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

typedef enum status
{
    SUCCESS,
    FAIL
} status;

static status buffer_pushback(circular_buffer *b, double pr_reading)
{
    size_t next = b->head + 1;      // pointer after write
    if (next >= b->maxBufferSize)   // restart at first index once completely circled 
        next = 0;
    
    printf("next: %i\n",next);


    // if (next == b->tail)            // 
    //     return FAIL;

    b->historyBuffer[b->head] = pr_reading;     // add data to buffer
    b->head = next;                             // set front of array
    return SUCCESS;
}

static void dbl_memset(double *buffer, size_t size){
    for (int i = 0; i < size; i++){
        buffer[i] = -DBL_MAX;
    }
}

void CircBuff_buffInit(circular_buffer *buffer, size_t size)
{
    buffer->head = 0;
    buffer->historySize = 0; 
    buffer->maxBufferSize = size;
    // buffer->tail = size-1; 
    buffer->historyBuffer = (double*)malloc(size * sizeof(double));   //dynamic array for circular buffer
    dbl_memset(buffer->historyBuffer, size);
    printf("Successfully initialized buffer of size %d\n", size);
}

void CircBuff_buffFree(circular_buffer *buffer)
{
    buffer->head = 0;
    // buffer->tail = 0; 
    buffer->historySize = 0; 
    buffer->maxBufferSize = 0;
    free(buffer->historyBuffer);
    printf("Successfully freed buffer\n");
}

void CircBuff_buffResize(circular_buffer *buffer, size_t size)
{
    double *oldBuf = buffer->historyBuffer;
    size_t oldBufSize = buffer->maxBufferSize;

    if (oldBufSize == size) { return; } // don't resize

    size_t minBufSize = (size > oldBufSize) ? oldBufSize : size;

    double *newBuf = (double*)malloc((size + 1) * sizeof(double));
    dbl_memset(buffer->historyBuffer, size);

    for (size_t i = 0; i < minBufSize; i++)
    {
        newBuf[i] = oldBuf[i];
    }
    buffer->maxBufferSize = size;

    free(oldBuf);
}

void CircBuff_addData(circular_buffer *buffer, double pr_reading)
{
    if (buffer_pushback(buffer,pr_reading)==FAIL)
    {
        printf("Error loading to buffer\n");
        // printf("Final array size: %i\n",Sampler_getNumSamplesInHistory());
    }
}
