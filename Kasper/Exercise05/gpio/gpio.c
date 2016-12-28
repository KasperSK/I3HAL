#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

dev_t my_dev;
struct cdev my_cdev;
DECLARE_WAIT_QUEUE_HEAD(key_wait);
int wake_me_flag = 0;

#ifdef READEOF
static char *hat_ptr;
char ret[2];
#endif

irqreturn_t boot_irq(int irq, void *dev)
{
	wake_me_flag = 1;
	wake_up_interruptible(&key_wait);
	return IRQ_HANDLED;
}

int boot_open(struct inode *inode, struct file *filep)
{
	#ifdef READEOF
	int read = 0;
	#endif
	int major, minor;
	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);
	printk("Opening MyGpio device [major], [minor]: %i, %i\n", major, minor);
	#ifdef READEOF
	if(minor == 0){
		read = gpio_get_value(164);
		read = !read;
	}else if(minor == 1){
		read = gpio_get_value(7);
	}	
	sprintf(ret, "%d\n",read);
	hat_ptr = ret;
	#endif
	return 0;

}

int boot_release(struct inode *inode, struct file *filep)
{
	int major, minor;
	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);
	printk("Closing MyGpio device [major], [minor]: %i, %i\n", major, minor);
	return 0;

}

#ifdef READEOF
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
#else

ssize_t boot_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char ret_len = 0;
	char ret = 0;
	int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	int read = 0;
	if(!(filp->f_flags & O_NONBLOCK)){
		wait_event_interruptible(key_wait, wake_me_flag != 0);
		wake_me_flag = 0;
	}
	if(minor == 0){
		read = gpio_get_value(164);
		read = !read;
	}else if(minor == 1){
		read = gpio_get_value(7);
	}
	sprintf(&ret, "%d", read);
	ret_len = sizeof(ret);
	copy_to_user(buf, &ret, ret_len);
	*f_pos += ret_len;
	return ret_len;
}
#endif

ssize_t boot_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	char in;
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
	.read =		boot_read,
	.open = 	boot_open,
	.write = 	boot_write,
	.release = 	boot_release,
};

static int __init mygpio_init(void)
{	
	int err, getMe;
	#ifdef DEBUG
	printk(KERN_WARNING "Fetching GPIO");
	#endif
	
	err = gpio_request(164,"sys_led");
	if(err){
		printk(KERN_WARNING "Failed obtaining GPIO");
		goto fail;
	}
	
	#ifdef DEBUG
	printk(KERN_WARNING "Setting Data Direction");
	#endif
	err = gpio_direction_output(164,1);
	if(err){
		printk(KERN_WARNING "Failed setting direction of GPIO");
		goto fail;
	}
	
	err = gpio_request(7,"boot_key");
	if(err){
		printk(KERN_WARNING "Failed obtaining GPIO");
		goto fail;
	}
	
	#ifdef DEBUG
	printk(KERN_WARNING "Setting Data Direction");
	#endif
	err = gpio_direction_input(7);
	if(err){
		printk(KERN_WARNING "Failed setting direction of GPIO");
		goto fail;
	}
	
	#ifdef DEBUG
	printk(KERN_WARNING "Allocation MAJOR");
	#endif
	err = alloc_chrdev_region(&my_dev, 0, 2, "gpio");
	if(err){
		printk(KERN_WARNING "Failed allocationg MAJOR number");
		goto fail;
	}
	
	getMe = gpio_to_irq(7);
	if(getMe < 0){
		printk(KERN_WARNING "Failed obtaining GPIO to IRQ");
		goto fail;
	}
	err = request_irq(getMe, boot_irq, IRQF_TRIGGER_RISING, "boot_key_irq", &my_dev);
	if(err){
		printk(KERN_WARNING "Failed obtaining IRQ");
		goto fail;
	}
	#ifdef DEBUG
	printk(KERN_WARNING "Initializing CDEV\n");
	#endif
	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	my_cdev.ops = &my_fops;
	err = cdev_add(&my_cdev, my_dev, 2);
	if(err){
		printk(KERN_WARNING "Failed to register CDEV");
		goto fail_irq;
	}
	return 0;
		
	fail_irq :										//fail major and minors!d
		unregister_chrdev_region(my_dev, 2);
		free_irq(getMe, &my_dev);
	fail :
		gpio_free(7);
		gpio_free(164);
		return -1;
}	


static void __exit mygpio_exit(void)
{
	int freeme = 0;
	freeme = gpio_to_irq(7);
	free_irq(freeme, &my_dev);
	cdev_del(&my_cdev);
	unregister_chrdev_region(my_dev, 2);
	gpio_free(164);
	gpio_free(7);
}



module_init(mygpio_init);
module_exit(mygpio_exit);
