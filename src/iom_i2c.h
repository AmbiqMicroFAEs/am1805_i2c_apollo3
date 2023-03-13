//*****************************************************************************
// SIMPLIFIED IOM SPI DRIVER
// Apollo3 Blue
// Mariusz Lacina, Ambiq 2023 
//*****************************************************************************


#ifndef IOM_SPI_H
#define IOM_SPI_H

#include "iom_dma.h"


//*****************************************************************************
//  RTC SLAVE ADDRESS
//*****************************************************************************
#define 	RTC_SLAVE_ADDR				(0xd2 >> 1)


uint32_t iom_i2c_init(void);
uint32_t iom_i2c_write(uint8_t slave_address, uint32_t *pbuf, uint32_t size, bool cont, bool block);
uint32_t iom_i2c_read(uint8_t slave_address, uint32_t *pbuf, uint32_t size, bool cont, bool block);

#endif


