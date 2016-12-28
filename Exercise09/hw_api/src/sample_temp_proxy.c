/*
 * sample_temp_proxy.c
 *
 *  Created on: 26/02/2014
 *      Author: phm
 */
//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "temp_sensor.h"

/*
 * Private Structures and Methods
 */

/* sample_temp Data Structure */
typedef struct sample_temp sample_temp;
struct sample_temp {
	FILE *fp;
	int status;
};

/* Method to access sample_temp struct in temp_sensor struct */
inline sample_temp* sample_temp_(temp_sensor *cthis) {
	return  (sample_temp*)(cthis->device);
}

/*
 * Public Methods
 */

float sample_temp_getTempC(temp_sensor *cthis){
	float floatValue;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

	FILE *fp = sample_temp_(cthis)->fp;

	/*
	 * C-Based I/O is used to allow the use
	 * of "getline" that reads a line in a file
	 */
	if ((read = getline(&line, &len, fp)) <0) {
		printf("error reading from sample file\n");
		return 0.0;
	}

	/* Convert string to float value */
	sscanf(line, "%f", &floatValue);
    free(line);

	return floatValue;
}

/* Destructor */
void sample_temp_proxy_release(temp_sensor *cthis){
	fclose(sample_temp_(cthis)->fp);
	free((cthis->device));
}

/* Constructor */
int sample_temp_proxy_init(temp_sensor *cthis, void *device){
	/* C-based File I/O, not POSIX
	 * Due to use of getline() no
	 * POSIX equivalent */

	FILE *fp;

	/* Assign Public Methods */
	cthis->getTempC = sample_temp_getTempC;
	cthis->release = sample_temp_proxy_release;

	/* Open Linux i2c-dev device */
	fp = fopen((char*)device, "r"); // device = int!!!
	if((int)(fp) <= 0) {
		printf("Invalid file pointer!\n");
		exit(1);
	}

	/* Create and init sample_temp data struct */
	sample_temp* its_temp_sensor = (sample_temp *)malloc(sizeof(sample_temp));
	if(its_temp_sensor == 0)
		exit(1); // Unable to allocate memory
	cthis->device = (void*)its_temp_sensor;
	sample_temp_(cthis)->fp = fp;

	return 0; // Success
}
