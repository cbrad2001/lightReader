//Module for UDP
//    calls functions in sampler module and POT module
//    only way to exit is through a UDP call, which should cleanly stop other modules
//    put in a shutdown module that handles shutting down the threads of the other modules (wait and trigger shutdown)
//    wait for shutdown is a blocking call that waits for the modules to shutdown before progressing
#include "include/sampler.h"
#include "include/potentiometer.h"
#define LIGHT_DIP_DIFFERENCE_V = 0.1 // volts

#define HELP "help"
#define COUNT "count"
#define VALUE "get value"
#define LENGTH "get length"
#define ARRAY "get array"
#define STOP "stop"

//waits and handles shutting down the threads of the other modules (wait and trigger shutdown)
void udp_awaitShutdown(void);

// thread listens for and acts on key commands 
void udp_startSampling(void);

void udp_stopSampling(void);


