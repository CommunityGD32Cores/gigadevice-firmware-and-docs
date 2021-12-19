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
#define MAX_RELOAD_SIZE          255

uint16_t eeprom_address;

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
void i2c_eeprom_init()
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
    i2c_process_enum state = I2C_START;
    uint16_t timeout = 0;
    uint8_t end_flag = 0;

    while(!end_flag) {
        switch(state) {
        case I2C_START:
            /* configure slave address */
            i2c_master_addressing(I2CX, eeprom_address, I2C_MASTER_TRANSMIT);
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(I2CX, number_of_byte + 1);
            /* clear I2C_TDATA register */
            I2C_STAT(I2CX) |= I2C_STAT_TBE;
            /* enable I2C automatic end mode in master mode */
            i2c_automatic_end_enable(I2CX);
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(I2CX);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                /* timeout, bus reset */
                i2c_bus_reset();
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in page write!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2CX, write_address);
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;
        case I2C_TRANSMIT_DATA:
            while(number_of_byte--) {
                /* wait until TI bit is set */
                while((!i2c_flag_get(I2CX, I2C_FLAG_TI)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* while there is data to be written */
                    i2c_data_transmit(I2CX, *p_buffer);
                    /* point to the next byte to be written */
                    p_buffer++;
                    timeout = 0;
                    state = I2C_STOP;
                } else {
                    /* wait TI timeout */
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master sends data timeout in page write!\n");
                    return ;
                }
            }
            break;
        case I2C_STOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2CX, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* clear STPDET flag */
                i2c_flag_clear(I2CX, I2C_FLAG_STPDET);
                timeout = 0;
                state = I2C_END;
                end_flag = 1;
            } else {
                /* stop detect timeout */
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in page write!\n");
            }
            break;
        default:
            /* default status */
            state = I2C_START;
            end_flag = 1;
            timeout = 0;
            printf("i2c master sends start signal in page write!\n");
            break;
        }
    }
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
    uint32_t nbytes_reload = 0;
    i2c_process_enum state = I2C_START;
    uint32_t timeout = 0;
    uint8_t end_flag = 0;
    uint8_t restart_flag = 0;
    uint8_t first_reload_flag = 1;

    while(!end_flag) {
        switch(state) {
        case I2C_START:
            if(0 == restart_flag) {
                /* clear I2C_TDATA register */
                I2C_STAT(I2CX) |= I2C_STAT_TBE;
                /* configure slave address */
                i2c_master_addressing(I2CX, eeprom_address, I2C_MASTER_TRANSMIT);
                /* configure number of bytes to be transferred */
                i2c_transfer_byte_number_config(I2CX, 1);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(I2CX);
                /* i2c master sends start signal only when the bus is idle */
                while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    i2c_start_on_bus(I2CX);
                    timeout = 0;
                    state = I2C_SEND_ADDRESS;
                } else {
                    /* timeout, bus reset */
                    i2c_bus_reset();
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c bus is busy in read!\n");
                }
            } else {
                /* restart */
                i2c_start_on_bus(I2CX);
                restart_flag = 0;
                state = I2C_TRANSMIT_DATA;
            }
            break;
        case I2C_SEND_ADDRESS:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2CX, read_address);
                timeout = 0;
                state = I2C_RESTART;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends data timeout in read!\n");
            }
            break;
        case I2C_RESTART:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2CX, I2C_FLAG_TC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* configure the EEPROM's internal address to write to : only one byte address */
                i2c_master_addressing(I2CX, eeprom_address, I2C_MASTER_RECEIVE);
                /* enable I2C reload mode */
                i2c_reload_enable(I2CX);
                /* configure number of bytes to be transferred */
                timeout = 0;
                state = I2C_RELOAD;
                restart_flag = 1;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends EEPROM's internal address timeout in read!\n");
            }
            break;
        case I2C_RELOAD:
            if(number_of_byte > MAX_RELOAD_SIZE) {
                number_of_byte = number_of_byte - MAX_RELOAD_SIZE;
                nbytes_reload = MAX_RELOAD_SIZE;
            } else {
                nbytes_reload = number_of_byte;
            }
            if(1 == first_reload_flag) {
                /* configure number of bytes to be transferred */
                i2c_transfer_byte_number_config(I2CX, nbytes_reload);
                first_reload_flag = 0;
                state = I2C_START;
            } else {
                /* wait for TCR flag */
                while((!i2c_flag_get(I2CX, I2C_FLAG_TCR)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* configure number of bytes to be transferred */
                    i2c_transfer_byte_number_config(I2CX, nbytes_reload);
                    /* disable I2C reload mode */
                    if(number_of_byte <= MAX_RELOAD_SIZE) {
                        i2c_reload_disable(I2CX);
                        /* enable I2C automatic end mode in master mode */
                        i2c_automatic_end_enable(I2CX);
                    }
                    timeout = 0;
                    state = I2C_TRANSMIT_DATA;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master reload data timeout in read!\n");
                }
            }
            break;
        case I2C_TRANSMIT_DATA:
            /* wait until TI bit is set */
            while((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                while(nbytes_reload) {
                    /* wait until the RBNE bit is set and clear it */
                    if(i2c_flag_get(I2CX, I2C_FLAG_RBNE)) {
                        /* read a byte from the EEPROM */
                        *p_buffer = i2c_data_receive(I2CX);
                        /* point to the next location where the byte read will be saved */
                        p_buffer++;
                        /* decrement the read bytes counter */
                        nbytes_reload--;
                    }
                }
                timeout = 0;
                /* check if the reload mode is enabled or not */
                if(I2C_CTL1(I2CX) & I2C_CTL1_RELOAD) {
                    timeout = 0;
                    state = I2C_RELOAD;
                } else {
                    timeout = 0;
                    state = I2C_STOP;
                }
            } else {
                /* wait TI timeout */
                timeout = 0;
                state = I2C_START;
                printf("i2c master read data timeout in read!\n");
            }
            break;
        case I2C_STOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2CX, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* clear STPDET flag */
                i2c_flag_clear(I2CX, I2C_FLAG_STPDET);
                timeout = 0;
                state = I2C_END;
                end_flag = 1;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in read!\n");
            }
            break;
        default:
            /* default status */
            state = I2C_START;
            end_flag = 1;
            timeout = 0;
            printf("i2c master sends start signal in read!\n");
            break;
        }
    }
}
