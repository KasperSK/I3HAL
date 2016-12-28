#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

dev_t dev;
struct cdev cdev;

struct file_operations key_fops;

static int __init BOOT_KEY_init(void)
{
   int st; 
   
   // Request GPIO
   st = gpio_request(7,"BOOT_KEY");
   
   // Set GPIO direction (in or out)
   st = gpio_direction_input(7);
   
   // Make device no (vælg - Enten bruger I statisk eller også bruger I dynamisk major/minor nummer allokering) 
   // Register Device
   st = alloc_chrdev_region(&dev, 0, 1, "BOOT_KEY");
   
   // Cdev Init
   cdev_init(&cdev, &key_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &key_fops;
   
   // Add Cdev
   cdev_add(&cdev, dev, 1);
   
   return 0;
}

static void __exit BOOT_KEY_exit(void)
{  
  // Delete Cdev
  cdev_del(&cdev);
  
  // Unregister Device
  unregister_chrdev_region(dev, 1);
  
  // Free GPIO 
  gpio_free(7);
}

// her placeres open release read write
int mygpio_open(struct inode *inode, struct file *filep)
{
    int major, minor;
    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);
    printk("Opening MyGpio Device [major], [minor]: %i, %i\n", major, minor);
    return 0; //??
}

int mygpio_release(struct inode *inode, struct file *filep)
{
    int minor, major;

    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);
    printk("Closing/Releasing MyGpio Device [major], [minor]: %i, %i\n", major, minor);

    return 0; //??
}

ssize_t mygpio_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos)
{
    char str[1];
    int var;
    var = gpio_get_value(7);
    
    // Hint konvertering fra int til string kan gøres via sprintf() - antagelsen er at det er strenge der    sendes til og fra user space. Det gør debugging lettere.
    
    int its;
    its = sprintf(str, "%d",var);
    
    char len = sizeof(str);
    copy_to_user(buf, str, len);
    
  
    *f_pos += len;
    return len;    
}


struct file_operations key_fops = {
  .owner 	= THIS_MODULE,
  .read  	= mygpio_read,
  .open  	= mygpio_open,
  .release	= mygpio_release
};


module_init(BOOT_KEY_init);
module_exit(BOOT_KEY_exit);

