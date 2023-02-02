// circularBuffer.h
// Provides definition for a circular buffer in this case of holding double values.

#ifndef _CIRC_BUF_H_
#define _CIRC_BUF_H_

//https://embedjournal.com/implementing-circular-buffer-embedded-c/ 
typedef struct circular_buffer
{
    double *historyBuffer;
    size_t tail;
    size_t head;
    size_t historySize;
} circular_buffer; 

// Add a data point to the circular buffer
void CircBuff_addData(circular_buffer *buffer, double pr_reading);

// Initialize a circular buffer with size of the given value.
void CircBuff_buffInit(circular_buffer *buffer, int size);

// Resizes a circular buffer to the size given.
void CircBuff_buffResize(circular_buffer *buffer, int size);

#endif
