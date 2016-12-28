#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/cpld-gpio.h>

#define NR_KEYPAD_GPIO 8

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

static unsigned char keypad_keycode[16] = {
	KEY_1, KEY_2, KEY_3, KEY_A,
	KEY_4, KEY_5, KEY_6, KEY_B,
	KEY_7, KEY_8, KEY_9, KEY_C,
	KEY_KPASTERISK, KEY_0, KEY_ENTER, KEY_D
};

struct keypad_ldd{
	struct input_dev* keypad_input_dev;
	struct platform_device* keypad_dev;
	struct timer_list keypad_timer;
	unsigned int timer_running;
	unsigned int gpio_state;
	unsigned int timeout_in_sec;
	unsigned int keypad_gpio[NR_KEYPAD_GPIO];
};

static struct keypad_ldd numeric_keypad = {
	.timeout_in_sec = 1,
	.keypad_gpio[0] = 130,
	.keypad_gpio[1] = 131,
	.keypad_gpio[2] = 132,
	.keypad_gpio[3] = 133,
	.keypad_gpio[4] = 134,
	.keypad_gpio[5] = 135,
	.keypad_gpio[6] = 136,
	.keypad_gpio[7] = 137,
};

DEVICE_ATTR(keyinput, 0666, NULL, NULL);

static struct attribute* keypad_attr[] = 
{
   &dev_attr_keyinput.attr,
   NULL
};

static struct attribute_group keypad_attr_group = 
{
   .attrs = keypad_attr,
};

static void keypad_timer(unsigned long funct_parameter){
	int i, j;
 // Re-schedule the timer
	numeric_keypad.keypad_timer.expires = jiffies + numeric_keypad.timeout_in_sec*HZ/10;
	add_timer(&numeric_keypad.keypad_timer);
	
	for(i = 0; i < 4; ++i){
		cpld_gpio_set_value(numeric_keypad.keypad_gpio[i], 0);
		for(j = 4; j < 8; ++j){
			if(!cpld_gpio_get_value(numeric_keypad.keypad_gpio[j])){
				input_report_key(numeric_keypad.keypad_input_dev, keypad_keycode[(i*4)+(j-4)], 1);
				input_report_key(numeric_keypad.keypad_input_dev, keypad_keycode[(i*4)+(j-4)], 0);
			}
		}
		cpld_gpio_set_value(numeric_keypad.keypad_gpio[i], 1);
	}
	input_sync(numeric_keypad.keypad_input_dev);
	printk("Input got synced\n");
}
	

static int __init keypad_init(void)
{	
	int err, i;
	
	for(i = 0; i < NR_KEYPAD_GPIO; ++i){
		err = cpld_gpio_request(numeric_keypad.keypad_gpio[i]);
		if(err){
			printk(KERN_WARNING "Failed obtaining GPIO 130");
			goto fail_gpio;
		}
	}
	for(i = 0; i < 4; ++i){	
		cpld_gpio_direction_output(numeric_keypad.keypad_gpio[i],1);
		if(err){
			printk(KERN_WARNING "Failed obtaining GPIO 130");
			goto fail_gpio;
		}
	}
	for(i = 4; i < 8; ++i){	
		cpld_gpio_direction_input(numeric_keypad.keypad_gpio[i]);
		if(err){
			printk(KERN_WARNING "Failed obtaining GPIO 130");
			goto fail_gpio;
		}
	}
	
	numeric_keypad.keypad_dev = platform_device_register_simple("keypad", -1, NULL, 0);
	if(IS_ERR(numeric_keypad.keypad_dev))
   {
      PTR_ERR(numeric_keypad.keypad_dev);
      printk("keypad_init: error\n");
	  goto fail_gpio;
   }

    (void*)sysfs_create_group(&numeric_keypad.keypad_dev->dev.kobj, &keypad_attr_group);
   
	numeric_keypad.keypad_input_dev = input_allocate_device();
	
	if(!numeric_keypad.keypad_input_dev)
    {
      printk("Bad input_alloc_device()\n");
	  goto fail_platform;
    }
	
	numeric_keypad.keypad_input_dev->name = "IHA-Keypad";
    numeric_keypad.keypad_input_dev->phys = "keypad/input0";
    numeric_keypad.keypad_input_dev->id.bustype = BUS_HOST;
    numeric_keypad.keypad_input_dev->id.vendor = 0x0002;
    numeric_keypad.keypad_input_dev->id.product = 0x0002;
    numeric_keypad.keypad_input_dev->id.version = 0x0300;

    numeric_keypad.keypad_input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
    numeric_keypad.keypad_input_dev->keycode = keypad_keycode;
    numeric_keypad.keypad_input_dev->keycodesize = sizeof(unsigned char);
    numeric_keypad.keypad_input_dev->keycodemax = ARRAY_SIZE(keypad_keycode);
	
	for (i = 0; i < numeric_keypad.keypad_input_dev->keycodemax; i++) {
      set_bit(keypad_keycode[i], numeric_keypad.keypad_input_dev->keybit);
    }
	
	input_register_device(numeric_keypad.keypad_input_dev);
	
	numeric_keypad.timeout_in_sec = 2;
	numeric_keypad.gpio_state = 1;
	
	//Timer initialization we add a timer to our list specify a timeout and a timer funktion
	init_timer(&numeric_keypad.keypad_timer);
	numeric_keypad.keypad_timer.expires = jiffies + numeric_keypad.timeout_in_sec*HZ;
	numeric_keypad.keypad_timer.function = keypad_timer;
	numeric_keypad.keypad_timer.data = 164;
	add_timer(&numeric_keypad.keypad_timer);

	return 0;
	
	
	//Fail flags we gota make sure to release everything that we have aquirred up to our fail point!
	fail_platform :
		platform_device_unregister(numeric_keypad.keypad_dev);
	fail_gpio :
		for(i = 0; i < NR_KEYPAD_GPIO; ++i){
			cpld_gpio_free(numeric_keypad.keypad_gpio[i]);
		}
		return -1;
}	


static void __exit keypad_exit(void)
{
	int i;
	del_timer(&numeric_keypad.keypad_timer);
	
	input_unregister_device(numeric_keypad.keypad_input_dev);
	sysfs_remove_group(&numeric_keypad.keypad_dev->dev.kobj, &keypad_attr_group);
	platform_device_unregister(numeric_keypad.keypad_dev);

	for(i = 0; i < NR_KEYPAD_GPIO; ++i){
		cpld_gpio_free(numeric_keypad.keypad_gpio[i]);
	}
	return;
}

module_init(keypad_init);
module_exit(keypad_exit);