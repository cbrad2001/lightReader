#include "include/sampler.h"
#include "include/terminal.h"
#include "include/potentiometer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>


static pthread_t termThreadID;
// static bool isPrinting;
static long long prevSampleSum; 

static void* print_to_terminal(void *vargp);


void Terminal_startPrinting(void)
{
    // isPrinting = true;
    prevSampleSum = 0;
    pthread_create(&termThreadID, NULL, &print_to_terminal, NULL);
}

void Terminal_stopPrinting(void)
{
    // isPrinting = false;
    pthread_join(termThreadID, NULL);
}



static void* print_to_terminal(void *vargp)
{
    //every second....
    while(1){
        long long totalSamples = Sampler_getNumSamplesTaken();
        long long sampleVal_lastS = totalSamples - prevSampleSum;
        prevSampleSum += sampleVal_lastS;
        int POT_val = Pot_getRawValue();
        double avg_light = Sampler_getAverageReading();
        int dips = 0;   //stub... implement
        //line 1: ( # light samples in last second) | (raw pot value) | (num valid samples in history) | (avg light 3dp) | (# dips)

        printf("# Samples (1s): %lli \t" 
               "| Raw POT value: %i \t" 
               "| # Valid Samples: %lli \t" 
               "| Average Light Read: %.3f \t" 
               "| # Dips: %i\n", sampleVal_lastS, POT_val, totalSamples, avg_light, dips);

        //line 2: (every 200th sample in history)
        Sampler_printEveryNth(200);


        sleep(1);       //write every second
    }
    return 0;
}