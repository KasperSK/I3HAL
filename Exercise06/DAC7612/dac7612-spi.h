#ifndef DAC7612_SPI_H
#define DAC7612_SPI_H
#include <linux/spi/spi.h>

struct spi_device* dac7612_get_device(void);
int dac7612_spi_write_reg16(struct spi_device *spi, u16 addr, u16 data);
int dac7612_spi_init(void);
void dac7612_spi_exit(void);

#endif

