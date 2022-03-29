/*!
    \file    exmc_sdram.h
    \brief   the header file of sdram(MICRON 48LC16M16A2) driver

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2019-04-01, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
    \version 2021-07-30, V2.3.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef EXMC_SDRAM_H
#define EXMC_SDRAM_H

#include "gd32f20x.h"

/* initialize sdram peripheral */
ErrStatus exmc_synchronous_dynamic_ram_init(uint32_t sdram_device);
/* fill the buffer with specified value */
void fill_buffer(uint8_t *pbuffer, uint16_t buffer_lengh, uint16_t offset);
/* write a byte buffer(data is 8 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t writeaddr, uint32_t numbyte_write);
/* read a block of 8-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t readaddr, uint32_t numbyte_read);
/* write a half-byte buffer(data is 16 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_16(uint32_t sdram_device, uint16_t *pbuffer, uint32_t writeaddr, uint32_t numhalfword_write);
/* read a block of 16-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_16(uint32_t sdram_device, uint16_t *pbuffer, uint32_t readaddr, uint32_t numhalfword_read);
/* write a word buffer(data is 32 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_32(uint32_t sdram_device, uint32_t *pbuffer, uint32_t writeaddr, uint32_t numword_write);
/* read a block of 32-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_32(uint32_t sdram_device, uint32_t *pbuffer, uint32_t readaddr, uint32_t numword_read);

#endif  /* EXMC_SDRAM_H */
