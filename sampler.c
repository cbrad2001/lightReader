#include "include/sampler.h"
#include "include/potentiometer.h"
#include "include/photoresistor.h"
#include "include/circularBuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <time.h> // eventually replace by provided code that does not sleep threads
#include <errno.h>

#define EXPONENTIAL_WEIGHTING_VALUE 0.001   //prev average weighed at 99.9%
#define LIGHT_DIP_DIFFERENCE_V 0.1 // volts

// temp test function
static int msleep(long msec);

// Function for the thread to sample the POT and update the buffer size.
static void* potThread(void *vargp);

// Function for the thread to sample the photoresistor and add the reading to the buffer.
static void* lightSamplingThread(void *vargp);

static pthread_t potThreadID, lightThreadID;
static pthread_mutex_t historyBufferMutex;
// static pthread_mutex_t historySizeMutex;
static bool isSampling;
static long long totalSamples;
static circular_buffer buffer;

static double filtered_average;     //average value based on exponential smoothing  (v.n)

void Sampler_startSampling(void)
{
    pthread_mutex_init(&historyBufferMutex, NULL);
    // pthread_mutex_init(&historySizeMutex, NULL);

    isSampling = true;
    totalSamples = 0;

    CircBuff_buffInit(&buffer, Pot_getRawValue());

    pthread_create(&potThreadID, NULL, &potThread, NULL);
    pthread_create(&lightThreadID, NULL, &lightSamplingThread, NULL);    
}

void Sampler_stopSampling(void)
{
    printf("Stopping the thread for sampling light level.\n");
    isSampling = false;
    pthread_mutex_destroy(&historyBufferMutex);
    // pthread_mutex_destroy(&historySizeMutex);

    pthread_join(lightThreadID, NULL);
    pthread_join(potThreadID, NULL);

    CircBuff_buffFree(&buffer);  
}

void Sampler_setHistorySize(int newSize)
{
    // pthread_mutex_lock(&historySizeMutex);
    // {
    //     buffer.historySize = newSize;
    // }
    // pthread_mutex_unlock(&historySizeMutex);

    pthread_mutex_lock(&historyBufferMutex);
    {
        CircBuff_buffResize(&buffer, newSize);
    }
    pthread_mutex_unlock(&historyBufferMutex);
}

int Sampler_getHistorySize(void)
{
    int toReturn;
    pthread_mutex_lock(&historyBufferMutex);
    {
        toReturn = buffer.maxBufferSize;
    }
    pthread_mutex_unlock(&historyBufferMutex);
    return toReturn;
}

double* Sampler_getHistory(int length)
{
    double* bufCopy;
    pthread_mutex_lock(&historyBufferMutex);
    {
        bufCopy = CircBuff_getDoubleCopy(&buffer, length);
    }
    pthread_mutex_unlock(&historyBufferMutex);
    return bufCopy;
}

int Sampler_getNumSamplesInHistory()
{
    int toReturn;
    pthread_mutex_lock(&historyBufferMutex);
    {
        toReturn = (int)CircBuff_numValidValues(&buffer);
    }
    pthread_mutex_unlock(&historyBufferMutex);
    return toReturn;
}


// vn = a*sn + (1-a) * v.(n-1) - this function performs the smoothing calculation
static double exponential_smoothing(double sample_n, double prev_filter_value)
{
    double current_weighted = EXPONENTIAL_WEIGHTING_VALUE * sample_n;
    double history_weighted = (1-EXPONENTIAL_WEIGHTING_VALUE)*prev_filter_value;
    double new_avg = current_weighted + history_weighted;
    return new_avg;
}

static void update_Average_Reading(double newestSample)
{
    //first case v.0 = s.0
    if(!filtered_average){ 
        filtered_average = newestSample;    //first case (average is the only value)
    }

    filtered_average = exponential_smoothing(newestSample,filtered_average);
}

double Sampler_getAverageReading(void)
{
    return filtered_average;
}

int Sampler_analyzeDips()
{
    int dipCount = 0;
    bool dipAvailable = true;
    int bufSize = Sampler_getNumSamplesInHistory();
    double* validHistory = Sampler_getHistory(bufSize);
    double currAvg = Sampler_getAverageReading();

    for (int i = 0; i < bufSize - 1; i++){
        bool isDip = validHistory[i] <= (currAvg - LIGHT_DIP_DIFFERENCE_V);
        if (isDip && dipAvailable)
        {
            dipCount += 1;
            dipAvailable = false;
        }
        else if (!isDip)
        {
            dipAvailable = true;
        }
        // if isDip && !dipAvailable do nothing
    }

    free(validHistory);
    return dipCount;       
}

long long Sampler_getNumSamplesTaken(void)
{
    return totalSamples;
}

void Sampler_printEveryNth(int n){
    printf("Sample values: ");
    for (int i = 0; i < buffer.maxBufferSize; i+=n){
        double val = buffer.historyBuffer[i];
        if (val != INVALID_VAL){
            printf("%.4f\t| ",val);
        }
    }
    printf("\n");
}


static void* potThread(void *vargp)
{
    while (isSampling)
    {
        // printf("Raw potentiometer value: %.3i\n", Pot_getRawValue());
        int sz = Pot_getRawValue();
        Sampler_setHistorySize(sz);  // count potentiometer value
        sleep(1);
    }
    printf("Sampling of POT has now stopped.\n");
    return 0;
}

static void* lightSamplingThread(void *vargp)
{
    while(isSampling){
        
        double current_lightRead_voltage = LightRead_getVoltage();
        pthread_mutex_lock(&historyBufferMutex);
        {
            CircBuff_addData(&buffer, current_lightRead_voltage);
        }
        pthread_mutex_unlock(&historyBufferMutex);
        totalSamples++;
        update_Average_Reading(current_lightRead_voltage);              //keep track of average

        msleep(1);
    }
    printf("Sampling of photoresistor has now stopped.\n");
    return 0;
}

// for testing, replace with provided timing code later
// taken from https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
static int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}
