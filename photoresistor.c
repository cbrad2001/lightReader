#include "include/photoresistor.h"
#include "include/sampler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

static pthread_t lightThreadID;
static bool isSampling;

static void* lightSamplingThread(void *vargp);

int LightRead_getRawValue(void)
{
    FILE *lightReadVoltageFile = fopen(LIGHTREAD_VOLTAGE_PATH, "r");
    bool fileStatus = true;

    if (!lightReadVoltageFile)
    {
        fprintf(stderr, "Error reading the voltage for the POT.\n");
        fileStatus = false;
    }

    const int MAX_LENGTH = 1024;
    char buf[MAX_LENGTH];
    fgets(buf, MAX_LENGTH, lightReadVoltageFile);
    int voltageVal = atoi(buf); // Potential TODO: error check atoi

    if (fileStatus == false)
    {
        voltageVal = -1;
    }

    return voltageVal;
}

double LightRead_getVoltage(void)
{
    int rawValue = LightRead_getRawValue();
    
    if (rawValue == -1)
    {
        return -1;
    }

    const int MAX_VALUE = 4095;
    const double REF_VOLT = 1.8;
    double voltage = ((double)rawValue / MAX_VALUE) * REF_VOLT;

    return voltage;
}

void PhotoRes_startSampling(void){
    isSampling = true;
    if (pthread_create(&lightThreadID, NULL, &lightSamplingThread, NULL)!=0){
        perror("pthread_create() error");
        exit(1);
    }
    
}

void PhotoRes_stopSampling(void){
    pthread_join(lightThreadID, NULL);
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
    printf("Sampling has now stopped.\n");
    return 0;

}


