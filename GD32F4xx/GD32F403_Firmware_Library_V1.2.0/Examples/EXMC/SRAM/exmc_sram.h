/*!
    \file  exmc_sram.c
    \brief the header file of EXMC SRAM(ISSI IS61LV51216) driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef EXMC_SRAM_H
#define EXMC_SRAM_H

#include "gd32f403.h"

/* function declarations */
/* SRAM peripheral initialize */
void exmc_sram_init(void);
/* write a Half-word buffer(data is 16 bits) to the EXMC SRAM memory */
void exmc_sram_writebuffer_16(uint16_t* pbuffer, uint32_t writeaddr, uint32_t num_halfword_write);
/* read a block of 16-bit data from the EXMC SRAM memory */
void exmc_sram_readbuffer_16(uint16_t* pbuffer, uint32_t readaddr, uint32_t num_halfword_read);
/* write a Byte buffer(data is 8 bits ) to the EXMC SRAM memory */
void exmc_sram_writebuffer_8(uint8_t* pbuffer, uint32_t writeaddr, uint32_t num_byte_write);
/* read a block of 8-bit data from the EXMC SRAM memory */
void exmc_sram_readbuffer_8(uint8_t* pbuffer, uint32_t readaddr, uint32_t num_byte_read);
/* write a word buffer(data is 32 bits) to the EXMC SRAM memory */
void exmc_sram_writebuffer_32(uint32_t* pbuffer, uint32_t writeaddr, uint32_t num_word_write);
/* read a block of 32-bit data from the EXMC SRAM memory */
void exmc_sram_readbuffer_32(uint32_t* pbuffer, uint32_t readaddr, uint32_t num_word_read);
/* fill the 16-bit buffer with specified value */
void fill_buffer_16(uint16_t *pbuffer, uint16_t buffer_lenght, uint16_t offset);

#endif /* EXMC_SRAM_H */
