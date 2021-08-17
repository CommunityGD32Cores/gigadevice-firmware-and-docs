/*!
    \file  gd32f207i_sdram_eval.h
    \brief the header file of sdram(MICRON 48LC16M16A2) driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef GD32F20X_SDRAM_EVAL_H
#define GD32F20X_SDRAM_EVAL_H

#include "gd32f20x.h"

/* sdram peripheral initialize */
void exmc_synchronous_dynamic_ram_init(uint32_t sdram_device);

/* fill the buffer with specified value */
void fill_buffer(uint8_t *pbuffer, uint16_t buffer_lengh, uint16_t offset);

/* write a byte buffer(data is 8 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_8(uint32_t sdram_device, uint8_t* pbuffer, uint32_t writeaddr, uint32_t numbyte_write);

/* read a block of 8-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_8(uint32_t sdram_device, uint8_t* pbuffer, uint32_t readaddr, uint32_t numbyte_read);

/* write a half-byte buffer(data is 16 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_16(uint32_t sdram_device, uint16_t* pbuffer, uint32_t writeaddr, uint32_t numhalfbyte_write);
/* read a block of 16-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_16(uint32_t sdram_device, uint16_t* pbuffer, uint32_t readaddr, uint32_t numhalfbyte_read);
/* write a word buffer(data is 32 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_32(uint32_t sdram_device, uint32_t* pbuffer, uint32_t writeaddr, uint32_t numword_write);
/* read a block of 32-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_32(uint32_t sdram_device, uint32_t* pbuffer, uint32_t readaddr, uint32_t numword_read);

#endif  /* GD32F20X_SDRAM_EVAL_H */
