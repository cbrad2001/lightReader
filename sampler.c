#include "include/sampler.h"
#include "include/potentiometer.h"
#include "include/photoresistor.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

// Function for the thread to sample the light level.
static void* samplingThread(void *vargp);

static pthread_t samplingThreadID;
static pthread_mutex_t historyBufferMutex, historySizeMutex;
static bool isSampling;
static double *historyBuffer;
static size_t historySize;
static long long totalSamples;

void Sampler_startSampling(void)
{
    pthread_mutex_init(&historyBufferMutex, NULL);
    pthread_mutex_init(&historySizeMutex, NULL);
    isSampling = true;
    totalSamples = 0;

    historyBuffer = (double *)malloc(Pot_getRawValue() * sizeof(double));   //dynamic array for circular buffer

    pthread_create(&samplingThreadID, NULL, &samplingThread, NULL);
}

void Sampler_stopSampling(void)
{
    printf("Stopping the thread for updating the buffer size for samples.");
    pthread_mutex_destroy(&historyBufferMutex);
    pthread_mutex_destroy(&historySizeMutex);
    isSampling = false;

    free(historyBuffer);            

    pthread_join(samplingThreadID, NULL);
}

void Sampler_setHistorySize(int newSize)
{
    pthread_mutex_lock(&historySizeMutex);
    {
        historySize = newSize;
    }
    pthread_mutex_unlock(&historySizeMutex);

    double *newBuffer = Sampler_getHistory(Pot_getRawValue());
    pthread_mutex_lock(&historyBufferMutex);
    {
        // TODO: update the buffer here
        double *oldBuffer = historyBuffer;
        free(oldBuffer);
        historyBuffer = newBuffer;
    }
    pthread_mutex_unlock(&historyBufferMutex);
}

int Sampler_getHistorySize(void)
{
    int toReturn;
    pthread_mutex_lock(&historySizeMutex);
    {
        toReturn = historySize;
    }
    pthread_mutex_unlock(&historySizeMutex);
    return toReturn;
}

double* Sampler_getHistory(int length)
{
    int checkedLength;

    if (length > Sampler_getHistorySize())
    {
        checkedLength = Sampler_getHistorySize();
    }
    else
    {
        checkedLength = length;
    }

    // A smart pointer would be nice...
    double *historyCopy = malloc(checkedLength * sizeof(double));
    for (int i = 0; i < checkedLength; i++)
    {
        historyCopy[i] = historyBuffer[i];
    }
    return historyCopy;
}

int Sampler_getNumSamplesInHistory()
{
    int taken = Sampler_getNumSamplesTaken();
    int hsize = Sampler_getHistorySize();
    
    return (taken > hsize) ? taken : hsize;

}

double Sampler_getAverageReading(void)
{

    //improve for "Exponential smoothing"... should be covered next lecture... this is just a temporary stopgap
    double sum,avg = 0;
    for (int i = 0; i < Sampler_getHistorySize(); i++){
        sum += historyBuffer[i];
    }
    avg = (sum / Sampler_getNumSamplesInHistory());

    // weights previous average at 99.9% 
    // avg = avg * 0.999

    return avg;
}

long long Sampler_getNumSamplesTaken(void)
{
    return totalSamples;
}

// Private thread implementations
static void* samplingThread(void *vargp)
{
    while (isSampling)
    {
        Sampler_setHistorySize(Pot_getRawValue());  // count potentiometer value 
        totalSamples++;

    }
    printf("Sampling has now stopped.\n");
    return 0;
}
