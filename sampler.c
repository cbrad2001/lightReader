#include "sampler.h"
#include "potentiometer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

static void *bufferSizeThread(void *vargp);
static void *samplingThread(void *vargp);

static pthread_t bufferThreadID, samplingThreadID;
static pthread_mutex_t historyBufferMutex, historySizeMutex;
static bool isUpdatingBuffer;
static double *historyBuffer;
static size_t historySize;


void Sampler_startSampling(void)
{

}

void Sampler_stopSampling(void)
{

}


void Sampler_setHistorySize(int newSize)
{
    pthread_mutex_lock(&historySizeMutex);
    {
        historySize = newSize;
    }
    pthread_mutex_unlock(&historySizeMutex);
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

void Sampler_startBufferSizeUpdate(void)
{
    pthread_mutex_init(&historyBufferMutex, NULL);
    pthread_mutex_init(&historySizeMutex, NULL);
    isUpdatingBuffer = true;
    pthread_create(&bufferThreadID, NULL, bufferSizeThread, NULL);
}

void Sampler_stopBufferSizeUpdate(void)
{
    printf("Stopping the thread for updating the buffer size for samples.");
    pthread_mutex_destroy(&historyBufferMutex);
    pthread_mutex_destroy(&historySizeMutex);
    isUpdatingBuffer = false;
    pthread_join(bufferThreadID, NULL);
}

double* Sampler_getHistory(int *length)
{

}

int Sampler_getNumSamplesInHistory()
{

}


double Sampler_getAverageReading(void)
{

}

long long Sampler_getNumSamplesTaken(void)
{
    
}

// Private thread implementations
static void *bufferSizeThread(void *vargp)
{
    while (isUpdatingBuffer)
    {
        Sampler_setHistorySize(Pot_getRawValue());
        // TODO: update the size of the buffer itself
        usleep(1000);
    }
    printf("POT is now not being sampled.");
}

static void *samplingThread(void *vargp)
{

}
