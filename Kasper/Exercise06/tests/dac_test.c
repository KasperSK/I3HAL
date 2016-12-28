#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	
	char led[] = "/dev/dacA";
	char action[10] = "1";
	unsigned int file = 0;
	unsigned int ret = 0;
	size_t i;

	file = open(led, O_WRONLY);
	if (file == -1){
		printf("Open file Error %d\n", errno);
		return -1;
	}
	printf("File %d opened\n", file);
	
	for(i = 0; i < 4000; i = i + 10){
		sprintf(action, "%d", i);
		ret = write(file, &action, sizeof(action));
		if (ret == -1){
			break;
		}
		usleep(100);
	}
	close(file);

	return 0;
}
