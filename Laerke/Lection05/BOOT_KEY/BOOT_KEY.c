#include <linux/gpio.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/interrupt.h> 
#include <linux/wait.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static DECLARE_WAIT_QUEUE_HEAD(queue); 
static int irq_cnt=0;
dev_t dev;
struct cdev cdev;

struct file_operations key_fops;

static irqreturn_t mygpio_isr(int irq, void *dev_id)
 { 
   irq_cnt = 1;
   //printk("IRQ event %i\n", irq_cnt ++);
   wake_up_interruptible(&queue);
    
   return IRQ_HANDLED;
 }

static int __init BOOT_KEY_init(void)
{
   int st, gpio_irq; 
   
   st = gpio_request(7,"BOOT_KEY");				// Request GPIO
   if(st){
     printk(KERN_WARNING "Couldt not contain gpio");
     return -1;
    }
   
   st = gpio_direction_input(7);				// Set GPIO direction (in or out)
   if(st){
     printk(KERN_WARNING "fail setting direction");
     gpio_free(7);
     return -1;
    }
   
    gpio_irq = gpio_to_irq(7);					// get interrupt line attached to GPIO port 7
   
   // Make device no (vælg - Enten bruger I statisk eller også bruger I dynamisk major/minor nummer allokering) 
   // Register Device
   st = alloc_chrdev_region(&dev, 0, 1, "BOOT_KEY");
   if(st){
     printk(KERN_WARNING "Fail allocating major");
     gpio_free(7);
     return -1;
    }
    
   /* Request IRQ */
   st = request_irq(gpio_irq, mygpio_isr, IRQF_TRIGGER_RISING, "BOOT_KEY", &dev);
   if(st){
     printk(KERN_WARNING "Faild obtaining gpio");
     gpio_free(7);
     return -1;
    }
   
   // Cdev Init
   cdev_init(&cdev, &key_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &key_fops;
   
   // Add Cdev
   st = cdev_add(&cdev, dev, 1);
   if(st){
     printk(KERN_WARNING "Faild to register character device");
     gpio_free(7);
     return -1;
    }
   

   
   return 0;
}

static void __exit BOOT_KEY_exit(void)
{  
  int gpio_irq;
  
  // Delete Cdev
  cdev_del(&cdev);
  
  gpio_irq = gpio_to_irq(7);
  
  // Free irq
  free_irq(gpio_irq, &dev);
  
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
    return 0; 
}

int mygpio_release(struct inode *inode, struct file *filep)
{
    int minor, major;

    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);
    printk("Closing/Releasing MyGpio Device [major], [minor]: %i, %i\n", major, minor);

    return 0; 
}

ssize_t mygpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    char str = 0;
    char len = 0;
    int var = 0;
    int its = 0;
    
    if(!(filp->f_flags & O_NONBLOCK)){
      //printk("Inside the IF\n");
	wait_event_interruptible(queue, irq_cnt != 0);
	irq_cnt = 0;
    }
   
    var = gpio_get_value(7);
    
    // konvertering fra int til string kan gøres via sprintf()
    
    //printk("før fprint..: %i\n", irq_cnt);
    
    its = sprintf(&str, "%d",var);
    
    len = sizeof(str);
    //printk("len: %i\n", len);
    
    copy_to_user(buf, &str, len);
    
    //printk("efter cp to user: %i\n", irq_cnt);
      
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

