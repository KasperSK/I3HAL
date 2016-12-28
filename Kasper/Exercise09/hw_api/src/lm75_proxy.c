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
#define LM75_I2C_FILE   "/dev/i2c-3"

/*
 * Private Data Structures and Methods
 */
 
typedef struct lm75_temp_sensor lm75_temp_sensor;
struct lm75_temp_sensor {
	int fd;
	int tempStatus;
	float tempLimit;
};

/* Access method for virt_temp.. struct */
inline lm75_temp_sensor* lm75_temp_sensor_(temp_sensor *cthis) {
	return  (lm75_temp_sensor*)(cthis->device); // Access device data
}

/*
 * Public Methods
 */
float lm75_getTempC(temp_sensor *cthis){
	return (float)(54.3);
}

/* Destructor */
void lm75_proxy_release(temp_sensor *cthis){
	free(lm75_temp_sensor_(cthis));
}

/* Constructor */
int lm75_proxy_init(temp_sensor *cthis, void *device){
	int fp=0;

	/* Assign Public Methods */
//	cthis->disable = temp_sensor_disable;
//	cthis->enable = temp_sensor_enable;
	cthis->getTempC = lm75_getTempC;
//	cthis->getTempStatus = temp_sensor_getTempStatus;
//	cthis->setTempLimitC = temp_sensor_setTempLimitC;

	/* Open Linux i2c-dev device */
	fp = open(LM75_I2C_FILE, O_RDWR);
	if(fp < 0){
		printf("Cloud not open I2C bus file\n");
		exit(1);
	}
	/* Set I2C Device address */
	if (ioctl(fp, I2C_SLAVE, LM75_I2C_ADDR) < 0) {
		printf("LM75 device not found at addr:%x\n", LM75_I2C_ADDR);
		exit(1);
	}

	/* Create and init LM75 data struct */
	lm75_temp_sensor* its_temp_sensor = (lm75_temp_sensor *)malloc(sizeof(lm75_temp_sensor));
	cthis->device = its_temp_sensor;
	lm75_temp_sensor_(cthis)->fd = fp;
	
	return 0; // Success
}
