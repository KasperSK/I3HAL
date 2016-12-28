#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>

#define BOOTKEY 7

MODULE_LICENSE("Dual BSD/GPL");

static int __init bootkey_init(void)
{
	int status;
	int err = gpio_request_one(BOOTKEY, GPIOF_DIR_IN, "Boot Key");
	if (err) {
		printk(KERN_ALERT "Could not request bootkey\n");
		return -1;
	}
	
	status = gpio_get_value(BOOTKEY);
	printk(KERN_ALERT "Value: %d", status);
	return 0;
}

static int __exit bootkey_exit(void)
{
	gpio_free(BOOTKEY);
	return 0;
}

module_init(bootkey_init);
module_exit(bootkey_exit);
