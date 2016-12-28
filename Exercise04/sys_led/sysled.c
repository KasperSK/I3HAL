#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

dev_t my_dev;
struct cdev my_cdev;
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

static int __init mygpio_init(void)
{	
	int err;
//	dev_t dev;
	#ifdef DEBUG
	printk(KERN_WARNING "Fetching GPIO");
	#endif
	err = gpio_request(164,"sys_led");
	if(err){
		printk(KERN_WARNING "Failed obtaining GPIO");
		return -1;
	}
	#ifdef DEBUG
	printk(KERN_WARNING "Setting Data Direction");
	#endif
	err = gpio_direction_output(164,1);
	if(err){
		printk(KERN_WARNING "Failed setting direction of GPIO");
		gpio_free(164);
		return -1;
	}
	#ifdef DEBUG
	printk(KERN_WARNING "Allocation MAJOR");
	#endif
	err = alloc_chrdev_region(&my_dev, 0, 1, "led_drive");
	if(err){
		printk(KERN_WARNING "Failed allocationg MAJOR number");
		gpio_free(164);
		return -1;
	}
	#ifdef DEBUG
	printk(KERN_WARNING "Initializing CDEV\n");
	#endif
	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	my_cdev.ops = &my_fops;
	err = cdev_add(&my_cdev, my_dev, 1);
	if(err){
		printk(KERN_WARNING "Failed to register CDEV");
		gpio_free(164);
		return -1;
	}
	return 0;
}	


static void __exit mygpio_exit(void)
{
	cdev_del(&my_cdev);
	unregister_chrdev_region(my_dev, 1);
	gpio_free(164);
}



module_init(mygpio_init);
module_exit(mygpio_exit);
