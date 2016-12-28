#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <stdlib.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define LEDS_OE 0x49058034
#define LEDS_IN 0x49058038
#define LEDS_OUT 0x4905803C

#define KEY_OE 0x48310034
#define KEY_IN 0x48310038

#define LEDS 0

struct sys_leds {
	void *map_base_leds_oe, *map_base_leds_in, *map_base_leds_out;
	unsigned int *virt_addr_leds_oe, *virt_addr_leds_in, *virt_addr_leds_out;
	off_t led_oe_reg, led_in_reg, led_out_reg;
};

struct boot_key {
	void *map_base_key_oe, *map_base_key_in;
	unsigned int *virt_addr_key_oe, *virt_addr_key_in;
	off_t key_oe_reg, key_in_reg;
};

int ledMmap(struct sys_leds* leds, int fd) {
	leds->map_base_leds_oe = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, leds->led_oe_reg & ~MAP_MASK);
	leds->map_base_leds_in = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, leds->led_in_reg & ~MAP_MASK);
	leds->map_base_leds_out = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, leds->led_out_reg & ~MAP_MASK);
	if(leds->map_base_leds_oe == MAP_FAILED || leds->map_base_leds_in == MAP_FAILED || leds->map_base_leds_out == MAP_FAILED)
		return -1;
	leds->virt_addr_leds_oe = (unsigned int *)(leds->map_base_leds_oe + (leds->led_oe_reg & MAP_MASK));
	leds->virt_addr_leds_in = (unsigned int *)(leds->map_base_leds_in + (leds->led_in_reg & MAP_MASK));
	leds->virt_addr_leds_out = (unsigned int *)(leds->map_base_leds_out + (leds->led_out_reg & MAP_MASK));
	return 0;
}

int keyMmap(struct boot_key* key, int fd) {
	key->map_base_key_oe = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, key->key_oe_reg & ~MAP_MASK);
	key->map_base_key_in = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, key->key_in_reg & ~MAP_MASK);
	
	if(key->map_base_key_oe == MAP_FAILED || key->map_base_key_in == MAP_FAILED)
		return -1;
	key->virt_addr_key_oe = (unsigned int *)(key->map_base_key_oe + (key->key_oe_reg & MAP_MASK));
	key->virt_addr_key_in = (unsigned int *)(key->map_base_key_in + (key->key_in_reg & MAP_MASK));

	return 0;
}

int main(int argc, char* argv[]) {

	// File descriptor 
	struct sys_leds sysLeds = {.led_oe_reg = LEDS_OE, .led_in_reg = LEDS_IN, .led_out_reg = LEDS_OUT};
	struct sys_leds* sysLedPtr = &sysLeds;
	struct boot_key keys = {.key_oe_reg = KEY_OE, .key_in_reg = KEY_IN};
	struct boot_key* keyPtr = &keys;
	int fd, retur;
	
	printf("Size of unsigned int: %d, size of unsigned long: %d, size of unsigned short: %d\n", sizeof(unsigned int), sizeof(unsigned long), sizeof(unsigned short));
	
	printf("Opening file /dev/mem\n");
	fd = open("/dev/mem", O_RDWR, 0);
	if(fd < 0) {
		printf("could not open file\n");
		exit(EXIT_FAILURE);
	}
	printf("Mapping memory using mmap\n");
	retur = ledMmap(sysLedPtr, fd);
	if(retur != 0) {
		printf("could not map memory\n");
		close(fd);
		exit(EXIT_FAILURE);
	}
	retur = keyMmap(keyPtr, fd);
	if(retur != 0) {
		printf("could not map memory\n");
		munmap(sysLedPtr->map_base_leds_oe, MAP_SIZE);
		munmap(sysLedPtr->map_base_leds_in, MAP_SIZE);
		munmap(sysLedPtr->map_base_leds_out, MAP_SIZE);
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Setting leds to output 
	*sysLedPtr->virt_addr_leds_oe = (*sysLedPtr->virt_addr_leds_oe & ~(1<<4));
	*sysLedPtr->virt_addr_leds_oe = (*sysLedPtr->virt_addr_leds_oe & ~(1<<3));	
	*sysLedPtr->virt_addr_leds_out = (*sysLedPtr->virt_addr_leds_out | (1<<4));
	*sysLedPtr->virt_addr_leds_out = (*sysLedPtr->virt_addr_leds_out | (1<<3));
	*keyPtr->virt_addr_key_oe = *keyPtr->virt_addr_key_oe | (1<<7);
	
	while(1) {
		printf("Waiting for somebody to push a key!\n");
		while(!(*keyPtr->virt_addr_key_in & (1<<7))) {
			usleep(1000);
		}
		if(((*sysLedPtr->virt_addr_leds_out) & (1<<4)) != 0) {
			*sysLedPtr->virt_addr_leds_out = (*(sysLedPtr->virt_addr_leds_out) & ~(1<<4));
			printf("Turn this baby ON!\n");
		} 
		else {
			*sysLedPtr->virt_addr_leds_out = (*(sysLedPtr->virt_addr_leds_out) | (1<<4));
			printf("Turn this baby OFF!\n");
		}
		usleep(1000000);
	}
	
	munmap(sysLedPtr->map_base_leds_oe, MAP_SIZE);
	munmap(sysLedPtr->map_base_leds_in, MAP_SIZE);
	munmap(sysLedPtr->map_base_leds_out, MAP_SIZE);
	munmap(keyPtr->map_base_key_oe, MAP_SIZE);
	munmap(keyPtr->map_base_key_in, MAP_SIZE);
	
}