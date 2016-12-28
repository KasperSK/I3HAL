#include <linux/init.h>
#include <linux/module.h>
// Libraries
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <asm/io.h>
 
// Pointere til IO6 Registre
volatile unsigned long *REG_GPIO6_OE; 
volatile unsigned long *REG_GPIO6_DOUT;

int value1 = 0;
int value2 = 0;

MODULE_LICENSE("Dual BSD/GPL");

static int __init hello_init(void)
{
	// Bede kernen om lov til at benytte memory området
	if (request_mem_region (0x49058034, 12, "hello_world") == NULL) {
		printk ("Allocation for I/O memory range is failed\n");
		return -1;
	}
	
	// Tildele addresser til pointere (mappet til en anden end den fysiske)
	REG_GPIO6_OE = ioremap( 0x49058034, 4); 		// Sætter det til at være indput eller output - BANK 6
	REG_GPIO6_DOUT = ioremap(0x4905803c, 4); 		// Den værdi der bliver sat på udgangen - BANK 6
	
	// Læse 32-bit
	value1 = ioread32(REG_GPIO6_OE);
	value2 = ioread32(REG_GPIO6_DOUT);
	 
	// Modify bit
	value1 = value1 & ~(1<<4);
	value2 = value2 & ~(1<<4);

	// Skrive 32-bit
	iowrite32(value1, REG_GPIO6_OE);		
	iowrite32(value2, REG_GPIO6_DOUT);
	  
	printk(KERN_ALERT "Hello, world\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");

	// Læse 32-bit
	value1 = ioread32(REG_GPIO6_DOUT);

	// Modify bit
	value1 = value1 | (1<<4);

	// Skrive 32-bit
	iowrite32(value1, REG_GPIO6_DOUT);
  
	// Frigiv memory
	release_mem_region (0x49058034, 12);
}

module_init(hello_init);
module_exit(hello_exit);




