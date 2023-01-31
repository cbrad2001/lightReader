OUTFILE = as2
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow 

app: 
		$(CC_C) $(CFLAGS) potentiometer.c noworky.c -o $(OUTDIR)/$(OUTFILE) -pthread

all: 
		$(CC_C) $(CFLAGS) potentiometer.c noworky.c -o $(OUTDIR)/$(OUTFILE) -pthread
		
clean: 
		rm $(OUTDIR)/$(OUTFILE)