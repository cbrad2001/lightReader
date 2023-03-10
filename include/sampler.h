// sampler.h
// Module to sample light levels in the background (thread).
// It provides access to a reading history of configurable
// length, the average light level, and the number of samples taken.
#ifndef _SAMPLER_H_
#define _SAMPLER_H_

// Begin/end the background thread which samples light levels.
void Sampler_startSampling(void);
void Sampler_stopSampling(void);

//trigger udp shutdown
void Sampler_quit(void);

// Set/get the maximum number of samples to store in the history.
void Sampler_setHistorySize(int newSize);
int Sampler_getHistorySize(void);

// assess the buffer history to count any substantial differences in light reading
int Sampler_analyzeDips(void);

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// If the length > current history size, then the returned history size will be
// trimmed to the current history size.
// Note: provides both data and size to ensure consistency.
double* Sampler_getHistory(int length);

// Returns how many valid samples are currently in the history.
// May be less than the history size if the history is not yet full.
int Sampler_getNumSamplesInHistory();

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void);

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void);

//get every Nth sample
void Sampler_printEveryNth(int n);

// Get a copy of the samples in the order that they were taken.
// Ordered by oldest to newest sample.
// The calling code must call free() on the returned pointer.
// If the length > current history size, then the returned history size will be
// trimmed to the current history size.
double* Sampler_getHistoryInOrder(int length);

#endif