#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define ON "1"
#define OFF "0"

void setLed(char* status) {
	int fd;
	fd = open("/sys/class/leds/led3/brightness", O_WRONLY);
	if (fd != -1) {
		write(fd, status, 1);
		close(fd);
	} else {
		printf("Error in opening file\n");
	}
}

void printHelp(char* file) {
	printf("%s ON   //Turn led on\n", file);
	printf("%s OFF  //Turn led off\n", file);
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "ON") == 0)
		{
			printf("ON\n");
			setLed(ON);
			return 0;
		} else if (strcmp(argv[1], "OFF") == 0) {
			printf("OFF\n");
			setLed(OFF);
			return 0;
		}
	}
	printHelp(argv[0]);
	return -1;
}
