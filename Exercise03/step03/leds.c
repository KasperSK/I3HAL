#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	
	char led[] = "/sys/class/leds/led3/brightness";
	char action = '1';
	unsigned int file = 0;
	unsigned int ret = 0;
	#ifdef DEBUG
	printf("Argument 1 %s\n", argv[0]);
	printf("Argument 2 %s\n", argv[1]);
	printf("Argument 3 %s\n", argv[2]);
	#endif
	
	if( argc >= 2){
		if(strcmp(argv[1],"led1")== 0){
			led[19] = '1';
		}
		else if(strcmp(argv[1], "led2")==0){
			led[19] = '2';
		}
		else if(strcmp(argv[1],"led3")== 0){
			led[19] = '3';
		}
		else if(strcmp(argv[1],"ledB")== 0){
			led[19] = 'B';
		}
		
		if( argc >= 3){
			if(strcmp(argv[2], "off")== 0){
				action = '0';
			}
		}
	}
	printf("Working led%c\n", led[19]);

	file = open(led, O_WRONLY);
	if (file == -1){
		printf("Open file Error %d\n", errno);
		return -1;
	}
	printf("File %d opened\n", file);

	ret = write(file, &action, 1);
	if (ret == -1){
		printf("Write file Error %d\n", errno);
	} else {
		printf("bytes written %d\n", ret);
	}
	close(file);

	return 0;
}
