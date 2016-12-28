#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <plat/mcspi.h>
#include <asm/uaccess.h>
#include <linux/spi/spi.h>


/* Who made this module */
MODULE_AUTHOR("KTIB");
MODULE_LICENSE("Dual BSD/GPL");

static struct spi_device *slave_spi_device;

/* OMAP CPU SPI controller config */

static struct omap2_mcspi_device_config mcspi_config = {
	.turbo_mode		= 0,
	.single_channel = 1,
};

static struct spi_board_info slave_spi_board_info = {
	.modalias			= "psoc4",
	.bus_num			= 1,
	.chip_select		= 0,
	.max_speed_hz		= 500000,
	.controller_data	= &mcspi_config,
	.mode				= SPI_MODE_3 //& ~SPI_CS_HIGH, // Clock polarity SPI_CPOL = 1 SPI_CPHA = 1
};

static int hotplug_spi_init(void)
{
	int bus_num;
	struct spi_master *slaves_spi_master;
	
	printk(KERN_ALERT "Adding SPI Device: %s, bus: %i, chip-sel: %i\n", 
		slave_spi_board_info.modalias, slave_spi_board_info.bus_num, slave_spi_board_info.chip_select);
		
	bus_num = slave_spi_board_info.bus_num;
	slaves_spi_master = spi_busnum_to_master(bus_num);
	slave_spi_device = spi_new_device(slaves_spi_master, &slave_spi_board_info);
	
	if(slave_spi_device < 0){
		printk(KERN_ALERT "Unsuccesful creating a new device\n");
		return -1;
	}
	return 0;
}

static void hotplug_spi_exit(void)
{
	printk(KERN_ALERT "Removing SPI Device: %s, bus: %i, chip-sel: %i\n", 
		slave_spi_board_info.modalias, slave_spi_board_info.bus_num, slave_spi_board_info.chip_select);
	spi_unregister_device(slave_spi_device);
}

module_init(hotplug_spi_init);
module_exit(hotplug_spi_exit);