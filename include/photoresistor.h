/*
photoresistor.h
Module for functionality of reading a photoresistor wired to P9 pin 40 of the BBG Zen Cape.
It provides access to raw digital and voltage readings from the wired photoresistor.
This module assumes that the hardware component is already wired to the Beaglebone.
*/

#ifndef _LIGHTREAD_H_
#define _LIGHTREAD_H_

#define LIGHTREAD_VOLTAGE_PATH "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"


// will add more comments here
// void* lightSamplingThread(void *vargp);

// Returns a the raw voltage value from the connected photoresistor from 0-4095 inclusive. 
// If it is unable to read the value, it returns -1.
int LightRead_getRawValue(void);

// Converts the raw value reading from the connected photoresistor to an actual voltage value.
// Returns -1 if there is an error reading the raw value of the photoresistor.
double LightRead_getVoltage(void);


void PhotoRes_startSampling(void);

void PhotoRes_stopSampling(void);

#endif