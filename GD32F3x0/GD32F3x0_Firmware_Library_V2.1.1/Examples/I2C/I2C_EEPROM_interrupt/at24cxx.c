/*!
    \file    at24cxx.c
    \brief   the read and write function file

    \version 2020-09-30, V1.0.0, firmware for GD32F3x0
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

#include "at24cxx.h"
#include "i2c.h"
#include <stdio.h>

#define EEPROM_BLOCK0_ADDRESS    0xA0
#define BUFFER_SIZE              256

uint16_t eeprom_address;
uint8_t i2c_buffer_write[BUFFER_SIZE];
uint8_t i2c_buffer_read[BUFFER_SIZE];

volatile uint8_t* i2c_write;
volatile uint8_t* i2c_read;
volatile uint16_t i2c_nbytes;
volatile uint16_t i2c_write_dress;
volatile uint16_t i2c_read_dress;
volatile uint8_t  i2c_process_flag = 0;

/*!
    \brief      I2C read and write functions
    \param[in]  none
    \param[out] none
    \retval     I2C_OK or I2C_FAIL 
*/
uint8_t i2c_24c02_test(void)
{
    uint16_t i;

    printf("\r\nAT24C02 writing...\r\n");

    /* initialize i2c_buffer_write */
    for(i = 0; i < BUFFER_SIZE; i++){
        i2c_buffer_write[i] = i;
        printf("0x%02X ", i2c_buffer_write[i]);
        if(15 == i%16){
            printf("\r\n");
        }
    }
    /* EEPROM data write */
    eeprom_buffer_write(i2c_buffer_write, EEP_FIRST_PAGE, BUFFER_SIZE);
    printf("AT24C02 reading...\r\n");
    /* EEPROM data read */
    eeprom_buffer_read_intrrupt(i2c_buffer_read, EEP_FIRST_PAGE, BUFFER_SIZE);
    /* compare the read buffer and write buffer */
    for(i = 0; i < BUFFER_SIZE; i++){
        if(i2c_buffer_read[i] != i2c_buffer_write[i]){
            printf("0x%02X ", i2c_buffer_read[i]);
            printf("Err:data read and write aren't matching.\n\r");
            return I2C_FAIL;
        }
        printf("0x%02X ", i2c_buffer_read[i]);
        if(15 == i%16){
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
void eeprom_buffer_write(uint8_t* p_buffer, uint8_t write_address, uint16_t number_of_byte)
{
    uint8_t number_of_page = 0, number_of_single = 0, address = 0, count = 0;

    address = write_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page =  number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;

    i2c_process_flag = 0;

    /* if write_address is I2C_PAGE_SIZE aligned  */
    if(0 == address){
        while(number_of_page--){
            i2c_nbytes = I2C_PAGE_SIZE;
            i2c_write_dress = write_address;
            i2c_write = p_buffer;
            eeprom_page_write_intrrupt();
            eeprom_wait_standby_state();
            write_address += I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }

        if(0 != number_of_single){
            i2c_nbytes = number_of_single;
            i2c_write_dress = write_address;
            i2c_write = p_buffer;
            eeprom_page_write_intrrupt();
            eeprom_wait_standby_state();
        }
    }else{
        /* if write_address is not I2C_PAGE_SIZE aligned */
        if(number_of_byte < count){
            i2c_nbytes = number_of_byte;
            i2c_write_dress = write_address;
            i2c_write = p_buffer;
            eeprom_page_write_intrrupt();
            eeprom_wait_standby_state();
        }else{
            number_of_byte -= count;
            number_of_page = number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;

            if(0 != count){
                i2c_nbytes = count;
                i2c_write_dress = write_address;
                i2c_write = p_buffer;
                eeprom_page_write_intrrupt();
                eeprom_wait_standby_state();

                write_address += count;
                p_buffer += count;
            }

            /* write page */
            while(number_of_page--){
                i2c_nbytes = number_of_page;
                i2c_write_dress = write_address;
                i2c_write = p_buffer;
                eeprom_page_write_intrrupt();
                eeprom_wait_standby_state();

                write_address += I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;;
            }

            /* write single */
            if(0 != number_of_single){
                i2c_nbytes = number_of_page;
                i2c_write_dress = write_address;
                i2c_write = p_buffer;
                eeprom_page_write_intrrupt();
                eeprom_wait_standby_state();
            }
        }
    }

    i2c_process_flag = SET;
}

/*!
    \brief      write more than one byte to the EEPROM by interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void eeprom_page_write_intrrupt(void)
{
    /* enable the I2C interrupt */
    i2c_interrupt_enable(BOARD_I2C, I2C_INT_ERR);
    i2c_interrupt_enable(BOARD_I2C, I2C_INT_EV);
    i2c_interrupt_enable(BOARD_I2C, I2C_INT_BUF);

    /* the master waits until the I2C bus is idle */
    while(i2c_flag_get(BOARD_I2C, I2C_FLAG_I2CBSY));

    /* the master sends a start condition to I2C bus */
    i2c_start_on_bus(BOARD_I2C);
    while((i2c_nbytes > 0));
}

/*!
    \brief      read data from the EEPROM by interrupt
    \param[in]  p_buffer: pointer to the buffer that receives the data read from the EEPROM
    \param[in]  read_address: EEPROM's internal address to start reading from
    \param[in]  number_of_byte: number of bytes to reads from the EEPROM
    \param[out] none
    \retval     none
*/
void eeprom_buffer_read_intrrupt(uint8_t* p_buffer, uint8_t read_address, uint16_t number_of_byte)
{
    i2c_read=p_buffer;
    i2c_read_dress=read_address;
    i2c_nbytes=number_of_byte;

    /* enable the I2C interrupt */
    i2c_interrupt_enable(BOARD_I2C, I2C_INT_ERR);
    i2c_interrupt_enable(BOARD_I2C, I2C_INT_EV);
    i2c_interrupt_enable(BOARD_I2C, I2C_INT_BUF);

    /* wait until I2C bus is idle */
    while(i2c_flag_get(BOARD_I2C, I2C_FLAG_I2CBSY));
    if(2 == number_of_byte){
        i2c_ackpos_config(BOARD_I2C, I2C_ACKPOS_NEXT);
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(BOARD_I2C);
    while((i2c_nbytes > 0));
}

/*!
    \brief      wait for EEPROM standby state
    \param[in]  none
    \param[out] none
    \retval     none
*/
void eeprom_wait_standby_state(void)
{
    __IO uint32_t val = 0;

    while(1){
        /* wait until I2C bus is idle */
        while(i2c_flag_get(BOARD_I2C, I2C_FLAG_I2CBSY));

        /* send a start condition to I2C bus */
        i2c_start_on_bus(BOARD_I2C);

        /* wait until SBSEND bit is set */
        while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_SBSEND));

        /* send slave address to I2C bus */
        i2c_master_addressing(BOARD_I2C, eeprom_address, I2C_TRANSMITTER);

        /* keep looping till the Address is acknowledged or the AE flag is set (address not acknowledged at time) */
        do{
            /* get the current value of the I2C_STAT0 register */
            val = I2C_STAT0(BOARD_I2C);
        }while(0 == (val & (I2C_STAT0_ADDSEND | I2C_STAT0_AERR)));

        /* check if the ADDSEND flag has been set */
        if(val & I2C_STAT0_ADDSEND){
            /* clear ADDSEND flag */
            i2c_flag_clear(BOARD_I2C, I2C_FLAG_ADDSEND);

            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(BOARD_I2C);

            /* exit the function */
            return ;
        }else{
            /* clear the bit of AE */
            i2c_flag_clear(BOARD_I2C, I2C_FLAG_AERR);
        }

        /* send a stop condition to I2C bus */
        i2c_stop_on_bus(BOARD_I2C);

        /* wait until the stop condition is finished */
        while(I2C_CTL0(BOARD_I2C) & 0x0200);
    }
}
