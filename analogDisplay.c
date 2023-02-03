#include "include/sampler.h"
#include "include/terminal.h"
#include "include/potentiometer.h"
#include "include/analogDisplay.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/ioctl.h> 
#include <linux/i2c.h> 
#include <linux/i2c-dev.h>

#define GPIO_EXPORT_FILE "/sys/class/gpio/export"

#define I2CDRV_LINUX_BUS0 "/dev /i2c-0" 
#define I2CDRV_LINUX_BUS1 "/dev /i2c-1" 
#define I2CDRV_LINUX_BUS2 "/dev /i2c-2"

#define BUS1_ADDRESS 0x20

#define first_dir "/sys/class/gpio/gpio61/direction"    //set out
#define second_dir "/sys/class/gpio/gpio44/direction"   //set out

#define first_val "/sys/class/gpio/gpio61/value"    //set #
#define second_val "/sys/class/gpio/gpio44/value"   //set #
    // we both have green capes
#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

static void* dipHistoryToDisplay(void *vargp);

static pthread_t anDisplayThreadID;
static int i2cFileDesc;
// static bool isDisplayActive;

//complete the steps from the I2C guide (2.3) to config the board to read values 
static void basicSetup()
{
    system("config-pin P9_18 i2c");         //config pins
	system("config-pin P9_17 i2c");

    editReading(GPIO_EXPORT_FILE,"61");     //set to export thru gpio
    editReading(GPIO_EXPORT_FILE,"44");

    editReading(first_dir,"out");           //set direction to output
    editReading(second_dir,"out");

    editReading(first_val,"1");             //set value to on
    editReading(second_val,"1");
}


void Analog_startDisplaying(void)
{
    basicSetup();
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, BUS1_ADDRESS);

	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
	

    // isDisplayActive = true;

    pthread_create(&anDisplayThreadID, NULL, &dipHistoryToDisplay, NULL);
}

void Analog_stopDisplaying(void)
{
    close(i2cFileDesc);     //cleanup i2c access
    // isDisplayActive = false;
    pthread_join(anDisplayThreadID, NULL);
}




static void editReading(char* fileName, char* val)
{
    FILE* pfile = fopen(fileName, "w");
	if (pfile == NULL) {
		printf("ERROR: Unable to open export file.\n");
		exit(1);
	}
	fprintf(pfile, "%s", val);          //set the file's contents to the value (on/off for seg display)
	fclose(pfile);
}

//provided code by I2C guide
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}


static int firstDigit(int val){
    switch (val){
		case 0:
			return 0xA1;
		case 1:
			return 0x80;
		case 2:
			return 0x31;
		case 3:
			return 0xB0;
		case 4:
			return 0x90;
		case 5:
			return 0xB0;
		case 6:
			return 0xB1;
		case 7:
			return 0x80;
		case 8:
			return 0xB1;
		case 9:
			return 0x90;
		default:
			return 0xA1;	
	}
}

static int staticDigit(int val){
    switch (val){
		case 0:
			return 0x86;
		case 1:
			return 0x02;
		case 2:
			return 0x0E;
		case 3:
			return 0x0E;
		case 4:
			return 0x8A;
		case 5:
			return 0x8C;
		case 6:
			return 0x8C;
		case 7:	
			return 0x06;
		case 8:
			return 0x8E;
		case 9:
			return 0x8E;
		default:
			return 0x86;
	}
}

static void* dipHistoryToDisplay(void *vargp)
{
    while(1){

        //display 01-99


        sleep(.01)      //update 10 times per second
    }
    return 0;
}