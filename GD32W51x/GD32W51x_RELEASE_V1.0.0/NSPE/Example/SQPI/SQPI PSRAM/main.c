/*!
    \file  main.c
    \brief SQPI psram example
    
    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#include "gd32w51x.h"
#include <stdio.h>

#define BUFFERSIZE 256
uint32_t device_id[2];
uint8_t rxbuffer[BUFFERSIZE];
uint8_t txbuffer[BUFFERSIZE]={
0x4F,0x08,0x10,0x70,0x47,0x51,0xC6,0x54,0x1A,0x68,0x95,0xBE,0xFC,0x7C,0x53,0x9E,
0x67,0xD0,0x66,0x8B,0x3D,0x79,0x01,0xB2,0x7E,0xE7,0x86,0x6D,0xEE,0xD9,0x7C,0x50,
0x61,0xAA,0x22,0x68,0x5E,0x39,0x7C,0x8C,0x69,0x7C,0x27,0x1F,0x14,0x77,0x2B,0x83,
0xED,0x7A,0xD1,0xE5,0x46,0xF4,0x00,0x7E,0x81,0x2A,0x4F,0xD4,0x75,0x51,0xF8,0xB1,
0x59,0x23,0x59,0x5A,0x63,0x18,0x79,0xA6,0xC9,0x28,0x87,0x40,0x69,0x62,0xDF,0x00,
0xA0,0x3E,0x88,0x8F,0x91,0x6E,0x9C,0x28,0xFB,0xA7,0x51,0xCC,0xCF,0x83,0x58,0xE6,
0x28,0x90,0x47,0xBF,0x86,0x97,0x5E,0x4C,0xAE,0x9E,0xC3,0xC4,0xD6,0xB4,0x1C,0x5B,
0x85,0xE4,0x15,0x43,0x5D,0x6A,0x05,0x73,0xDA,0xD2,0xE3,0x8E,0xB6,0xA2,0x33,0x7F,
0x4F,0x08,0x10,0x70,0x47,0x51,0xC6,0x54,0x1A,0x68,0x95,0xBE,0xFC,0x7C,0x53,0x9E,
0x67,0xD0,0x66,0x8B,0x3D,0x79,0x01,0xB2,0x7E,0xE7,0x86,0x6D,0xEE,0xD9,0x7C,0x50,
0x61,0xAA,0x22,0x68,0x5E,0x39,0x7C,0x8C,0x69,0x7C,0x27,0x1F,0x14,0x77,0x2B,0x83,
0xED,0x7A,0xD1,0xE5,0x46,0xF4,0x00,0x7E,0x81,0x2A,0x4F,0xD4,0x75,0x51,0xF8,0xB1,
0x59,0x23,0x59,0x5A,0x63,0x18,0x79,0xA6,0xC9,0x28,0x87,0x40,0x69,0x62,0xDF,0x00,
0xA0,0x3E,0x88,0x8F,0x91,0x6E,0x9C,0x28,0xFB,0xA7,0x51,0xCC,0xCF,0x83,0x58,0xE6,
0x28,0x90,0x47,0xBF,0x86,0x97,0x5E,0x4C,0xAE,0x9E,0xC3,0xC4,0xD6,0xB4,0x1C,0x5B,
0x85,0xE4,0x15,0x43,0x5D,0x6A,0x05,0x73,0xDA,0xD2,0xE3,0x8E,0xB6,0xA2,0x33,0x7F,     
};


ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length); 
void sqpi_reset(void);
void sqpi_gpio_config(void);
void sqpi_config(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* initilize SQPI */
    sqpi_gpio_config();
    sqpi_config();
    sqpi_reset();

    /* Read ID command provides information of vendor ID, known-good-die, device density, and manufacturing
       ID. It can be executed under the following 2 conditions:
       1. Run it as the first operation after power up and before any other commands being executed.
       2. Issue a memory read command with address = 0x00000 first, then Read ID command or Issue a pre-condition of dummy read id, then Read ID command. */

    /* Read ID command : 0x9F*/
    sqpi_read_command_config(SQPI_MODE_SSS, 0, 0x9F);
    sqpi_read_id_command();
    sqpi_read_id_command();
    device_id[0] = sqpi_low_id_receive();
    device_id[1] = sqpi_high_id_receive();
    if ((device_id[1]&0xFFFF0000) != 0x0D5D0000){
        while(1);
    }
    
    /* Enter Quad mode : 0x35*/
    sqpi_write_command_config(SQPI_MODE_SSS, 0, 0x35);
    sqpi_special_command();
          
    /* QUAD Write command : 0x38*/
    sqpi_write_command_config(SQPI_MODE_QQQ,0,0x38);
    for(uint32_t i = 0;i < BUFFERSIZE;i++)
    {
        *(uint8_t *)(SQPI_LOGIC_ADDR + i) = txbuffer[i];
    }
    
    /* QUAD Read command : 0xEB*/
    sqpi_read_command_config(SQPI_MODE_QQQ, 6, 0xEB);
    for(uint32_t i = 0;i < BUFFERSIZE;i++)
    {
        rxbuffer[i]=*(uint8_t *)(SQPI_LOGIC_ADDR+i);
    }
    
    /* Exit Quad mode : 0x35*/
    sqpi_write_command_config(SQPI_MODE_QQQ, 0, 0xF5);
    sqpi_special_command();

    /* check */
    memory_compare(txbuffer, rxbuffer, BUFFERSIZE);

    while(1){
    }
}

/*!
    \brief      SQPI PSARAM reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_reset(void)
{
    sqpi_write_command_config(SQPI_MODE_QQQ, 0, 0x66);
    sqpi_special_command();
    sqpi_write_command_config(SQPI_MODE_QQQ, 0, 0x99);
    sqpi_special_command();

    sqpi_write_command_config(SQPI_MODE_SSS, 0, 0x66);
    sqpi_special_command();
    sqpi_write_command_config(SQPI_MODE_SSS, 0, 0x99);
    sqpi_special_command();
}

/*!
    \brief      SQPI GPIO configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_25MHZ,GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_3|GPIO_PIN_4);
    gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_25MHZ,GPIO_PIN_3|GPIO_PIN_4);
    
    gpio_af_set(GPIOA,GPIO_AF_3,GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
    gpio_af_set(GPIOA,GPIO_AF_8,GPIO_PIN_12);
    gpio_af_set(GPIOB,GPIO_AF_8,GPIO_PIN_3|GPIO_PIN_4);
}

/*!
    \brief      SQPI parameter configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_config(void)
{
    sqpi_parameter_struct  sqpi_para;

    rcu_periph_clock_enable(RCU_SQPI);
    /* SQPI parameter config */
    sqpi_para.addr_bit = 24;
    sqpi_para.clk_div = 4;
    sqpi_para.cmd_bit = SQPI_CMDBIT_8_BITS;
    sqpi_para.id_length = SQPI_ID_LENGTH_64_BITS;
    sqpi_para.polarity = SQPI_SAMPLE_POLARITY_FALLING;

    sqpi_init( &sqpi_para);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while (length--){
        if (*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}

