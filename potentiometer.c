#include "potentiometer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

static int potRawValue;
static pthread_t potID;
static bool sampling;

static void Pot_updateRawValue(void)
{
    FILE *potVoltage = fopen(POT_VOLTAGE_PATH, "r");
    bool fileStatus = true;

    if (potVoltage == NULL)
    {
        fprintf(stderr, "Error reading the voltage for the POT.\n");
        fileStatus = false;
    }

    const int MAX_LENGTH = 1024;
    char buf[MAX_LENGTH];
    fgets(buf, MAX_LENGTH, potVoltage);
    int voltageVal = atoi(buf);

    if (fileStatus == false)
    {
        voltageVal = -1;
    }

    static pthread_mutex_t rawValMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&rawValMutex);
    {
        potRawValue = voltageVal;
    }
    pthread_mutex_unlock(&rawValMutex);
}

int Pot_getRawValue(void)
{
    return potRawValue;
}

double Pot_getVoltage(void)
{
    if (Pot_getRawValue() == -1)
    {
        return -1;
    }

    double rawValue = (double)Pot_getRawValue();
    const int MAX_VALUE = 4095;
    const double REF_VOLT = 1.8;
    double voltage = (rawValue / MAX_VALUE) * REF_VOLT;

    return voltage;
}

static void *potThread(void *vargp)
{
    while (sampling)
    {
        Pot_updateRawValue();
        usleep(1000);
    }
    printf("POT is now not being sampled.");
}

void Pot_startSampling(void)
{
    sampling = true;
    pthread_create(&potID, NULL, potThread, NULL);
}

void Pot_stopSampling(void)
{
    sampling = false;
    printf("Stopping the thread for reading the POT.")
    pthread_join(potID, NULL);
}
