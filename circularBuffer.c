#include "include/circularBuffer.h"

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

void CircBuff_buffInit(circular_buffer *buffer, int size)
{
    b->head = 0;
    b->tail = 0; 
    b->historySize = 0; 
    b->historyBuffer = (double *)malloc(size * sizeof(double));   //dynamic array for circular buffer
    printf("successfully initialized buffer of size %d\n", size);
}

void CircBuff_addData(circular_buffer *buffer, double pr_reading)
{
    if (buffer_pushback(buffer,pr_reading)==FAIL)
    {
        printf("Error loading to buffer\n");
        printf("Final array size: %i\n",Sampler_getNumSamplesInHistory());
    }
}
