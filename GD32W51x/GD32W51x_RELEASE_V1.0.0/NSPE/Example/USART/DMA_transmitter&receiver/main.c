/*!
    \file    main.c
    \brief   USART DMA transmitter receiver
    
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

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define USART2_TDATA_ADDRESS     (&USART_TDATA(USART2))  
#define USART2_RDATA_ADDRESS     (&USART_RDATA(USART2))  

uint8_t rxbuffer[10];
uint8_t txbuffer[]="\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";

void com_usart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* initialize the COM */
    com_usart_init();

    printf("\n\ra usart dma function test example!\n\r");

    /* deinitialize DMA0 channel3 */
    dma_deinit(DMA0, DMA_CH3);
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_init_struct.periph_addr = (uint32_t)USART2_TDATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory0_addr = (uint32_t)txbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.number = ARRAYNUM(txbuffer);
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH3, &dma_init_struct);
    
    /* DMA channel peripheral select */
    dma_channel_subperipheral_select(DMA0, DMA_CH3, DMA_SUBPERI4);
    
    /* enable DMA0 channel3 */
    dma_channel_enable(DMA0, DMA_CH3);
    /* USART DMA enable for transmission */
    usart_dma_enable(USART2, USART_DMA_TRANSMIT);
    
    /* wait DMA0 Channe3 transfer complete */
    while(RESET == dma_flag_get(DMA0, DMA_CH3, DMA_FLAG_FTF));
    
    while(1){
        /* deinitialize DMA0 channel1 */
        dma_deinit(DMA0, DMA_CH1);
        dma_single_data_para_struct_init(&dma_init_struct);
        dma_init_struct.periph_addr = (uint32_t)USART2_RDATA_ADDRESS;
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory0_addr = (uint32_t)rxbuffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
        dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
        dma_init_struct.number = 10;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_single_data_mode_init(DMA0, DMA_CH1, &dma_init_struct);
        
        /* DMA0 channel peripheral select */
        dma_channel_subperipheral_select(DMA0, DMA_CH1, DMA_SUBPERI4);
        /* enable DMA0 channel1 */
        dma_channel_enable(DMA0, DMA_CH1);
        /* USART DMA enable for reception */
        usart_dma_enable(USART2, USART_DMA_RECEIVE);
        
        /* wait DMA0 channel2 receive complete */
        while(RESET == dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF));
        printf("\n\r%s\n\r", rxbuffer);
    }
}

/*!
    \brief      initialize the USART configuration of the COM
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART2);
    
    /* connect port to USART TX */
    gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_10);
    /* connect port to USART RX */
    gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_11);
    
    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_11);

    /* USART configure */
    usart_deinit(USART2);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);
    usart_baudrate_set(USART2, 115200U);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);

    usart_enable(USART2);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART2, (uint8_t) ch);
    while(RESET == usart_flag_get(USART2, USART_FLAG_TBE));
    return ch;
}
