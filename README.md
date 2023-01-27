# 433as2

TODO:

4 Modules Minimum

1. sampler.h / sampler.c 

- thread to keep continuous record of light level 
- keeps an average light level

2. AnalogToDigital.h / AnalogToDigital.c

- analog to digital reading from BBG (pin 40 P9)
-- sleep between for 1ms
- buffer to track most recent N samples (dynamically) [may need N+1]
- support changing history size

2a) potentiometer management

3. Networking.h / Networking.c 


4. output.h / output.c 
- terminal output (printf of data in 1.3) every second
- I2C 14 segment display

5. UDPlistener.h / UDPlistener.c 



