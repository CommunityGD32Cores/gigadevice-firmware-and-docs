/*!
    \file  main.c
    \brief USART DMA transmitter receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"


uint8_t rxbuffer[10];
uint8_t txbuffer[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";
#define ARRAYNUM(arr_name)     (uint32_t)(sizeof(arr_name) / sizeof(*(arr_name)))
#define USART0_DATA_ADDRESS    ((uint32_t)0x40013804)

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA1 */
    rcu_periph_clock_enable(RCU_DMA0);
    /* initialize USART */
    gd_eval_com_init(EVAL_COM1);
    /* deinitialize DMA channel3(USART0 tx) */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)txbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    /* enable DMA channel3 */
    dma_channel_enable(DMA0, DMA_CH3);
    
    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    usart_dma_receive_config(USART0, USART_DENR_ENABLE);

     /* wait DMA Channel transfer complete */
    while(RESET == dma_flag_get(DMA0, DMA_CH3, DMA_INTF_FTFIF));
    while(1){
        /* deinitialize DMA channel4 (USART0 rx) */
        dma_deinit(DMA0, DMA_CH4);
        dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr = (uint32_t)rxbuffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = 10;
        dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(DMA0, DMA_CH4, dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(DMA0, DMA_CH4);        
        /* enable DMA channel4 */
        dma_channel_enable(DMA0, DMA_CH4);

        /* wait DMA channel transfer complete */
        while(RESET == dma_flag_get(DMA0, DMA_CH4, DMA_INTF_FTFIF));
        printf("\n\r%s\n\r", rxbuffer);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));
    return ch;
}
