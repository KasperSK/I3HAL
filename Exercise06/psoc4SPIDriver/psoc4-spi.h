#ifndef PSOC4_SPI_H
#define PSOC4_SPI_H
#include <linux/spi/spi.h>

struct spi_device* psoc4_get_device(void);
//int psoc4_spi_read_reg8(struct spi_device *spi, u8 addr, u8* data);
int psoc4_spi_write_reg8(struct spi_device *spi, u8 addr, u8 data);
int psoc4_spi_read_reg16(struct spi_device *spi, u8 addr, u16* data);
//int psoc4_spi_write_reg16(struct spi_device *spi, u8 addr, u16 data);
int psoc4_spi_init(void);
void psoc4_spi_exit(void);

#endif