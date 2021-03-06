#include <linux/err.h>
#include <plat/mcspi.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include "ads7870.h"

#define MODULE_DEBUG 0

/* ADS7870 Device Data */
struct ads7870 {
  int revision;
};

/* We are only using ONE SPI device so far */
static struct spi_device *ads7870_spi_device = NULL;

/* Method to retrieve SPI device pointer */
struct spi_device* ads7870_get_device(void){
  return ads7870_spi_device;
}

/*
 * ADS7870 SPI Read 8-bit Register
 * Reads 8-bit content of register at 
 * the provided ADS7870 address
 */
int ads7870_spi_read_reg8(struct spi_device *spi, u8 addr, u8* value)
{
	struct spi_transfer t[2];
	struct spi_message m;
	u8 cmd;
	u8 data = 0;

    /* Check for valid spi device */
    if(!spi)
      return -ENODEV;
    
	/* Create Cmd byte:
	 *
	 * | 0|RD| 8|     ADDR     |
	 *   7  6  5  4  3  2  1  0
     */	 
	cmd = (1<<6) | (0<<5) | (addr & 0x1f);

	/* Init Message */
	memset(t, 0, sizeof(t));
	spi_message_init(&m);
	m.spi = spi;

	/* Configure tx/rx buffers */
	t[0].tx_buf = &cmd;
	t[0].rx_buf = NULL;
	t[0].len = 1;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = NULL;
	t[1].rx_buf = &data;
	t[1].len = 1;
	spi_message_add_tail(&t[1], &m);

	/* Transmit SPI Data (blocking) */
	spi_sync(m.spi, &m);

	if(MODULE_DEBUG)
	  printk(KERN_DEBUG "ADS7870: Read Reg8 Addr 0x%02x Data: 0x%02x\n", cmd, data);

    *value = data;
	return 0;
}

/*
 * ADS7870 SPI Read 16-bit Register
 * Reads 16-bit content of register at 
 * the provided ADS7870 address
 */
int ads7870_spi_read_reg16(struct spi_device *spi, u8 addr, u16* value)
{
	struct spi_transfer t[2];
	struct spi_message m;
	u8 cmd;
	u16 data = 0;

    /* Check for valid spi device */
    if(!spi)
      return -ENODEV;

	/* Create Cmd byte:
	 *
	 * | 0|RD|16|     ADDR     |
	 *   7  6  5  4  3  2  1  0
     */	 
	cmd = (1<<6) | (1<<5) | (addr & 0x1f);

	/* Init Message */
	memset(t, 0, sizeof(t));
	spi_message_init(&m);
	m.spi = spi;

	/* Configure tx/rx buffers */
	t[0].tx_buf = &cmd;
	t[0].rx_buf = NULL;
	t[0].len = 1;
	if(MODULE_DEBUG)
	  printk("requesting data from addr 0x%x\n", cmd);
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = NULL;
	t[1].rx_buf = &data;
	t[1].len = 2;
	spi_message_add_tail(&t[1], &m);

	/* Transmit SPI Data (blocking) */
	spi_sync(m.spi, &m);

	if(MODULE_DEBUG)
	  printk(KERN_DEBUG "ADS7870: Read Reg16 Addr 0x%02x Data: 0x%04x\n", cmd, data);

    *value = data;
	return 0;
}

/*
 * ADS7870 SPI Write 8-bit Register
 * Writes 8-bit content to register at 
 * the provided ADS7870 address
 */
int ads7870_spi_write_reg8(struct spi_device *spi, u8 addr, u8 data)
{
  struct spi_transfer t[2];
  struct spi_message m;
  u8 cmd;

  /* Check for valid spi device */
    if(!spi)
    return -ENODEV;

  
  /* Create Cmd byte:
   *
   * | 0|WR| 8|     ADDR     |
   *   7  6  5  4  3  2  1  0
   */ 
  cmd =  (0<<6) | (0<<5) | (addr & 0x1f);

  /* Init Message */
  memset(&t, 0, sizeof(t)); 
  spi_message_init(&m);
  m.spi = spi;

  if(MODULE_DEBUG)
    printk(KERN_DEBUG "ADS7870: Write Reg8 Addr 0x%x Data 0x%02x\n", addr, data); 
  /* Configure tx/rx buffers */
  t[0].tx_buf = &cmd;
  t[0].rx_buf = NULL;
  t[0].len = 1;
  spi_message_add_tail(&t[0], &m);

  t[1].tx_buf = &data;
  t[1].rx_buf = NULL;
  t[1].len = 1;
  spi_message_add_tail(&t[1], &m);

  /* Transmit SPI Data (blocking) */
  spi_sync(m.spi, &m);

  return 0;
}

/*
 * ADS7870 Probe
 * Used by the SPI Master to probe the device
 * when an SPI device is registered.
 */
static int __devinit ads7870_spi_probe(struct spi_device *spi)
{
  int err;
  u8 value;
  struct ads7870 *adsdev;
  
  printk(KERN_DEBUG "New SPI device: %s using chip select: %i\n",
	 spi->modalias, spi->chip_select);
  
  spi->bits_per_word = 8;  
  spi_setup(spi);

  /* In this case we assume just one device */ 
  ads7870_spi_device = spi;  
 
  err = ads7870_spi_read_reg8(spi, ADS7870_ID, &value);
  printk(KERN_DEBUG "Probing ADS7870, Revision %i\n", 
         value);

  /* Configure ADS7870 according to data sheet */
  ads7870_spi_write_reg8(spi, ADS7870_REFOSC, 
                         ADS7870_REFOSC_OSCR |
                         ADS7870_REFOSC_OSCE |
                         ADS7870_REFOSC_REFE |
                         ADS7870_REFOSC_BUFE |
                         ADS7870_REFOSC_R2V); 		 
		 
  /* Allocate memory for driver's per-chip state */
  adsdev = kzalloc(sizeof *adsdev, GFP_KERNEL);
  if (!adsdev)
    return -ENOMEM;
  adsdev->revision = value;
  spi_set_drvdata(spi, adsdev);  
  
  return err;
}

/*
 * ADS7870 Remove
 * Called when the SPI device is removed
 */
static int __devexit ads7870_remove(struct spi_device *spi)
{
  struct ads7870 *adsdev = dev_get_drvdata(&spi->dev);
  
  ads7870_spi_device = 0;
  
  printk (KERN_ALERT "Removing SPI device %s revision %i on chip select %i\n", 
	  spi->modalias, adsdev->revision, spi->chip_select);

  kfree(adsdev); /* Free kernel memory */

  return 0;
}

/*
 * ADS7870 SPI Driver Struct
 * Holds function pointers to probe/release
 * methods and the name under which it is registered
 *
 */
static struct spi_driver ads7870_spi_driver = {
  .driver = {
    .name = "ads7870",
    .bus = &spi_bus_type,
    .owner = THIS_MODULE,
  },
  .probe = ads7870_spi_probe,
  .remove = __devexit_p(ads7870_remove),
};


/*
 * ADS7870 SPI Init
 * Registers the spi driver with the SPI host
 */
int ads7870_spi_init(void)
{
  int err;

  err = spi_register_driver(&ads7870_spi_driver);
  
  if(err<0)
    printk (KERN_ALERT "Error %d registering the ads7870 SPI driver\n", err);
  
  return err;
}

/*
 * ADS7870 SPI Exit
 * Exit routine called from character driver.
 * Unregisters the driver from the SPI host
 */
void ads7870_spi_exit(void)
{
  spi_unregister_driver(&ads7870_spi_driver); 
}
