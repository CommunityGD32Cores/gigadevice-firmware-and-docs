/*!
    \file  main.c
    \brief USART DMA transmitter receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include <stdio.h>
#include "gd32f3x0_eval.h"

uint8_t rxbuffer[10];
uint8_t txbuffer[]="\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";
#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define USART0_TDATA_ADDRESS      ((uint32_t)0x40013828)  
#define USART0_RDATA_ADDRESS      ((uint32_t)0x40013824)  

extern __IO uint8_t txcount; 
extern __IO uint16_t rxcount; 


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    gd_eval_com_init(EVAL_COM1);

    printf("\n\ra usart dma function test example!\n\r");

    /* deinitialize DMA channel1 */
    dma_deinit(DMA_CH1);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)txbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr = USART0_TDATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1,dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH1);
    dma_memory_to_memory_disable(DMA_CH1);
    /* enable DMA channel1 */
    dma_channel_enable(DMA_CH1);
    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    /* wait DMA Channel transfer complete */
    while(RESET == dma_flag_get(DMA_CH1, DMA_FLAG_FTF));
    while(1){
        /* deinitialize DMA channel2 */
        dma_deinit(DMA_CH2);
        dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr = (uint32_t)rxbuffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = 10;
        dma_init_struct.periph_addr = USART0_RDATA_ADDRESS;
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(DMA_CH2, dma_init_struct);
        
        dma_circulation_disable(DMA_CH2);
        dma_memory_to_memory_disable(DMA_CH2);

        dma_channel_enable(DMA_CH2);
        usart_dma_receive_config(USART0, USART_DENR_ENABLE);
        /* wait DMA channel transfer complete */
        while(RESET == dma_flag_get( DMA_CH2, DMA_FLAG_FTF));
        printf("\n\r%s\n\r",rxbuffer);

    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));
    return ch;
}
