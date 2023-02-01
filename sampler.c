#include "include/sampler.h"
#include "include/potentiometer.h"
#include "include/photoresistor.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

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

enum status{
    SUCCESS,
    FAIL
} status;

static circular_buffer buffer;

static enum status buffer_pushback(circular_buffer *b, double pr_reading)
{
    int next = b->head + 1; //pointer after write
    if (next >= b->historySize)
        next = 0;
    
    if (next == b->tail)
        return FAIL;

    b->historyBuffer[b->head] = pr_reading;
    b->head = next;
    return SUCCESS;
}

static void buff_init(circular_buffer *b){
    b->head = 0;
    b->tail = 0; 
    b->historySize = 0; 
    b->historyBuffer = (double *)malloc(Pot_getRawValue() * sizeof(double));   //dynamic array for circular buffer
    printf("successfully initialized buffer\n");
}

void add_data(double pr_reading){
    if (buffer_pushback(&buffer,pr_reading)==FAIL){
        printf("Error loading to buffer\n");
        printf("Final array size: %i\n",Sampler_getNumSamplesInHistory());
    }
}


void Sampler_startSampling(void)
{
    pthread_mutex_init(&historyBufferMutex, NULL);
    pthread_mutex_init(&historySizeMutex, NULL);

    isSampling = true;
    totalSamples = 0;

    buff_init(&buffer);

    pthread_create(&potThreadID, NULL, &potThread, NULL);
    pthread_create(&lightThreadID, NULL, &lightSamplingThread, NULL);    
}

void Sampler_stopSampling(void)
{
    printf("Stopping the thread for sampling light level.\n");
    pthread_mutex_destroy(&historyBufferMutex);
    pthread_mutex_destroy(&historySizeMutex);
    isSampling = false;

    free(buffer.historyBuffer);            

    pthread_join(potThreadID, NULL);
    pthread_join(lightThreadID, NULL);
}

void Sampler_setHistorySize(int newSize)
{
    pthread_mutex_lock(&historySizeMutex);
    {
        buffer.historySize = newSize;
    }
    pthread_mutex_unlock(&historySizeMutex);

    double *newBuffer = Sampler_getHistory(Pot_getRawValue());
    pthread_mutex_lock(&historyBufferMutex);
    {
        // TODO: update the buffer here
        double *oldBuffer = buffer.historyBuffer;
        free(oldBuffer);
        buffer.historyBuffer = newBuffer;
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
        historyCopy[i] = buffer.historyBuffer[i];
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
        sum += buffer.historyBuffer[i];
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

static void* potThread(void *vargp)
{
    while (isSampling)
    {
        printf("Raw potentiometer value: %.3i\n", Pot_getRawValue());
        Sampler_setHistorySize(Pot_getVoltage());  // count potentiometer value



        buffer.tail = Sampler_getHistorySize()-1;
        totalSamples++;

    }
    printf("Sampling of POT has now stopped.\n");
    return 0;
}

static void* lightSamplingThread(void *vargp)
{
    while(isSampling){

        double current_lightRead_voltage = LightRead_getVoltage();
        add_data(current_lightRead_voltage);
            
        //put the reading into the buffer
        printf("Light value: %.4f\n", current_lightRead_voltage);         // temp for visual output
        sleep(0.001);                                                   // between light samples, sleep for 1ms 
    }
    printf("Sampling of photoresistor has now stopped.\n");
    return 0;

}
