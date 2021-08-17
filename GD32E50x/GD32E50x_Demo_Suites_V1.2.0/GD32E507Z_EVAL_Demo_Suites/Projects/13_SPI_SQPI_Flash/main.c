/*!
    \file    main.c
    \brief   SQPI SPI-FLASH example

    \version 2020-03-10, V1.0.0, demo for GD32E50x
    \version 2020-08-26, V1.1.0, demo for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
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
#include "sqpi_flash.h"

#define BUFFERSIZE  4096
#define SFLASH_ID   0xC8401500
#define BUFFER_SIZE              256
#define TX_BUFFER_SIZE           (countof(tx_buffer) - 1)
#define RX_BUFFER_SIZE           0xFF
#define FLASH_WRITE_ADDRESS      0x000000
#define FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
uint8_t tx_buffer[256];
uint8_t rx_buffer[256];
uint32_t flash_id=0;
uint8_t  is_successful = 0;
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length); 
void sqpi_gpio_config(void);
void sqpi_config(void);

int main(void)
{   
    uint32_t i=0;
    /* configure systick */
    systick_config();
    /* initialize SQPI gpio */
    sqpi_gpio_config();
    gd_eval_com_init(EVAL_COM0);
    /* read SPI flash ID */
    sqpi_flash_init();
    flash_id = sqpi_flash_read_id();
    if (flash_id != SFLASH_ID){
        while(1);
    }
    printf("\n\rThe Flash_ID:0x%X\n\r\n\r",flash_id);
    /* enable flash quad operation */
    sqpi_flash_quad_enable();    
    /* GPIO configure for quad SPI mode */
    gpio_init(GPIOF,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2|GPIO_PIN_10);

    /* flash id is correct */
    if(SFLASH_ID == flash_id){
        printf("\n\rWrite to tx_buffer:\n\r\n\r");

        /* printf tx_buffer value */
        for(i = 0; i < BUFFER_SIZE; i++){
            tx_buffer[i] = i;
            printf("0x%02X ",tx_buffer[i]);

            if(15 == i%16)
                printf("\n\r");
        }

        printf("\n\r\n\rRead from rx_buffer:\n\r\n\r");

        /* erase the specified flash sector */
        sqpi_flash_sector_erase(FLASH_WRITE_ADDRESS);

        /* write tx_buffer data to the flash */ 
        sqpi_flash_buffer_write(FLASH_WRITE_ADDRESS,tx_buffer,256);

        delay_1ms(10);
        /* read a block of data from the flash to rx_buffer */
        sqpi_flash_buffer_read(FLASH_READ_ADDRESS,rx_buffer,256);

        /* printf rx_buffer value */
        for(i = 0; i < BUFFER_SIZE; i ++){
            printf("0x%02X ", rx_buffer[i]);
            if(15 == i%16)
                printf("\n\r");
        }

        if(ERROR == memory_compare(tx_buffer,rx_buffer,256)){
            printf("\n\rErr:Data Read and Write aren't Matching.\n\r");
            is_successful = 1;
        }

        /* spi qspi flash test passed */
        if(0 == is_successful){
            printf("\n\rSPI-GD25Q16 Test Passed!\n\r");
        }
    }
        else{
        /* spi flash read id fail */
        printf("\n\rSPI Flash: Read ID Fail!\n\r");
    }

    while(1);
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
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOF,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_8);
    /* configure for standard SPI mode */
    gpio_init(GPIOF,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2|GPIO_PIN_10);
    gpio_bit_set(GPIOF,GPIO_PIN_10);
    gpio_bit_reset(GPIOF,GPIO_PIN_2);
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

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}