#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/timer.h>

#define NRGPIO 2

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

struct gpio_ldd{
	struct gpio my_gpios[NRGPIO];
	dev_t my_dev;
	struct cdev my_cdev;
	struct class* my_class;
	struct device* my_device;
	struct timer_list my_timer;
	unsigned int timer_running;
	unsigned int gpio_state;
	unsigned int timeout_in_sec;
};

struct gpio_ldd my_driver = {
	.my_gpios[0] = {.gpio = 164 , .flags = GPIOF_OUT_INIT_HIGH, .label = "Sys_led4"},
	.my_gpios[1] = {.gpio = 163 , .flags = GPIOF_OUT_INIT_HIGH, .label = "Sys_led3"},
	};
	
static void timer_funct(unsigned long funct_parameter){
 // Re-schedule the timer
	if(my_driver.gpio_state){
		my_driver.my_timer.expires = jiffies + my_driver.timeout_in_sec*HZ;
		add_timer(&my_driver.my_timer);
	}
	gpio_set_value(funct_parameter, !gpio_get_value(funct_parameter)); 
	
}

static ssize_t gpio_toggle_rate_show(struct device *dev, struct device_attribute *attr, char *buf) 
{
	int *gpio_used_ptr = dev_get_drvdata(dev);
	printk("Hello from show rate %i is the gpio in use\n",*gpio_used_ptr);
	return sprintf(buf, "%d\n", my_driver.timeout_in_sec);
}

static ssize_t gpio_toggle_rate_store(struct device *dev, struct device_attribute *attr, char *buf, size_t size)
{
	int *gpio_used_ptr = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned long value;
	
	if((ret = kstrtoul(buf, 10, &value)) < 0)
		return ret;
	ret = size;

	printk("Used store to set gpio %i to %i\n", *gpio_used_ptr, (int)value);
	my_driver.timeout_in_sec = value;
	return size;
}

static ssize_t gpio_toggle_state_show(struct device *dev, struct device_attribute *attr, char *buf) 
{
	int *gpio_used_ptr = dev_get_drvdata(dev);
	printk("Hello from show state %i is the gpio in use\n",*gpio_used_ptr);
	return sprintf(buf, "%d\n", my_driver.gpio_state);
}

static ssize_t gpio_toggle_state_store(struct device *dev, struct device_attribute *attr, char *buf, size_t size)
{
	int *gpio_used_ptr = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned long value;
	
	if((ret = kstrtoul(buf, 10, &value)) < 0)
		return ret;
	ret = size;

	printk("Used store to set gpio %i to %i\n", *gpio_used_ptr, (int)value);
	my_driver.gpio_state = value;
	if(value){
		my_driver.my_timer.expires = jiffies + my_driver.timeout_in_sec*HZ;
		add_timer(&my_driver.my_timer);
	}
	return ret;
}

static struct device_attribute gpio_sys_attr[] = { 
	__ATTR(toggle_rate, 0666, gpio_toggle_rate_show, gpio_toggle_rate_store),
	__ATTR(toggle_state, 0666, gpio_toggle_state_show, gpio_toggle_state_store),
	__ATTR_NULL,
};

int mygpio_open(struct inode *inode, struct file *filep)
{
	int major, minor;
	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);
	printk("Opening MyGpio device [major], [minor]: %i, %i\n", major, minor);
	return 0;

}

int mygpio_release(struct inode *inode, struct file *filep)
{
	int major, minor;
	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);
	printk("Closing MyGpio device [major], [minor]: %i, %i\n", major, minor);
	return 0;

}

ssize_t mygpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char ret_len = 0;
	char ret = 0;
	int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	int read = 0;

	read = gpio_get_value(164);
	read = !read;

	sprintf(&ret, "%d", read);
	ret_len = sizeof(ret);
	if(copy_to_user(buf, &ret, ret_len)){
		printk(KERN_WARNING "Failed to copy to user");
	}
	*f_pos += ret_len;
	return ret_len;
}


ssize_t mygpio_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	char in = 0;
	int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	int out = 0;
	int ret = 0;
	if(minor == 0)
	{
		if(len > 0){
			get_user(in,buf);
			ret = 1;
			printk(KERN_WARNING "Modtaget %d\n",in);
		}
		if((in >= '0') && (in <= '1')){
			out = in - '0';
			gpio_set_value(164, !out);
		}else{
			printk(KERN_WARNING "Please write 1 or 0\n");
		}
	}
	return ret;
}

struct file_operations my_fops = {

	.owner = 	THIS_MODULE,
	.read =		mygpio_read,
	.open = 	mygpio_open,
	.write = 	mygpio_write,
	.release = 	mygpio_release,
};

static int __init mygpio_init(void)
{	
	int err, getMe;
	
	err = gpio_request_array(my_driver.my_gpios, NRGPIO);
	if(err){
		printk(KERN_WARNING "Failed obtaining GPIOS");
		goto fail;
	}
	

	err = alloc_chrdev_region(&my_driver.my_dev, 0, NRGPIO, "gpio");
	if(err){
		printk(KERN_WARNING "Failed allocationg MAJOR number");
		goto fail_gpio;
	}

	cdev_init(&my_driver.my_cdev, &my_fops);
	err = cdev_add(&my_driver.my_cdev, my_driver.my_dev, 2);
	if(err){
		printk(KERN_WARNING "Failed to register CDEV");
		goto fail_majors;
	}
	my_driver.timeout_in_sec = 2;
	my_driver.gpio_state = 1;
	
	init_timer(&my_driver.my_timer);
	my_driver.my_timer.expires = jiffies + my_driver.timeout_in_sec*HZ;
	my_driver.my_timer.function = timer_funct;
	my_driver.my_timer.data = 164;
	add_timer(&my_driver.my_timer);

	
	my_driver.my_class = class_create(THIS_MODULE, "gpio_led");
	my_driver.my_class->dev_attrs = gpio_sys_attr;
	my_driver.my_device = device_create(my_driver.my_class, NULL, my_driver.my_dev, &my_driver.my_gpios[0].gpio ,"gpio_sys_led");
	return 0;
	
	
	fail_majors :
		unregister_chrdev_region(my_driver.my_dev, 2);
	fail_gpio :
		gpio_free_array(my_driver.my_gpios, 2);
	fail :
		return -1;
}	


static void __exit mygpio_exit(void)
{
	del_timer(&my_driver.my_timer);
	device_destroy(my_driver.my_class, my_driver.my_dev);
	class_destroy(my_driver.my_class);
	cdev_del(&my_driver.my_cdev);
	unregister_chrdev_region(my_driver.my_dev, 2);
	gpio_free_array(my_driver.my_gpios, NRGPIO);
}



module_init(mygpio_init);
module_exit(mygpio_exit);
