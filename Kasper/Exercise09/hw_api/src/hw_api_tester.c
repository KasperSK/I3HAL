/*
 ============================================================================
 Name        : text_formatter.c
 Author      : PHM
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "temp_sensor.h"

int main(int narg, char *argp[]) {
	puts("Testing HW Proxy Pattern");

	char pathName[] = "/home/stud/I3HAL/Kasper/Exercise09/hw_api/sample_data.txt";
	temp_sensor *sensor1 = temp_sensor_new(VIRT, pathName);

	printf("Temperature: %f\n", sensor1->getTempC(sensor1));
	printf("Temperature: %f\n", sensor1->getTempC(sensor1));
	printf("Temperature: %f\n", sensor1->getTempC(sensor1));
	printf("Temperature: %f\n", sensor1->getTempC(sensor1));
	printf("Temperature: %f\n", sensor1->getTempC(sensor1));

	printf("Status: %x\n", sensor1->getTempStatus(sensor1));
	printf("Status: %x\n", sensor1->getTempStatus(sensor1));
	printf("Status: %x\n", sensor1->getTempStatus(sensor1));
	printf("Status: %x\n", sensor1->getTempStatus(sensor1));

	sensor1->disable(sensor1);

	temp_sensor_delete(sensor1);

	puts("Testing Completed!");

	return EXIT_SUCCESS;
}
