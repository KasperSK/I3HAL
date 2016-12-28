#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/module.h>
#include "psoc4-spi.h"

#define PSOC4_MINOR_COUNT 4
#define MAXLEN 20

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

static struct cdev psoc4Dev;
static dev_t devNo;
static struct spi_device *psoc4_spi_device = NULL;

/* Macro to handle Errors */
#define ERRGOTO(label, ...)                     \
  {                                             \
  printk (__VA_ARGS__);                         \
  goto label;                                   \
  } while(0)
	  
int psoc4_cdrv_open(struct inode *inode, struct file *filep)
{
	int major = imajor(inode);
	int minor = iminor(inode);

	printk("Opening PSOC4 Device [major], [minor]: %i, %i\n", major, minor);

	/* Check if minor number is within range */
	if (minor > PSOC4_MINOR_COUNT-1)
	{
		printk("Minor no out of range (0-%i): %i\n",minor, PSOC4_MINOR_COUNT);
		return -ENODEV;
	}

	/* Check if a ads7870 device is registered */
	if(!(psoc4_spi_device = psoc4_get_device()))
		return -ENODEV;
  
	return 0;
}

int psoc4_cdrv_release(struct inode *inode, struct file *filep)
{
	int major = imajor(inode);
	int minor = iminor(inode);

	printk("Closing PSOC4 Device [major], [minor]: %i, %i\n", major, minor);

	if ((minor > PSOC4_MINOR_COUNT-1) || !(psoc4_spi_device = psoc4_get_device()))
		return -ENODEV;
    
	return 0;
}

ssize_t psoc4_cdrv_write(struct file *filep, const char __user *ubuf, size_t count, loff_t *f_pos)
{
	int minor, len; 
	int value;
	char kbuf[MAXLEN];    
    
	minor = MINOR(filep->f_dentry->d_inode->i_rdev);
	
	if(minor == 3){
		printk(KERN_ALERT "Writing to PSOC4 [Minor] %i \n", minor);		
	}else{
		printk(KERN_ALERT "Can only write to PSOC4 [Minor] 3 \n");
		return -ENODEV;
	}
    
	/* Limit copy length to MAXLEN allocated andCopy from user */
	len = count < MAXLEN ? count : MAXLEN;
	if(copy_from_user(kbuf, ubuf, len))
		return -EFAULT;
	
	/* Pad null termination to string */
	kbuf[len] = '\0';   
	
	/* Convert sting to int */
	sscanf(kbuf,"%i", &value);
	
	#ifdef MODULE_DEBUG
		printk("string from user: %s\n", kbuf);
		printk("value %i\n", value);
	#endif
	
	
	psoc4_spi_write_reg8(psoc4_spi_device, minor + 1, (value < 255 ? value : 127 ));

	/* Legacy file ptr f_pos. Used to support 
	* random access but in char drv we dont! 
	* Move it the length actually  written 
	* for compability */
	*f_pos += len;

	/* return length actually written */
	return len;
}

ssize_t psoc4_cdrv_read(struct file *filep, char __user *ubuf, size_t count, loff_t *f_pos)
{
  int minor, len;
  char resultBuf[MAXLEN];
  u16 result;

  minor = MINOR(filep->f_dentry->d_inode->i_rdev);

  #ifdef MODULE_DEBUG
    printk(KERN_ALERT "Reading from psoc4 [Minor] %i \n", minor);
  #endif
	if(minor > 3){
		printk(KERN_ALERT "Can only read from PSOC4 [Minor] 0-2 \n");
		return -ENODEV;
	}
  #ifdef MODULE_DEBUG
    printk(KERN_ALERT "Reading from psoc4 result %d before spi read \n", result);
  #endif

  psoc4_spi_read_reg16(psoc4_spi_device, minor + 1, &result);
  
  #ifdef MODULE_DEBUG
    printk(KERN_ALERT "Reading from psoc4 result %d after spi read \n", result);
  #endif
  
  /* Convert to string and copy to user space */
  //  len = snprintf(resultBuf, sizeof resultBuf, "%d\n", result);
  /* Convert integer to string limited to "count" size. Returns
   * length excluding NULL termination */
  len = snprintf(resultBuf, count, "%d\n", result);
  len++;

  /* Copy data to user space */
  if(copy_to_user(ubuf, resultBuf, len))
    return -EFAULT;

  /* Move fileptr */
  *f_pos += len;

  return len;
}

struct file_operations psoc4_Fops = 
{
  .owner   = THIS_MODULE,
  .open    = psoc4_cdrv_open,
  .release = psoc4_cdrv_release,
  .write   = psoc4_cdrv_write,
  .read    = psoc4_cdrv_read,
};

static int __init psoc4_cdrv_init(void)
{
	int err; 
	
	printk("psoc4 driver initializing\n");
	
	/* call SPI specific function wrapper */
	err = psoc4_spi_init();
	if(err)
		ERRGOTO(error, "Failed SPI initializing\n");
	
	/* dynamicaly allocate MAJOR number */
	err = alloc_chrdev_region(&devNo, 0, PSOC4_MINOR_COUNT, "psoc4");
	if(err)
		ERRGOTO(err_spi_init, "Failed allocating char region error %d\n", err);
	
	/* register character device */
	cdev_init(&psoc4Dev, &psoc4_Fops);
	err = cdev_add(&psoc4Dev, devNo, PSOC4_MINOR_COUNT);
	if (err)
		ERRGOTO(err_char_dev, "Error %d adding PSOC4 device\n", err);
  
	return 0;
	
	/* Step fail */
	err_char_dev:
	unregister_chrdev_region(devNo, PSOC4_MINOR_COUNT);
	
	err_spi_init:
	psoc4_spi_exit();
	
	error:
	return err;
}

static void __exit psoc4_cdrv_exit(void)
{
	printk("PSOC4 Driver says goodbye\n");
	cdev_del(&psoc4Dev);
	unregister_chrdev_region(devNo, PSOC4_MINOR_COUNT);
	psoc4_spi_exit();
}

module_init(psoc4_cdrv_init);
module_exit(psoc4_cdrv_exit);
