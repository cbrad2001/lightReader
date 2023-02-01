#include "include/photoresistor.h"
#include "include/sampler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

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

