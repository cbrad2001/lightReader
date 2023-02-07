OUTFILE = as2
OUTFILE2 = noworky
OUTDIR = $(HOME)/Documents/CMPT_433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow 

app: 
		$(CC_C) $(CFLAGS) potentiometer.c photoresistor.c sampler.c periodTimer.c udpComms.c circularBuffer.c analogDisplay.c terminal.c main.c milliSleep.c -o $(OUTDIR)/$(OUTFILE) -pthread

all: 
		$(CC_C) $(CFLAGS) potentiometer.c photoresistor.c sampler.c periodTimer.c udpComms.c circularBuffer.c analogDisplay.c terminal.c main.c milliSleep.c -o $(OUTDIR)/$(OUTFILE) -pthread

noworky:
		$(CC_C) $(CFLAGS) noworky_debug/noworky.c -o $(OUTDIR)/$(OUTFILE2)
		
clean: 
		rm $(OUTDIR)/$(OUTFILE) 
		rm $(OUTDIR)/$(OUTFILE2)
