#include <linux/err.h>
#include <plat/mcspi.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include "dac7612.h"

//#define MODULE_DEBUG


/* We are only using ONE SPI device so far */
static struct spi_device *dac7612_spi_device = NULL;

/* Method to retrieve SPI device pointer */
struct spi_device* dac7612_get_device(void){
  return dac7612_spi_device;
}

/*
 * dac7612 SPI Write 8-bit Register
 * Writes 8-bit content to register at 
 * the provided dac7612 address
 */
int dac7612_spi_write_reg16(struct spi_device *spi, u16 addr, u16 data)
{
	struct spi_transfer t;
	struct spi_message m;
	u16 cmd;

	/* Check for valid spi device */
	if(!spi)
		return -ENODEV;

	/* We write a 14 bit word to the DAC first two bits are adress last twelve are data */
	
	cmd =  ((addr + 2) << 12) | (data);
	
	#ifdef MODULE_DEBUG
	printk(KERN_ALERT "Writing to DAC7612 [cmd] %i \n", cmd);
	printk(KERN_ALERT "Writing to DAC7612 [addr] %i \n", addr);
	printk(KERN_ALERT "Writing to DAC7612 [data] %i \n", data);
	#endif
	
	/* Init Message */
	memset(&t, 0, sizeof(t)); 
	spi_message_init(&m);
	m.spi = spi;
		
	/* Configure tx/rx buffers */
	t.tx_buf = &cmd;
	t.rx_buf = NULL;
	t.len = 2;
	spi_message_add_tail(&t, &m);

	/* Transmit SPI Data (blocking) */
	spi_sync(m.spi, &m);

	return 0;
}

/*
 * dac7612 Probe
 * Used by the SPI Master to probe the device
 * when an SPI device is registered.
 */
static int __devinit dac7612_spi_probe(struct spi_device *spi)
{
	int err = 0;
	
	printk(KERN_DEBUG "New SPI device: %s using chip select: %i\n",
		spi->modalias, spi->chip_select);
  
	spi->bits_per_word = 14;  
	spi_setup(spi);

	/* In this case we assume just one device */ 
	dac7612_spi_device = spi; 
	
	return err;
}

/*
 * dac7612 Remove
 * Called when the SPI device is removed
 */
static int __devexit dac7612_remove(struct spi_device *spi)
{
	dac7612_spi_device = 0;
  
	printk (KERN_ALERT "Removing SPI device %s on chip select %i\n", 
		spi->modalias, spi->chip_select);

	return 0;
}

/*
 * dac7612 SPI Driver Struct
 * Holds function pointers to probe/release
 * methods and the name under which it is registered
 *
 */
static struct spi_driver dac7612_spi_driver = {
	.driver = { .name = "dac7612", .bus = &spi_bus_type, .owner = THIS_MODULE, },
	.probe = dac7612_spi_probe,
	.remove = __devexit_p(dac7612_remove),
};


/*
 * dac7612 SPI Init
 * Registers the spi driver with the SPI host
 */
int dac7612_spi_init(void)
{
	int err;

	err = spi_register_driver(&dac7612_spi_driver);
	if(err<0)
		printk (KERN_ALERT "Error %d registering the dac7612 SPI driver\n", err);
  
	return err;
}

/*
 * dac7612 SPI Exit
 * Exit routine called from character driver.
 * Unregisters the driver from the SPI host
 */
void dac7612_spi_exit(void)
{
	spi_unregister_driver(&dac7612_spi_driver); 
}
