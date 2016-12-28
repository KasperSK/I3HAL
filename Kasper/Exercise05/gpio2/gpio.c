#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define NRGPIO 2

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

DECLARE_WAIT_QUEUE_HEAD(key_wait);

struct gpio_ldd{
	struct gpio my_gpios[NRGPIO];
	dev_t my_dev;
	struct cdev my_cdev;
	int wake_me_flag;
	#ifdef READEOF
	char *hat_ptr;
	char ret[2];
	#endif
};

struct gpio_ldd my_driver = {
	.my_gpios[0] = {.gpio = 164 , .flags = GPIOF_OUT_INIT_LOW, .label = "Sys_led4"},
	.my_gpios[1] = {.gpio = 7 , .flags = GPIOF_IN, .label = "Boot_key"}
	};



irqreturn_t mygpio_irq(int irq, void *dev)
{
	my_driver.wake_me_flag = 1;
	wake_up_interruptible(&key_wait);
	return IRQ_HANDLED;
}

int mygpio_open(struct inode *inode, struct file *filep)
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
	sprintf(my_driver.ret, "%d\n",read);
	my_driver.hat_ptr = my_driver.ret;
	#endif
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

#ifdef READEOF
ssize_t mygpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char ret_len = 0;
	if (*my_driver.hat_ptr == 0) return 0;
	while(count && *my_driver.hat_ptr){
		put_user(*(my_driver.hat_ptr++),buf++);
		count--;
		ret_len++;
	}	
	*f_pos += ret_len;
	return ret_len;
}
#else

ssize_t mygpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char ret_len = 0;
	char ret = 0;
	int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	int read = 0;
	if(!(filp->f_flags & O_NONBLOCK)){
		wait_event_interruptible(key_wait, my_driver.wake_me_flag != 0);
		my_driver.wake_me_flag = 0;
	}
	if(minor == 0){
		read = gpio_get_value(164);
		read = !read;
	}else if(minor == 1){
		read = gpio_get_value(7);
	}
	sprintf(&ret, "%d", read);
	ret_len = sizeof(ret);
	if(copy_to_user(buf, &ret, ret_len)){
		printk(KERN_WARNING "Failed to copy to user");
	}
	*f_pos += ret_len;
	return ret_len;
}
#endif

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
	

	err = alloc_chrdev_region(&my_driver.my_dev, 0, 2, "gpio");
	if(err){
		printk(KERN_WARNING "Failed allocationg MAJOR number");
		goto fail_gpio;
	}
	
	getMe = gpio_to_irq(7);
	if(getMe < 0){
		printk(KERN_WARNING "Failed obtaining GPIO to IRQ");
		goto fail_majors;
	}
	err = request_irq(getMe, mygpio_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "boot_key_irq", &my_driver.my_dev);
	if(err){
		printk(KERN_WARNING "Failed obtaining IRQ");
		goto fail_majors;
	}

	cdev_init(&my_driver.my_cdev, &my_fops);
//	my_driver.my_cdev.owner = THIS_MODULE;
//	my_driver.my_cdev.ops = &my_fops;
	err = cdev_add(&my_driver.my_cdev, my_driver.my_dev, 2);
	if(err){
		printk(KERN_WARNING "Failed to register CDEV");
		goto fail_irq;
	}
	return 0;
	
	
	fail_irq :				//fail major and minors!d
		free_irq(getMe, &my_driver.my_dev);
	fail_majors :
		unregister_chrdev_region(my_driver.my_dev, 2);
	fail_gpio :
		gpio_free_array(my_driver.my_gpios, 2);
	fail :
		return -1;
}	


static void __exit mygpio_exit(void)
{
	int freeme = 0;
	freeme = gpio_to_irq(7);
	printk(KERN_WARNING "Fjerner irq: %d", freeme);
	free_irq(freeme, &my_driver.my_dev);
	cdev_del(&my_driver.my_cdev);
	unregister_chrdev_region(my_driver.my_dev, 2);
	gpio_free_array(my_driver.my_gpios, NRGPIO);
}



module_init(mygpio_init);
module_exit(mygpio_exit);
