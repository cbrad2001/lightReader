#include "include/sampler.h"
#include "include/potentiometer.h"
#include "include/photoresistor.h"
#include "include/circularBuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define EXPONENTIAL_WEIGHTING_VALUE 0.5

// Function for the thread to sample the POT and update the buffer size.
static void* potThread(void *vargp);

// Function for the thread to sample the photoresistor and add the reading to the buffer.
static void* lightSamplingThread(void *vargp);

static pthread_t potThreadID, lightThreadID;
static pthread_mutex_t historyBufferMutex, historySizeMutex;
static bool isSampling;
// static double *historyBuffer;
// static size_t historySize;
static long long totalSamples;
static circular_buffer buffer;

static double filtered_average;     //average value based on exponential smoothing  (v.n)

void Sampler_startSampling(void)
{
    pthread_mutex_init(&historyBufferMutex, NULL);
    pthread_mutex_init(&historySizeMutex, NULL);

    isSampling = true;
    totalSamples = 0;

    CircBuff_buffInit(&buffer, Pot_getRawValue());

    pthread_create(&potThreadID, NULL, &potThread, NULL);
    pthread_create(&lightThreadID, NULL, &lightSamplingThread, NULL);    
}

void Sampler_stopSampling(void)
{
    printf("Stopping the thread for sampling light level.\n");
    pthread_mutex_destroy(&historyBufferMutex);
    pthread_mutex_destroy(&historySizeMutex);
    isSampling = false;

    pthread_join(lightThreadID, NULL);
    pthread_join(potThreadID, NULL);

    CircBuff_buffFree(&buffer);  
}

void Sampler_setHistorySize(int newSize)
{
    pthread_mutex_lock(&historySizeMutex);
    {
        buffer.historySize = newSize;
    }
    pthread_mutex_unlock(&historySizeMutex);

    pthread_mutex_lock(&historyBufferMutex);
    {
        CircBuff_buffResize(&buffer, newSize);
    }
    pthread_mutex_unlock(&historyBufferMutex);
}

int Sampler_getHistorySize(void)
{
    int toReturn;
    pthread_mutex_lock(&historySizeMutex);
    {
        toReturn = buffer.historySize;
    }
    pthread_mutex_unlock(&historySizeMutex);
    return toReturn;
}

double* Sampler_getHistory(int length)
{
    // TODO: change the implementation to be in terms of the circular buffer
    // Approach: define a getter function in the circularBuffer module
    // that returns the double array from the order of head to tail
    // The 373 in me is saying that Sampler module shouldn't know how to work with circular_buffer
    // int checkedLength;

    // if (length > Sampler_getHistorySize())
    // {
    //     checkedLength = Sampler_getHistorySize();
    // }
    // else
    // {
    //     checkedLength = length;
    // }

    // // A smart pointer would be nice...
    // double *historyCopy = malloc(checkedLength * sizeof(double));
    // for (int i = 0; i < checkedLength; i++)
    // {
    //     historyCopy[i] = buffer.historyBuffer[i];
    // }
    // return historyCopy;

    // STUB
    return NULL;
}

int Sampler_getNumSamplesInHistory()
{
    int taken = Sampler_getNumSamplesTaken();
    int hsize = Sampler_getHistorySize();
    
    return (taken > hsize) ? taken : hsize;

}

// vn = a*sn + (1-a) * v.(n-1)
static double exponential_smoothing(double sample_n, double prev_filter_value){
    double current_weighted = EXPONENTIAL_WEIGHTING_VALUE * sample_n;
    double history_weighted = (1-EXPONENTIAL_WEIGHTING_VALUE)*prev_filter_value;
    double new_avg = current_weighted + history_weighted;
    return new_avg;
}

static void update_Average_Reading(double newestSample){
    //first case v.0 = s.0
    if(!filtered_average){ 
        filtered_average = newestSample;    //first case (average is the only value)
    }

    filtered_average = exponential_smoothing(newestSample,filtered_average);
}

double Sampler_getAverageReading(void)
{
    return filtered_average;


    // double sum,avg = 0;
    // for (int i = 0; i < Sampler_getHistorySize(); i++){
    //     sum += buffer.historyBuffer[i];
    // }
    // avg = (sum / Sampler_getNumSamplesInHistory());

    // // weights previous average at 99.9% 
    // // avg = avg * 0.999

    // return avg;
}

long long Sampler_getNumSamplesTaken(void)
{
    return totalSamples;
}

static void* potThread(void *vargp)
{
    //change 1 to isSampling
    while (1)
    {
        printf("Raw potentiometer value: %.3i\n", Pot_getRawValue());
        Sampler_setHistorySize(Pot_getVoltage());  // count potentiometer value

        buffer.tail = Sampler_getHistorySize()-1;
    }
    printf("Sampling of POT has now stopped.\n");
    return 0;
}

static void* lightSamplingThread(void *vargp)
{
    
    while(1){
        
        double current_lightRead_voltage = LightRead_getVoltage();
        CircBuff_addData(&buffer, current_lightRead_voltage);

        update_Average_Reading(current_lightRead_voltage);              //keep track of average
            
        //put the reading into the buffer
        printf("Light value: %.4f\n", current_lightRead_voltage);       // temp for visual output
        sleep(0.001);                                                   // between light samples, sleep for 1ms 
    }
    printf("Sampling of photoresistor has now stopped.\n");
    return 0;

}
