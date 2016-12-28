#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

volatile unsigned long *REG_GPIO6_OE;
volatile unsigned long *REG_GPIO6_DOUT;
volatile unsigned int value;

static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello world\n");
	if (request_mem_region (0x49058034, 12, "hello") == NULL){
		printk("Allocation for I/O memory range has failed\n");
		return 0;
	}
	REG_GPIO6_OE = ioremap( 0x49058034, 4);
	REG_GPIO6_DOUT = ioremap( 0x4905803c, 4);
	value = ioread32(REG_GPIO6_OE);
	printk(KERN_ALERT "read in %d\n", value);
	if (value != 0){
		value = 0;
		iowrite32(value,REG_GPIO6_OE);
		printk(KERN_ALERT "read in %d\n", value);
	}
	
	value = ioread32(REG_GPIO6_DOUT);
	printk(KERN_ALERT "read in dout %d\n", value);
	iowrite32(0xFFFFFF0F,REG_GPIO6_DOUT); // 0xF0FFFF00 Turns on all leds
	return 0;
}

static void __exit  hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");	
	iowrite32(0xFFFFFFFF,REG_GPIO6_DOUT);
	release_mem_region(0x49058034, 12);
}

module_init(hello_init);
module_exit(hello_exit);
