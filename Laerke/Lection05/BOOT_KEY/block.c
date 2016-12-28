#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	
	char led[] = "/dev/boot_key_test";
	char action = '1';
	unsigned int file = 0;
	unsigned int ret = 0;
	
	file = open(led, O_NONBLOCK);
	if (file == -1){
		printf("Open file Error %d\n", errno);
		return -1;
	}
	printf("File %d opened\n", file);

	ret = read(file, &action, 1);
	if (ret == -1){
		printf("Write file Error %d\n", errno);
	} else {
		printf("gpio read %d\n", action -'0');
	}
	close(file);

	return 0;
}
