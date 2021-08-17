/*!
    \file    main.c
    \brief   SQPI psram example

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include "systick.h"
#include <stdio.h>
#include "gd32e507z_eval.h"

/* PSRAM commands */
#define PSRAM_READ_ID              0x9F
#define PSRAM_RESET_ENABLE         0x66
#define PSRAM_RESET                0x99
#define PSRAM_READ                 0x03
#define PSRAM_FAST_READ            0x0B
#define PSRAM_WRITE                0x02
#define PSRAM_QUAD_WRITE           0x38
#define PSRAM_ENTER_QUAD           0x35
#define PSRAM_EXIT_QUAD            0xF5

/* SQPI logic address */
#define SQPI_LOGIC_ADDR  ((uint32_t)0xB0000000)

uint8_t rxbuffer[256];
uint8_t txbuffer[256]={
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
#define BUFFERSIZE 256

ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length); 
void sqpi_reset(void);
void sqpi_gpio_config(void);
void sqpi_config(void);
uint32_t sqpi_psram_read_id(void);
void sqpi_psram_buffer_read(uint32_t read_addr,uint8_t * read_buff,uint32_t size);
void sqpi_psram_buffer_write(uint32_t write_addr,uint8_t * write_buff,uint32_t size);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{   
    __IO uint32_t i=0;
    /* configure systick */
    systick_config();
    /* configure USART */
    gd_eval_com_init(EVAL_COM0);
    /* initilize SQPI gpio */
    sqpi_gpio_config();
    /* configure SQPI parameter */
    sqpi_config();
    /* reset PSRAM */
    sqpi_reset();
    /* read psram ID */
    if(sqpi_psram_read_id() != 0x0D5D0000){
        while(1);
    }

    /* write tx_buffer data to the PARAM */ 
    sqpi_psram_buffer_write(0x00000100,txbuffer,BUFFERSIZE);
    /* read a block of data from the PSRAM to rx_buffer */
    sqpi_psram_buffer_read(0x00000100,rxbuffer,BUFFERSIZE);
    
    if(memory_compare(txbuffer, rxbuffer, BUFFERSIZE) != ERROR){
        printf("\n SQPI read and write operation success");
    }else{
        printf("\n SQPI read and write operation error");
    }

    while(1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}

/*!
    \brief      SQPI PSARAM reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_reset(void)
{
      sqpi_write_command_config(SQPI_MODE_SSS,0,PSRAM_RESET_ENABLE);
      sqpi_special_command();
      sqpi_write_command_config(SQPI_MODE_SSS,0,PSRAM_RESET);
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
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_SQPI);

    gpio_init(GPIOF,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_10);
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

    /* SQPI parameter config */
    sqpi_para.addr_bit = 24;
    sqpi_para.clk_div = 23;
    sqpi_para.cmd_bit = SQPI_CMDBIT_8_BITS;
    sqpi_para.id_length = SQPI_ID_LENGTH_64_BITS;
    sqpi_para.polarity = SQPI_SAMPLE_POLARITY_FALLING;

    sqpi_init( &sqpi_para);
}

/*!
    \brief      read PSRAM identification
    \param[in]  none
    \param[out] none
    \retval     PARAM identification
*/
uint32_t sqpi_psram_read_id(void)
{
    uint32_t temp= 0;

    sqpi_read_command_config(SQPI_MODE_SSS,0,PSRAM_READ_ID);
    /* Read ID command provides information of vendor ID, known-good-die, device density, and manufacturing
       ID. It can be executed under the following 2 conditions:
       1. Run it as the first operation after power up and before any other commands being executed.
       2. Issue a memory read command with address = 0xh00000 first, then Read ID command.
       3. Issue a pre-condition of dummy read id, then Read ID command. */
    sqpi_read_id_command();
    sqpi_read_id_command();
    temp = sqpi_low_id_receive();    
    temp = sqpi_high_id_receive();
    return ( temp & 0xFFFF0000);
}

/*!
    \brief      read a block of data from the PSRAM
    \param[in]  read_addr: PARAM's internal address to read from
    \param[in]  read_buff: pointer to the buffer that receives the data read from the PSRAM
    \param[in]  size: number of bytes to read from the PSRAM
    \param[out] none
    \retval     none
*/
void sqpi_psram_buffer_read(uint32_t read_addr,uint8_t * read_buff,uint32_t size)
{
    uint32_t i =0;
    /* Enter QPI mode*/
    sqpi_write_command_config(SQPI_MODE_SSS,0,PSRAM_ENTER_QUAD);
    sqpi_special_command();

    sqpi_read_command_config(SQPI_MODE_QQQ,4,PSRAM_FAST_READ);

    for(i=0;i<size;i++){
        /* read a byte from the PSRAM */
        *read_buff =*(uint8_t *)(SQPI_LOGIC_ADDR+read_addr+i);
        /* point to the next location where the byte read will be saved */
        read_buff++;
    }

    /* Exit QPI mode*/
    sqpi_write_command_config(SQPI_MODE_QQQ,0,PSRAM_EXIT_QUAD);
    sqpi_special_command();
}

/*!
    \brief      write a block of data to the PSRAM
    \param[in]  write_addr: PARAM's internal address to write
    \param[in]  write_buff: pointer to the buffer that transmit the data write to the PSRAM
    \param[in]  size: number of bytes to write to the PSRAM
    \param[out] none
    \retval     none
*/
void sqpi_psram_buffer_write(uint32_t write_addr,uint8_t * write_buff,uint32_t size)
{
    uint32_t i =0;
    /* Enter QPI mode*/
    sqpi_write_command_config(SQPI_MODE_SSS,0,PSRAM_ENTER_QUAD);
    sqpi_special_command();

    sqpi_write_command_config(SQPI_MODE_QQQ,0,PSRAM_QUAD_WRITE);
    for(i=0;i<size;i++){
        /* write a byte to the PSRAM */
        *(uint8_t *)(SQPI_LOGIC_ADDR+write_addr+i) = *write_buff;
        write_buff++;
    }

    /* Exit QPI mode*/
    sqpi_write_command_config(SQPI_MODE_QQQ,0,PSRAM_EXIT_QUAD);
    sqpi_special_command();
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