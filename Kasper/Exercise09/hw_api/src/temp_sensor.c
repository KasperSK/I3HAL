/*
 * temp_sensor.c
 *
 *  Created on: 23/08/2013
 *      Author: phm
 */
#include <stdio.h>
#include <stdlib.h>
#include "temp_sensor.h"
#include "lm75_proxy.h"
#include "virt_temp_proxy.h"
#include "sample_temp_proxy.h"

/*
 * Default Method Implementations
 */
void temp_sensor_disable(temp_sensor *cthis){
	printf("Temperature sensor disabled\n");
}

void temp_sensor_enable(temp_sensor *cthis){
	printf("Temperature sensor Enabled\n");
}

float temp_sensor_getTempC(temp_sensor *cthis){
	printf("getTempC not Implemented\n");
	exit(1);
	return 0.0;
}

void temp_sensor_setTempLimitC(temp_sensor *cthis, float temp){
	printf("setTempLimitC not Implemented\n");
	exit(1);
}

int temp_sensor_getTempStatus(temp_sensor *cthis){
	printf("setTempLimitC not Implemented\n");
	exit(1);
	return 0;
}

/*
 * CLASS CONSTRUCTOR AND DESCTRUCTOR
 */

temp_sensor * temp_sensor_new(temp_sensor_strategy strategy, void *device){

	temp_sensor* its_temp_sensor = (temp_sensor *)malloc(sizeof(temp_sensor));

	if(its_temp_sensor != NULL) {
		temp_sensor_init(its_temp_sensor, strategy, device);
	}

	return its_temp_sensor;
}

void temp_sensor_delete(temp_sensor *cthis) {
	temp_sensor_release(cthis);
	free((void*)cthis);
}

void temp_sensor_init(temp_sensor *cthis, temp_sensor_strategy strategy, void *device){

	/* Set Default Implementations */
	cthis->disable = temp_sensor_disable;
	cthis->enable = temp_sensor_enable;
	cthis->getTempC = temp_sensor_getTempC;
	cthis->getTempStatus = temp_sensor_getTempStatus;
	cthis->setTempLimitC = temp_sensor_setTempLimitC;

	/* Init according to strategy */
	switch(strategy) {
	case VIRT:
		virt_temp_proxy_init(cthis, device);
		break;
	case SAMPLE:
		sample_temp_proxy_init(cthis, device);
		break;
	default:
		break;
	}
}

void temp_sensor_release(temp_sensor *cthis){
	cthis->release(cthis);
}
