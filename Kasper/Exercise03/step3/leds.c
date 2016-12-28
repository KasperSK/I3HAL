#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	
	char default_led[] = "/sys/class/leds/led3/brightness";
	char default_action[] = "1";
	unsigned int file = 0;
	unsigned int ret = 0;
	char *led, *action;
	printf("Argument 1 %s\n", argv[0]);
	printf("Argument 2 %s\n", argv[1]);
	printf("Argument 3 %s\n", argv[2]);
	led = default_led;
	action = default_action;
	
	if( argc >= 2){
		if(strcmp(argv[1],"led1")== 0){
			led = "/sys/class/leds/led1/brightness";
			printf("Working led1\n");
		}
		else if(strcmp(argv[1], "led2")==0){
			led = "/sys/class/leds/led2/brightness";
			printf("Working led2\n");
		}
		else if(strcmp(argv[1],"led3")== 0){
			led = "/sys/class/leds/led3/brightness";
			printf("Working led3\n");
		}
		else if(strcmp(argv[1],"ledB")== 0){
			led = "/sys/class/leds/ledB/brightness";
			printf("Working ledB\n");
		}
		else{
			led = default_led;
			printf("Working led3\n");
		}
		
		if( argc >= 3)
			if(strcmp(argv[2], "on")== 0){
				action = default_action;
			}else{
				action = "0";
			}
	}

	file = open(led, O_WRONLY);
	if (file == -1){
		printf("Error %d\n", errno);
	}
	printf("File %d opened\n", file);
	ret = write(file, action, 1);
	if (ret == -1){
		printf("Error %d\n", errno);
	}
	printf("bytes written %d\n", ret);
	close(file);
	

	return 0;
}
