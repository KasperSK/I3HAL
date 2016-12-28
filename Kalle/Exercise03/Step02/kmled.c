// Libraries
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <asm/io.h>

#define GPIO6_OE   0x49058034
#define GPIO6_DOUT 0x4905803c
#define SYS_LED4   4

MODULE_LICENSE("Dual BSD/GPL");

// Pointere til IO6 Registre
volatile unsigned char allocation = 0;

static int kmled_init(void) {

	volatile unsigned long *REG_GPIO6_OE; 
	volatile unsigned long *REG_GPIO6_DOUT;
	volatile unsigned long value;

	// Bede kernen om lov til at benytte memory området
	if (request_mem_region (0x49058034, 12, "hello") == NULL) {
	        printk ("Allocation for I/O memory range is failed\n");
	        return 0;
	}
	allocation = 1;

	// Tildele addresser til pointere (mappet til en anden end den fysiske)
	REG_GPIO6_OE =   ioremap(GPIO6_OE, 4);
	REG_GPIO6_DOUT = ioremap(GPIO6_DOUT, 4); 

	//Sæt GPIO_164 til OUT
	value = ioread32(REG_GPIO6_OE);
	value &= ~(1 << SYS_LED4);
	iowrite32(value, REG_GPIO6_OE);

	// Tænd LED4
	value = ioread32(REG_GPIO6_DOUT);
	value &= ~(1 << SYS_LED4);
	iowrite32(value, REG_GPIO6_DOUT);

	printk(KERN_ALERT "Entering\n");
	return 0;
}

static void kmled_exit(void) {
	volatile unsigned long *REG_GPIO6_DOUT;
	volatile unsigned long value;

	if (allocation) {
		REG_GPIO6_DOUT = ioremap(GPIO6_DOUT, 4); 

		// Slukke LED4
		value = ioread32(REG_GPIO6_DOUT);
		value |= (1 << SYS_LED4);
		iowrite32(value, REG_GPIO6_DOUT);

		//Release regionen
		release_mem_region (GPIO6_OE, 12);
	}
	printk(KERN_ALERT "Leaving\n");

}

module_init(kmled_init);

module_exit(kmled_exit);

