//Module for UDP
//    calls functions in sampler module and POT module
//    only way to exit is through a UDP call, which should cleanly stop other modules
//    put in a shutdown module that handles shutting down the threads of the other modules (wait and trigger shutdown)
//    wait for shutdown is a blocking call that waits for the modules to shutdown before progressing
#ifndef _UDP_H_
#define _UDP_H_

// Begin/end the background thread which prints to screen using printf()
void udp_startSampling(void);
void udp_stopSampling(void);

#endif

