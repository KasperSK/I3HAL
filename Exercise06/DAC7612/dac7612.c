#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/module.h>
#include "dac7612-spi.h"

#define DAC7612_MAJOR        0
#define DAC7612_MINOR        0
#define MAXLEN              64
#define NBR_DAC_CH           2
#define COMPLIMENTARY_BIT   11

//#define MODULE_DEBUG 

/* Char Driver Globals */
static struct cdev dac7612Dev;
static dev_t devno;

/* Pointer to SPI Device */
static struct spi_device *dac7612_spi_device = NULL;

/* Macro to handle Errors */
#define ERRGOTO(label, ...)                     \
  {                                             \
  printk (__VA_ARGS__);                         \
  goto label;                                   \
  } while(0)

/*
 * PUBLIC METHODS
 */

int dac7612_cdrv_open(struct inode *inode, struct file *filep)
{
	int major = imajor(inode);
	int minor = iminor(inode);

	printk("Opening DAC7612 Device [major], [minor]: %i, %i\n", major, minor);

	/* Check if minor number is within range */
	if (minor > NBR_DAC_CH-1)
	{
		printk("Minor no out of range (0-%i): %i\n", NBR_DAC_CH, minor);
		return -ENODEV;
	}

	/* Check if a ads7870 device is registered */
	if(!(dac7612_spi_device = dac7612_get_device()))
		return -ENODEV;
  
	return 0;
}

int dac7612_cdrv_release(struct inode *inode, struct file *filep)
{
	int major = imajor(inode);
	int minor = iminor(inode);

	printk("Closing DAC7612 Device [major], [minor]: %i, %i\n", major, minor);

	if ((minor > NBR_DAC_CH-1) || !(dac7612_spi_device=dac7612_get_device()))
		return -ENODEV;
    
	return 0;
}

ssize_t dac7612_cdrv_write(struct file *filep, const char __user *ubuf, size_t count, loff_t *f_pos)
{
	int minor, len; 
	u16 value;
	char kbuf[MAXLEN];    
    
	minor = MINOR(filep->f_dentry->d_inode->i_rdev);

	printk(KERN_ALERT "Writing to DAC7612 [Minor] %i \n", minor);
    
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
	
	dac7612_spi_write_reg16(dac7612_spi_device, minor, (value < 4000 ? value : 4000 ));

	/* Legacy file ptr f_pos. Used to support 
	* random access but in char drv we dont! 
	* Move it the length actually  written 
	* for compability */
	*f_pos += len;

	/* return length actually written */
	return len;
}

ssize_t dac7612_cdrv_read(struct file *filep, char __user *ubuf, size_t count, loff_t *f_pos)
{
	int minor, len = 0;
    
	minor = MINOR(filep->f_dentry->d_inode->i_rdev);

	printk(KERN_ALERT "Reading from dac7612 [Minor] %i is not possible only write oprations are available \n", minor);
    
	*f_pos += len;
	return len;
}



struct file_operations dac7612_Fops = 
{
  .owner   = THIS_MODULE,
  .open    = dac7612_cdrv_open,
  .release = dac7612_cdrv_release,
  .write   = dac7612_cdrv_write,
  .read    = dac7612_cdrv_read,
};

static int __init dac7612_cdrv_init(void)
{
	int err; 
  
	printk("DAC7612 driver initializing\n");  

	/* Register SPI Driver */
	err=dac7612_spi_init();
	if(err)
		ERRGOTO(error, "Failed SPI Initialization\n");
  
	/* Allocate chrdev region */
	err = alloc_chrdev_region(&devno, 0, NBR_DAC_CH, "dac7612");
	if(err)
		ERRGOTO(err_spi_init, "Failed allocating char region (%d,%d) +%d, error %d\n",
			DAC7612_MAJOR, DAC7612_MINOR, NBR_DAC_CH , err);
  
	/* Register Char Device */
	cdev_init(&dac7612Dev, &dac7612_Fops);
	err = cdev_add(&dac7612Dev, devno, NBR_DAC_CH );
	if (err)
		ERRGOTO(err_register, "Error %d adding DAC7612 device\n", err);
  
	return 0;
  
	err_register:
	unregister_chrdev_region(devno, NBR_DAC_CH );

	err_spi_init:
	dac7612_spi_exit();
  
	error:
	return err;
}

static void __exit dac7612_cdrv_exit(void)
{
  printk("DAC7612 driver Exit\n");
  cdev_del(&dac7612Dev);

  unregister_chrdev_region(devno, NBR_DAC_CH);

  dac7612_spi_exit();
}

module_init(dac7612_cdrv_init);
module_exit(dac7612_cdrv_exit);

MODULE_AUTHOR("KTIB");
MODULE_LICENSE("GPL");

