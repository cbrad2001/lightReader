/*
potentiometer.h
Module for functionality of controlling the potentiometer (POT) on the zen cape 
It provides access to a getting a raw value reading from the POT.
*/

#ifndef _POT_H_
#define _POT_H_

#define POT_VOLTAGE_PATH "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

// Returns a the raw voltage value from the POT from 0-4095 inclusive. 
// If it is unable to read the value, it returns -1.
// Note that the POT value itself is only updated once per second in this program.
int Pot_getRawValue(void);

// Converts the raw value reading from the POT to an actual voltage value.
// Returns -1 if there is an error reading the raw value of the POT.
// Note that the POT value itself is only updated once per second in this program.
double Pot_getVoltage(void);

// Starts/ends the thread that reads the POT once per second in the background.
void Pot_startSampling(void);
void Pot_stopSampling(void);

#endif