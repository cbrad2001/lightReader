#include "include/sampler.h"
#include "include/potentiometer.h"
#include "include/photoresistor.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

// Function for the thread to sample the light level.
static void *samplingThread(void *vargp);

static pthread_t samplingThreadID;
static pthread_mutex_t historyBufferMutex, historySizeMutex;
static bool isSampling;
static double *historyBuffer;
static size_t historySize, totalSamples;

void Sampler_startSampling(void)
{
    pthread_mutex_init(&historyBufferMutex, NULL);
    pthread_mutex_init(&historySizeMutex, NULL);
    isSampling = true;
    totalSamples = 0;
    pthread_create(&samplingThreadID, NULL, &samplingThread, NULL);
}

void Sampler_stopSampling(void)
{
    printf("Stopping the thread for updating the buffer size for samples.");
    pthread_mutex_destroy(&historyBufferMutex);
    pthread_mutex_destroy(&historySizeMutex);
    isSampling = false;
    pthread_join(samplingThreadID, NULL);
}

void Sampler_setHistorySize(int newSize)
{
    pthread_mutex_lock(&historySizeMutex);
    {
        historySize = newSize;
    }
    pthread_mutex_unlock(&historySizeMutex);

    pthread_mutex_lock(&historyBufferMutex);
    {
        // TODO: update the buffer here
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

double *Sampler_getHistory(int *length)
{
    // A smart pointer would be nice...
    // return historyBuffer;
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
static void *samplingThread(void *vargp)
{
    while (isSampling)
    {
        Sampler_setHistorySize(Pot_getRawValue());

        double voltage = LightRead_getVoltage();
        printf("Light value: %.3f", voltage); // temp
        totalSamples++;
        sleep(1); // busy wait (sleep 1s between samples)
    }
    printf("Sampling has now stopped.\n");
}
