// circularBuffer.h
// Provides definition for a circular buffer in this case of holding double values.
#ifndef _CIRC_BUF_H_
#define _CIRC_BUF_H_

#include <stdlib.h>
#include <float.h>

#define INVALID_VAL DBL_MAX

//concept inspired by:
//https://embedjournal.com/implementing-circular-buffer-embedded-c/ 
typedef struct circular_buffer
{
    double* historyBuffer;
    size_t head;
    size_t maxBufferSize;
    size_t validItemCount;
} circular_buffer; 

// Add a data point to the circular buffer
void CircBuff_addData(circular_buffer *buffer, double pr_reading);

// Initialize a circular buffer with size of the given value.
// Values are initialized with value of -DBL_MAX
void CircBuff_buffInit(circular_buffer *buffer, size_t size);

// Frees the dynamically allocated memory used by the circular buffer.
void CircBuff_buffFree(circular_buffer *buffer);

// Resizes a circular buffer to the size given.
// Empty values after resizing all have values of -DBL_MAX
void CircBuff_buffResize(circular_buffer *buffer, size_t size);

// Gets the number of valid values in the buffer.
size_t CircBuff_numValidValues(circular_buffer *buffer);

// Returns a copy of the buffer as a regular array.
double* CircBuff_getDoubleCopy(circular_buffer *buffer, int length);

// Returns a copy of the buffer as a regular array order by samples in ascending time taken.
double* CircBuff_getCopyInOrder(circular_buffer *buffer, int length);

#endif
