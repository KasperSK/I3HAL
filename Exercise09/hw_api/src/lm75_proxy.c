/*
 * lm75_proxy.c
 *
 *  Created on: 26/02/2014
 *      Author: phm
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "temp_sensor.h"

/* Excerpt from i2c-dev.h */
#define I2C_SLAVE       0x0703  /* Set slave addr IOCTL cmd ID */

/* Local Definitions */
#define LM75_I2C_ADDR	0x48    /* Devkit8000 Add-On Board LM75 Address */
#define LM75_I2C_BUS	3

/*
 * Private Data Structures and Methods
 */
typedef struct lm75_temp_sensor lm75_temp_sensor;
struct lm75_temp_sensor {
	int fd;
};

inline lm75_temp_sensor* lm75_temp_sensor_(temp_sensor *cthis){
	return (lm75_temp_sensor*)(cthis->device);
}


/*
 * Public Methods
 */
float lm75_getTempC(temp_sensor *cthis){
	char data[2];

	ioctl(lm75_temp_sensor_(cthis)->fd, 0x0703, 0x48);
	read(lm75_temp_sensor_(cthis)->fd, data, 2);

	if(data[1] & 128){
		return (float) data[0] + 0.5;
	}else{
		return (float) data[0];
	}
}

/* Destructor */
void lm75_proxy_release(temp_sensor *cthis){
	close(lm75_temp_sensor_(cthis)->fd);
	free((cthis->device));
}

/* Constructor */
int lm75_proxy_init(temp_sensor *cthis, void *device){
	int fp=0;
	int fd;

	/* Assign Public Methods */
	cthis->getTempC = lm75_getTempC;
	cthis->release = lm75_proxy_release;

	/* Open Linux i2c-dev device */
	fd = open("/dev/i2c-3", O_RDWR);
	if(fd < 0){
		printf("Could not open i2c-bus\n");
		exit(1);
	}

	/* Set I2C Device address */
	if (ioctl(fd, I2C_SLAVE, LM75_I2C_ADDR) < 0) {
		printf("LM75 device not found at addr:%x\n", LM75_I2C_ADDR);
		exit(1);
	}

	/* Create and init LM75 data struct */
	lm75_temp_sensor* its_temp_sensor = (lm75_temp_sensor *)malloc(sizeof(lm75_temp_sensor));
	cthis->device = its_temp_sensor;

	lm75_temp_sensor_(cthis)->fd=fd;

	return 0; // Success
}
