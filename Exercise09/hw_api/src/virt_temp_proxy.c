/*
 * virt_temp_proxy.c
 *
 *  Created on: 26/02/2014
 *      Author: phm
 */
#include <stdio.h>
#include <stdlib.h>
#include "temp_sensor.h"

/*
 * Private Structures and Methods
 */

/* virt_temp_sensor private data structure */
typedef struct virt_temp_sensor virt_temp_sensor;
struct virt_temp_sensor {
	int tempStatus;
	float tempLimit;
};

/* Access method for virt_temp.. struct */
inline virt_temp_sensor* virt_temp_sensor_(temp_sensor *cthis) {
	return  (virt_temp_sensor*)(cthis->device); // Access device data
}

/*
 * Public Methods
 */

float virt_temp_getTempC(temp_sensor *cthis) {
	return (float)((rand()%160)/2.0) - 40.0; // Range [-40..+40] deg C
}

int virt_temp_getTempStatus(temp_sensor *cthis) {
	int stat = rand()%255;
	virt_temp_sensor_(cthis)->tempStatus = stat;
	return stat;    // Range [0..255]
}

/* Destructor */
void virt_temp_proxy_release(temp_sensor *cthis){
	free(virt_temp_sensor_(cthis));
}

/* Constructor */
int virt_temp_proxy_init(temp_sensor* cthis){
	cthis->getTempC = virt_temp_getTempC;
	cthis->getTempStatus = virt_temp_getTempStatus;
	cthis->release = virt_temp_proxy_release;

	virt_temp_sensor* its_temp_sensor = (virt_temp_sensor *)malloc(sizeof(virt_temp_sensor));
	cthis->device = its_temp_sensor;
	return 0;
}
