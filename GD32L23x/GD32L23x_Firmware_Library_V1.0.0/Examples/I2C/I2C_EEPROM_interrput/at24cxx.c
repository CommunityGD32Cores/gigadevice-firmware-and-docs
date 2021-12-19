/*!
    \file    at24cxx.c
    \brief   the read and write function file

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

#include "at24cxx.h"
#include "i2c.h"
#include <stdio.h>

#define EEPROM_BLOCK0_ADDRESS    0xA0
#define BUFFER_SIZE              256

volatile uint16_t eeprom_address;
volatile uint16_t address_byte_flag = 0;
volatile uint8_t gaddress;
volatile uint16_t tx_nBytes, rx_nBytes;
volatile uint8_t *pwbuffer, *prbuffer;
volatile uint8_t i2c_buffer_write[BUFFER_SIZE];
volatile uint8_t i2c_buffer_read[BUFFER_SIZE];
volatile uint32_t nbytes_read = 0;
volatile ErrStatus status = ERROR;

/*!
    \brief      I2C read and write functions
    \param[in]  none
    \param[out] none
    \retval     I2C_OK or I2C_FAIL
*/
uint8_t i2c_24c02_test(void)
{
    uint16_t i;
    uint8_t i2c_buffer_write[BUFFER_SIZE];
    uint8_t i2c_buffer_read[BUFFER_SIZE];

    printf("\r\nAT24C02 writing...\r\n");
    /* initialize i2c_buffer_write */
    for(i = 0; i < BUFFER_SIZE; i++) {
        i2c_buffer_write[i] = i;
        printf("0x%02X ", i2c_buffer_write[i]);
        if(15 == i % 16) {
            printf("\r\n");
        }
    }
    /* EEPROM data write */
    eeprom_buffer_write(i2c_buffer_write, EEP_FIRST_PAGE, BUFFER_SIZE);
    printf("AT24C02 reading...\r\n");
    /* EEPROM data read */
    eeprom_buffer_read(i2c_buffer_read, EEP_FIRST_PAGE, BUFFER_SIZE);
    /* compare the read buffer and write buffer */
    for(i = 0; i < BUFFER_SIZE; i++) {
        if(i2c_buffer_read[i] != i2c_buffer_write[i]) {
            printf("0x%02X ", i2c_buffer_read[i]);
            printf("Err:data read and write aren't matching.\n\r");
            return I2C_FAIL;
        }
        printf("0x%02X ", i2c_buffer_read[i]);
        if(15 == i % 16) {
            printf("\r\n");
        }
    }
    printf("I2C-AT24C02 test passed!\n\r");

    return I2C_OK;
}

/*!
    \brief      initialize peripherals used by the I2C EEPROM driver
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_eeprom_init(void)
{
    eeprom_address = EEPROM_BLOCK0_ADDRESS;
}

/*!
    \brief      write buffer of data to the I2C EEPROM
    \param[in]  p_buffer: pointer to the buffer  containing the data to be written to the EEPROM
    \param[in]  write_address: EEPROM's internal address to write to
    \param[in]  number_of_byte: number of bytes to write to the EEPROM
    \param[out] none
    \retval     none
*/
void eeprom_buffer_write(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte)
{
    uint8_t number_of_page = 0, number_of_single = 0, address = 0, count = 0;

    address = write_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page =  number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;

    /* if write_address is I2C_PAGE_SIZE aligned */
    if(0 == address) {
        while(number_of_page--) {
            eeprom_page_write(p_buffer, write_address, I2C_PAGE_SIZE);
            delay_1ms(5);
            write_address +=  I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }
        if(0 != number_of_single) {
            eeprom_page_write(p_buffer, write_address, number_of_single);
            delay_1ms(5);
        }
    } else {
        /* if write_address is not I2C_PAGE_SIZE aligned */
        if(number_of_byte < count) {
            eeprom_page_write(p_buffer, write_address, number_of_byte);
            delay_1ms(5);
        } else {
            number_of_byte -= count;
            number_of_page =  number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;
            if(0 != count) {
                eeprom_page_write(p_buffer, write_address, count);
                delay_1ms(5);
                write_address += count;
                p_buffer += count;
            }
            /* write page */
            while(number_of_page--) {
                eeprom_page_write(p_buffer, write_address, I2C_PAGE_SIZE);
                delay_1ms(5);
                write_address +=  I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }
            /* write single */
            if(0 != number_of_single) {
                eeprom_page_write(p_buffer, write_address, number_of_single);
                delay_1ms(5);
            }
        }
    }
}

/*!
    \brief      write more than one byte to the EEPROM with a single write cycle
    \param[in]  p_buffer: pointer to the buffer containing the data to be written to the EEPROM
    \param[in]  write_address: EEPROM's internal address to write to
    \param[in]  number_of_byte: number of bytes to write to the EEPROM
    \param[out] none
    \retval     none
*/
void eeprom_page_write(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte)
{
    uint8_t tx_buffer[9] = {0};

    tx_buffer[0] = write_address;
    for(int i = 1; i < number_of_byte + 1; i++) {
        tx_buffer[i] = *p_buffer;
        p_buffer++;
    }
    /* configure slave address */
    i2c_master_addressing(I2CX, eeprom_address, I2C_MASTER_TRANSMIT);
    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(I2CX, number_of_byte + 1);
    tx_nBytes = number_of_byte + 1;
    pwbuffer = tx_buffer;
    /* enable I2C automatic end mode in master mode */
    i2c_automatic_end_enable(I2CX);
    /* clear I2C_TDATA register */
    I2C_STAT(I2CX) |= I2C_STAT_TBE;
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2CX);
    while(tx_nBytes > 0);
}

/*!
    \brief      read data from the EEPROM
    \param[in]  p_buffer: pointer to the buffer that receives the data read from the EEPROM
    \param[in]  read_address: EEPROM's internal address to start reading from
    \param[in]  number_of_byte: number of bytes to reads from the EEPROM
    \param[out] none
    \retval     none
*/
void eeprom_buffer_read(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte)
{
    /* clear I2C_TDATA register */
    I2C_STAT(I2CX) |= I2C_STAT_TBE;
    /* configure slave address */
    i2c_master_addressing(I2CX, eeprom_address, I2C_MASTER_TRANSMIT);
    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(I2CX, 1);
    address_byte_flag = 1;
    gaddress = read_address;
    prbuffer = p_buffer;
    nbytes_read = number_of_byte;
    /* disable I2C automatic end mode in master mode */
    i2c_automatic_end_disable(I2CX);
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2CX);
    while(SUCCESS != status);
    /* disable the I2CX interrupt */
    i2c_interrupt_disable(I2CX, I2C_INT_ERR | I2C_INT_STPDET | I2C_INT_RBNE | I2C_INT_TC | I2C_INT_TI);
}
