#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/potentiometer.h"
#include "include/photoresistor.h"
#include "include/sampler.h"
#include "include/terminal.h"
#include "include/analogDisplay.h"
#include "include/udpComms.h"


/**
 * Main running code for light dip reader
*/
int main()
{
    Sampler_startSampling();
	Terminal_startPrinting();
	// Analog_startDisplaying();
	udp_startSampling();
	
	udp_stopSampling();
	// Analog_stopDisplaying();
	Terminal_stopPrinting();
	Sampler_stopSampling();
	
    return 0;

}