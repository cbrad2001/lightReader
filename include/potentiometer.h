/*
potentiometer.h
Module for functionality of reading the integrated potentiometer (POT) on the zen cape 
It provides access to raw digital and voltage readings from the POT.
*/
#ifndef _POT_H_
#define _POT_H_

#define POT_VOLTAGE_PATH "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

// Returns a the raw voltage value from the POT from 0-4095 inclusive. 
// If it is unable to read the value, it returns -1.
int Pot_getRawValue(void);

// Converts the raw value reading from the POT to an actual voltage value.
// Returns -1 if there is an error reading the raw value of the POT.
double Pot_getVoltage(void);

#endif