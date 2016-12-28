/*
 * temp_sensor.h
 *
 *  Created on: 26/02/2014
 *      Author: phm
 */

#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

// Forward declaration of struct
typedef struct temp_sensor temp_sensor;

struct temp_sensor {
	void *device;
	float (*getTempC)(temp_sensor*);
	void (*setTempLimitC)(temp_sensor*, float);
	int (*getTempStatus)(temp_sensor*);
	void (*disable)(temp_sensor*);
	void (*enable)(temp_sensor*);
	void (*release)(temp_sensor*);
};

typedef enum {VIRT, SAMPLE, LM75} temp_sensor_strategy;

void temp_sensor_init(temp_sensor *cthis, temp_sensor_strategy strategy, void *device);
void temp_sensor_release(temp_sensor *cthis);

temp_sensor * temp_sensor_new(temp_sensor_strategy strategy, void *device);
void temp_sensor_delete(temp_sensor *cthis);

#endif /* TEMP_SENSOR_H_ */
