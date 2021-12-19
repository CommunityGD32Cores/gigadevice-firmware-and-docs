/*!
    \file    at24cxx.h
    \brief   the header file of AT24Cxx

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef AT24CXX_H
#define AT24CXX_H

#include "gd32l23x_it.h"

typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_RESTART,
    I2C_TRANSMIT_DATA,
    I2C_RELOAD,
    I2C_STOP,
    I2C_END
} i2c_process_enum;

#define I2C_TIME_OUT   (uint32_t)(20000)
#define EEP_FIRST_PAGE 0x00
#define I2C_OK         0
#define I2C_FAIL       1

/* function declarations */
/* I2C read and write functions */
uint8_t i2c_24c02_test(void);
/* initialize peripherals used by the I2C EEPROM driver */
void i2c_eeprom_init(void);
/* write buffer of data to the I2C EEPROM */
void eeprom_buffer_write(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte);
/* write more than one byte to the EEPROM with a single write cycle */
void eeprom_page_write(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte);
/* read data from the EEPROM */
void eeprom_buffer_read(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte);

#endif /* AT24CXX_H */
