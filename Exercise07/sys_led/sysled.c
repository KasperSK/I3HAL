#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

dev_t my_dev;                   /* Allotted device number*/
struct class *sys_led_class;    /* Class  */
struct device *sys_led_device;	/* Device */

struct timer_list my_timer;		/* Timer list */
unsigned int timeout_in_sec  = 1;

struct cdev my_cdev;            /* Associated cdev */

static char *hat_ptr;
char ret[2];

int boot_open(struct inode *inode, struct file *filep)
{
	int read;
	int major, minor;
	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);
	printk("Opening MyGpio device [major], [minor]: %i, %i\n", major, minor);
	read = gpio_get_value(164);
	sprintf(ret, "%d\n",read);
	hat_ptr = ret;
	return 0;

}

ssize_t boot_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char ret_len = 0;
	if (*hat_ptr == 0) return 0;
	while(count && *hat_ptr){
		put_user(*(hat_ptr++),buf++);
		count--;
		ret_len++;
	}	
	*f_pos += ret_len;
	return ret_len;
}

ssize_t boot_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	char in;
	int out = 0;
	int ret = 0;
	if(len > 0){
		get_user(in,buf);
		ret = 1;
		#ifdef DEBUG
		printk(KERN_WARNING "Modtaget %d\n",in);
		#endif
	}
	if((in >= '0') && (in <= '1')){
		out = in - '0';
		gpio_set_value(164, !out);
	}else{
		printk(KERN_WARNING "Please write 1 or 0\n");
	}
	return ret;
}

struct file_operations my_fops = {
	.owner = 	THIS_MODULE,
	.read =		boot_read,
	.open = 	boot_open,
	.write = 	boot_write,
//	.release = 	boot_release,
};

/* Timer function */
static void timer_funct(unsigned long funct_parameter)
{
    my_timer.expires = jiffies + timeout_in_sec*HZ;
    add_timer(&my_timer);
	
	if(gpio_get_value(164)){
		gpio_set_value(164, 0);
	}else{
		gpio_set_value(164, 1);
	}
}

/* Sysfs "read" method prototype (show() function) */
//rate
static ssize_t gpio_toggle_rate_show(struct device *dev, struct device_attribute *attr, char *buf){
        
		//printk("hello from show!");
        
		return sprintf(buf, "%d\n", timeout_in_sec);
}
//state
static ssize_t gpio_toggle_state_show(struct device *dev, struct device_attribute *attr, char *buf){
        
        return sprintf(buf, "%d\n", (int)timer_pending(&my_timer));
}

/* Sysfs "write" method prototype (store() function)*/
//rate
static ssize_t gpio_toggle_rate_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size){
	ssize_t ret = -EINVAL;
	unsigned long value;
	
	if((ret = kstrtoul(buf, 10, &value))<0){
		return ret;
	}
	
	ret = size;
	timeout_in_sec = value;
	return ret;
}

//state
static ssize_t gpio_toggle_state_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size){
	ssize_t ret = -EINVAL;
	unsigned long value;
	
	if((ret = kstrtoul(buf, 10, &value))<0){
		return ret;
	}
	
    ret = size;

	if(value == 0 && timer_pending(&my_timer)){
		del_timer_sync(&my_timer);
	}

	if(value == 1 && !timer_pending(&my_timer)){
        my_timer.expires = jiffies + timeout_in_sec*HZ;
        add_timer(&my_timer);
    }
        return ret;
}

/* create attributes*/
static struct device_attribute led_class_attrs[] = {
        __ATTR(toggle_rate, 0666, gpio_toggle_rate_show, gpio_toggle_rate_store),
		__ATTR(toggle_state, 0666, gpio_toggle_state_show, gpio_toggle_state_store),
        __ATTR_NULL,
};

static int __init mygpio_init(void){
	int err;

	err = gpio_request(164,"sys_led");
	if(err){
		printk(KERN_WARNING "Failed obtaining GPIO");
		goto fail;
	}
	
	err = gpio_direction_output(164,1);
	if(err){
		printk(KERN_WARNING "Failed setting direction of GPIO");
		goto fail_gpio;
	}
        
    /*Request dynamic allocation of a device major number*/
	err = alloc_chrdev_region(&my_dev, 0, 1, "led_drive");
	if(err){
		printk(KERN_WARNING "Failed allocationg MAJOR number");
		goto fail_gpio;
	}
	
	/*Create the led class, ie a folder in sysfs - /sys/class/sys_led*/
    sys_led_class = class_create(THIS_MODULE, "sys_led");
    if(IS_ERR(sys_led_class)){
		printk("Failed to create class\n");
		goto fail_majors;
    }
	
	sys_led_class->dev_attrs = led_class_attrs;
	
    /*create an object (device), ie a file under the class sys_led*/
    sys_led_device = device_create(sys_led_class, NULL, my_dev, NULL, "sys_led");
	if(IS_ERR(sys_led_device))
    {
        printk(KERN_ALERT "Failed to create device\n");
		goto fail_free_class;
    }
	  
    /*Connect the major/minor to the cdev*/
	cdev_init(&my_cdev, &my_fops);

	err = cdev_add(&my_cdev, my_dev, 1);
	if(err){
		printk(KERN_WARNING "Failed to register CDEV");
		goto fail_majors;
	}
	
	/* Timer */
	init_timer(&my_timer);
    my_timer.expires = jiffies + timeout_in_sec*HZ;	
    my_timer.function = timer_funct;
	my_timer.data = timeout_in_sec;
	add_timer(&my_timer);

	
	return 0;
	
	/*goto in case of errors*/
    fail_free_class:
		class_destroy(sys_led_class);
	fail_majors :
		unregister_chrdev_region(my_dev, 1);
	fail_gpio :
		gpio_free(164);
	fail :
		return -1;
}	


static void __exit mygpio_exit(void){
	cdev_del(&my_cdev);
	unregister_chrdev_region(my_dev, 1);
        
	/*Destroy device from device model / sysfs*/
	device_destroy(sys_led_class, my_dev);
        
	/*Remove class from device modal / sysfs*/
	class_destroy(sys_led_class);
        
	gpio_free(164);
}



module_init(mygpio_init);
module_exit(mygpio_exit);
