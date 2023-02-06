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

void CircBuff_buffInit(circular_buffer *buffer, size_t size)
{
    buffer->head = 0;
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
    if (b->maxBufferSize == 0)
    {
        return SUCCESS;                          // don't write anything to size 0
    }

    b->historyBuffer[b->head] = pr_reading;      // add data to buffer

    int next = b->head + 1;                      // pointer after write
    if (next >= b->maxBufferSize) {  next = 0; } // restart at first index once completely circled 
    b->head = next;                              // set front of array

    if (!(b->validItemCount + 1 > b->maxBufferSize)) { b->validItemCount += 1; }
    
    return SUCCESS;
}


void CircBuff_buffFree(circular_buffer *buffer)
{
    buffer->head = 0;
    buffer->maxBufferSize = 0;
    buffer->validItemCount = 0;
    free(buffer->historyBuffer);
    printf("Successfully freed buffer\n");
}

void CircBuff_buffResize(circular_buffer *buffer, size_t size)
{
    // use of int to avoid underflow from counting backwards
    int oldBufSize = buffer->maxBufferSize;
    if (size < oldBufSize + 2 && size > oldBufSize - 2) { return; }    // hysteresis value of +-2

    double *oldBuf = buffer->historyBuffer;

    if (size == 0)
    {
        buffer->head = 0;
        buffer->maxBufferSize = 0;
        buffer->validItemCount = 0;
        buffer->historyBuffer = NULL;
        free(oldBuf);
        oldBuf = NULL;
        return;
    }

    double *newBuf = (double*)malloc(size * sizeof(double));
    dbl_memset(newBuf, size);

    int destPos = (size > oldBufSize) ? (oldBufSize - 1) : (size - 1);
    int sourcePos = buffer->head;
    if (size > oldBufSize)
    {
        for (size_t i = 0; i < oldBufSize; i++)
        {
            newBuf[i] = oldBuf[i];
        }
    }
    else // take the most recent M samples on buffer size decrease
    {
        while (destPos >= 0)
        {
            newBuf[destPos] = oldBuf[sourcePos];
            if (sourcePos == 0)
            {
                sourcePos = oldBufSize - 1; // loop back to end of array and continue copying
            }
            else
            {
                sourcePos -= 1;
            }
            destPos -= 1;
        }
    }

    if (size > oldBufSize)
    {
        if (oldBufSize > 0)
        {
            buffer->head = oldBufSize - 1;
        }
        else // if oldBufSize == 0
        {
            buffer->head = 0;
        }
        buffer->validItemCount = oldBufSize;
    }
    else
    {
        buffer->head = 0; // buffer is full so just start from the front
        buffer->validItemCount = size;
    }
    buffer->maxBufferSize = size;
    buffer->historyBuffer = newBuf;
    free(oldBuf);
    oldBuf = NULL;
}

void CircBuff_addData(circular_buffer *buffer, double pr_reading)
{
    if (buffer_pushback(buffer,pr_reading) == FAIL)
    {
        printf("Error loading to buffer\n");
    }
}

size_t CircBuff_numValidValues(circular_buffer *buffer) 
{
    return buffer->validItemCount;
}

double* CircBuff_getDoubleCopy(circular_buffer *buffer, int length)
{
    if (length == 0)
    {
        return NULL;
    }

    if (length > buffer->validItemCount)
    {
        length = buffer->validItemCount;
    }

    double* toReturn = (double*)malloc(length*sizeof(double));
    for (size_t i = 0; i < length; i++)
    {
        toReturn[i] = buffer->historyBuffer[i];
    }


    return toReturn;
}

double* CircBuff_getCopyInOrder(circular_buffer *buffer, int length)
{
    if (length == 0)
    {
        return NULL;
    }

    if (length > buffer->validItemCount)
    {
        length = buffer->validItemCount;
    }

    double* toReturn = (double*)malloc(length*sizeof(double));
    int sourcePos = buffer->head;
    int destPos = length - 1;
    while (destPos >= 0) 
    {
        toReturn[destPos] = buffer->historyBuffer[sourcePos];
        
        if (sourcePos == 0) { sourcePos = buffer->maxBufferSize - 1; }
        else { sourcePos -= 1; }

        destPos -= 1;
    }

    return toReturn;
}
