#include "include/sampler.h"
#include "include/terminal.h"
#include "include/potentiometer.h"
#include "include/periodTimer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>


static pthread_t termThreadID;
static bool isPrinting;
static long long prevSampleSum; 

static void* print_to_terminal(void *vargp);

void Terminal_startPrinting(void)
{
    isPrinting = true;
    prevSampleSum = 0;
    pthread_create(&termThreadID, NULL, &print_to_terminal, NULL);
}

void Terminal_stopPrinting(void)
{
    isPrinting = false;
    pthread_join(termThreadID, NULL);
}

void Terminal_quit()
{
    isPrinting = false;
}

//thread repeatedly outputs specified data to screen at a 1 second rate
static void* print_to_terminal(void *vargp)
{
    while(isPrinting){
        Period_statistics_t stats; 
        long long totalSamples = Sampler_getNumSamplesTaken();
        long long sampleVal_lastS = totalSamples - prevSampleSum;
        prevSampleSum += sampleVal_lastS;

        int POT_val = Pot_getRawValue();
        double avg_light = Sampler_getAverageReading();
        int dips = Sampler_analyzeDips();

        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &stats);

        //line 1: ( # light samples in last second) | (raw pot value) | (num valid samples in history) | (avg light 3dp) | (# dips) | (# samples in buffer)
        printf("# Samples/s): %lli \t" 
               "| Pot Value: %i \t" 
               "| History Size: %lli \t" 
               "| Light AVG: %.3f \t" 
               "| Dips: %i\t"
               "| Sampling[%.3f, %.3f] avg %.3f/%lli\n", 
                    sampleVal_lastS, 
                    POT_val, 
                    totalSamples, 
                    avg_light, 
                    dips, 
                    stats.minPeriodInMs, stats.maxPeriodInMs, stats.avgPeriodInMs,sampleVal_lastS);
        //line 2: (every 200th sample in history)
        Sampler_printEveryNth(200);

        sleep(1);       //write every second
    }
    return 0;
}