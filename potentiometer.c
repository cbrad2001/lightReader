#include "include/potentiometer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int Pot_getRawValue(void)
{
    FILE *potVoltageFile = fopen(POT_VOLTAGE_PATH, "r");
    bool fileStatus = true;

    if (!potVoltageFile)
    {
        fprintf(stderr, "Error reading the voltage for the POT.\n");
        fileStatus = false;
    }

    const int MAX_LENGTH = 1024;
    char buf[MAX_LENGTH];
    fgets(buf, MAX_LENGTH, potVoltageFile);
    fclose(potVoltageFile);
    int voltageVal = atoi(buf); // Potential TODO: error check atoi

    if (fileStatus == false)
    {
        voltageVal = -1;
    }

    return voltageVal;
}

double Pot_getVoltage(void)
{
    int rawValue = Pot_getRawValue();

    if (rawValue == -1)
    {
        return -1;
    }

    const int MAX_VALUE = 4095;
    const double REF_VOLT = 1.8;
    double voltage = ((double)rawValue / MAX_VALUE) * REF_VOLT;

    return voltage;
}
