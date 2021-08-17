/*!
    \file  gd25qxx.h
    \brief the header file of SPI flash gd25qxx driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-05-10, V1.0.0, demo for GD32F403Z
*/

#ifndef GD25QXX_H
#define GD25QXX_H

#include "gd32f403.h"

#define  SPI_FLASH_PAGE_SIZE       0x100
#define  SPI_FLASH_CS_LOW()        gpio_bit_reset(GPIOE, GPIO_PIN_3)
#define  SPI_FLASH_CS_HIGH()       gpio_bit_set(GPIOE, GPIO_PIN_3)

/* initialize SPI0 GPIO and parameter */
void spi_flash_init(void);
/* erase the specified flash sector */
void spi_flash_sector_erase(uint32_t sector_addr);
/* erase the entire flash */
void spi_flash_bulk_erase(void);
/* write more than one byte to the flash */
void spi_flash_page_write(uint8_t* pbuffer,uint32_t write_addr,uint16_t num_byte_to_write);
/* write block of data to the flash */
void spi_flash_buffer_write(uint8_t* pbuffer,uint32_t write_addr,uint16_t num_byte_to_write);
/* read a block of data from the flash */
void spi_flash_buffer_read(uint8_t* pbuffer,uint32_t read_addr,uint16_t num_byte_to_read);
/* read flash identification */
uint32_t spi_flash_read_id(void);
/* initiate a read data byte (read) sequence from the flash */
void spi_flash_start_read_sequence(uint32_t read_addr);
/* read a byte from the SPI flash */
uint8_t spi_flash_read_byte(void);
/* send a byte through the SPI interface and return the byte received from the SPI bus */
uint8_t spi_flash_send_byte(uint8_t byte);
/* send a half word through the SPI interface and return the half word received from the SPI bus */
uint16_t spi_flash_send_halfword(uint16_t half_word);
/* enable the write access to the flash */
void spi_flash_write_enable(void);
/* poll the status of the write in progress (wip) flag in the flash's status register */
void spi_flash_wait_for_write_end(void);

#endif /* GD25QXX_H */
