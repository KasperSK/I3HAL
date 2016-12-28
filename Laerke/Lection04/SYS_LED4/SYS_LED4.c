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

static int __init SYS_LED4_init(void)
{
   int st; 
   
   // Request GPIO
   st = gpio_request(7,"SYS_LED4");
   
   // Set GPIO direction (in or out)
   st = gpio_direction_output(164, 0);
   
   
   
   // Make device no (vælg - Enten bruger I statisk eller også bruger I dynamisk major/minor nummer allokering) 
   // Register Device
   st = alloc_chrdev_region(&dev, 0, 1, "SYS_LED4");
   
   // Cdev Init
   cdev_init(&cdev, &key_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &key_fops;
   
   // Add Cdev
   cdev_add(&cdev, dev, 1);
   
   return 0;
}

static void __exit SYS_LED4_exit(void)
{  
  // Delete Cdev
  cdev_del(&cdev);
  
  // Unregister Device
  unregister_chrdev_region(dev, 1);
  
  // Free GPIO 
  gpio_free(164);
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
    var = gpio_get_value(164);
    
    // Hint konvertering fra int til string kan gøres via sprintf() - antagelsen er at det er strenge der    sendes til og fra user space. Det gør debugging lettere.
    
    int its;
    its = sprintf(str, "%d",var);
    
    char len = sizeof(str);
    copy_to_user(buf, str, len);
    
  
    *f_pos += len;
    return len;    
}

ssize_t mygpio_write(struct file *filep, const char __user *ubuf, size_t count, loff_t *f_pos)
{
  int minor;
  
  minor = MINOR(filep->f_dentry->d_inode->i_rdev);
  // Hint konvertering fra string til int kan gøres via sscanf() - antagelsen er at det er strenge der sendes til og fra user space. Det debugging lettere
  char str[1];
  
  int var = 0;
  int st = 0;
  
  st = copy_from_user(str, ubuf, 1);
  
  if(st != 0){
    printk("error: st is %d", st);
  }
  
  sscanf(str, "%d", &var);
  
  gpio_set_value(164, var);
  
  *f_pos += count;
 
  return count;
}

struct file_operations key_fops = {
  .owner 	= THIS_MODULE,
  .read  	= mygpio_read,
  .write 	= mygpio_write,
  .open  	= mygpio_open,
  .release	= mygpio_release
};


module_init(SYS_LED4_init);
module_exit(SYS_LED4_exit);

