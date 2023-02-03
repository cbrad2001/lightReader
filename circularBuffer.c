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

static void dbl_memset(double *buffer, size_t size){
    double max = INVALID_VAL;
    for (size_t i = 0; i < size; i++){
        buffer[i] = max;
    }
}

void CircBuff_buffInit(circular_buffer *buffer, double size)
{
    buffer->head = 0;
    buffer->historySize = 0; 
    buffer->maxBufferSize = size;
    buffer->validItemCount = 0; 
    double* buff = (double*)malloc(size * sizeof(double));   //dynamic array for circular buffer
    // buffer->tail = size-1; 
    buffer->historyBuffer = buff;
    dbl_memset(buffer->historyBuffer, size);
    printf("Successfully initialized buffer of size %d\n", buffer->maxBufferSize);
}

static status buffer_pushback(circular_buffer *b, double pr_reading)
{
    int next = b->head + 1;      // pointer after write

    if (next >= b->maxBufferSize)  // restart at first index once completely circled 
        next = 0;

    // if (next == b->tail)            // 
    //     return FAIL;

    b->historyBuffer[b->head] = pr_reading;     // add data to buffer
    b->head = next;                             // set front of array

    if (!(b->validItemCount + 1 > b->maxBufferSize))
    {
        b->validItemCount += 1;
    }

    return SUCCESS;
}


void CircBuff_buffFree(circular_buffer *buffer)
{
    buffer->head = 0;
    // buffer->tail = 0; 
    buffer->historySize = 0; 
    buffer->maxBufferSize = 0;
    buffer->validItemCount = 0;
    free(buffer->historyBuffer);
    printf("Successfully freed buffer\n");
}

void CircBuff_buffResize(circular_buffer *buffer, double size)
{
    size_t oldBufSize = buffer->maxBufferSize;
    if (oldBufSize == size) { return; } // don't resize
    
    double *oldBuf = buffer->historyBuffer; 

    size_t minBufSize = (size > oldBufSize) ? oldBufSize : size;

    double *newBuf = (double*)malloc((size+1) * sizeof(double));
    dbl_memset(newBuf, size+1);

    for (size_t i = 0; i < minBufSize; i++)
    {
        newBuf[i] = oldBuf[i];
    }
    buffer->maxBufferSize = size;
    buffer->validItemCount = minBufSize;
    buffer->historyBuffer = newBuf;

    free(oldBuf);
    oldBuf = NULL;
}

void CircBuff_addData(circular_buffer *buffer, double pr_reading)
{
    if (buffer_pushback(buffer,pr_reading)==FAIL)
    {
        printf("Error loading to buffer\n");
        // printf("Final array size: %i\n",Sampler_getNumSamplesInHistory());
    }
}

size_t CircBuff_numValidValues(circular_buffer *buffer) {
    return buffer->validItemCount;
}
