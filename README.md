TODO: updated 1/30

- Enclose thread creation and clean up in separate functions
- First code up the module provided as sampler.h in the a2 specs
    - Uses the sensor to sample light (thread)
    (below 3 are not threads)
    - Gets a history size (array size from potentiometer)
    - Gets the array of light level values
    - Calculates average light level

- One module/header file for terminal output every second
    - needs access to getter functions in the sampler module
    - also needs 200th sample in the sample history

- One module for reading zen cape potentiometer per second
    - have it set a static global in the module, and have a function to wrap the global that returns the value
    - no need for an active thread, just read the value from a function

- One module for zen cape 14 segment display

- One module for UDP
    - calls functions in sampler module and POT module
    - only way to exit is through a UDP call, which should cleanly stop other modules
    - suggestion: put in a shutdown module that handles shutting down the threads of the other modules (wait and trigger shutdown)
    - wait for shutdown is a blocking call that waits for the modules to shutdown before progressing

- One module for shutting down
    - separates the logic for shutting down away from UDP, more modular... maybe we might want to extend shutdown to a push of a button on the hardware as well