#include <linux/err.h>
#include <plat/mcspi.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include "psoc4-spi.h"



static struct spi_device* psoc4_spi_device = NULL;

/* Function to get spi device */
struct spi_device* psoc4_get_device(void)
{
	return psoc4_spi_device;
}

/* PSoC4 SPI write function. */
int psoc4_spi_write_reg8(struct spi_device *spi, u8 addr, u8 data)
{
	struct spi_transfer t;
	struct spi_message m;
	u16 cmd = 0;
	
	if(!spi)
		return -ENODEV;
	
	cmd = 0x4000 | (addr << 8) | data;
	
	/* Initialize message */
	memset(&t, 0, sizeof(t));
	spi_message_init(&m);
	m.spi = spi;
	
	/* Configure tx/rx buffer */
	t.tx_buf = &cmd;
	t.rx_buf = NULL;
	t.len = 2;
	
	spi_message_add_tail(&t, &m);
	
	spi_sync(m.spi, &m);
	
	return 0;
} 

/* PSoC4 SPI read function */
int psoc4_spi_read_reg16(struct spi_device *spi, u8 addr, u16* data)
{
	struct spi_transfer t[2];
	struct spi_message m;
	u16 cmdAddr;
	
	if(!spi)
		return -ENODEV;
	
	memset(&t, 0, sizeof(t));
	spi_message_init(&m);
	m.spi = spi;
	
	cmdAddr = (u16)addr << 8;
	
	#ifdef MODULE_DEBUG
		printk(KERN_ALERT "psoc4 Addr %i \n", cmdAddr);
    #endif
	
	/* Configure tx/rx buffers */
	t[0].tx_buf = &cmdAddr;
	t[0].rx_buf = NULL;
	t[0].len = 2;
	t[0].delay_usecs = 60;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = NULL;
	t[1].rx_buf = data;
	t[1].len = 2;
	spi_message_add_tail(&t[1], &m);

	/* Transmit SPI Data (blocking) */
	spi_sync(m.spi, &m);
	
	return 0;
}

static int __devinit psoc4_spi_probe(struct spi_device *spi)
{
	
	printk(KERN_DEBUG "New SPI device: %s using chip select: %i\n",
		spi->modalias, spi->chip_select);
		
	spi->bits_per_word = 16;
	spi_setup(spi);
	
	psoc4_spi_device = spi;
	return 0;
}

static int __devexit psoc4_remove(struct spi_device *spi)
{
    psoc4_spi_device = 0;
  
	printk (KERN_ALERT "Removing SPI device %s on chip select %i\n", 
		spi->modalias, spi->chip_select);

	return 0;
}

static struct spi_driver psoc4_spi_driver = {
	.driver = {.name = "psoc4", .bus = &spi_bus_type, .owner = THIS_MODULE,},
	.probe = psoc4_spi_probe,
	.remove = __devexit_p(psoc4_remove),
};

int psoc4_spi_init(void)
{
  int err;

  err = spi_register_driver(&psoc4_spi_driver);
  
  if(err<0)
    printk (KERN_ALERT "Error %d registering the psoc4 SPI driver\n", err);
  
  return err;
}

void psoc4_spi_exit(void)
{
	spi_unregister_driver(&psoc4_spi_driver); 
}