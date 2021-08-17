/*!
    \file  main.c
    \brief transfer data from RAM to USART
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h" 
#include "gd32f403_eval.h"
  
#define USART0_DATA_ADDRESS      ((uint32_t)0x40013804)
#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))

uint8_t welcome[]="hi,this is a example: RAM_TO_USART by DMA !\n";
FlagStatus g_transfer_complete = RESET;

void led_config(void);
void nvic_config(void);

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
    rcu_periph_clock_enable(RCU_DMA0);
    /* USART configure */
    gd_eval_com_init(EVAL_COM1);
    /*configure DMA interrupt*/
    nvic_config();
    /* initialize LED */
    led_config();   
    /* all LED off */
    gd_eval_led_off(LED1);
    
    /* initialize DMA channel1 */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)welcome;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(welcome);
    dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, dma_init_struct);
    
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    
    /* enable DMA transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF);
    
    /* enable DMA channel1 */
    dma_channel_enable(DMA0, DMA_CH3);
   
    /* waiting for the transfer to complete*/
    while(RESET == g_transfer_complete);
    
    /* light LED1 */
    gd_eval_led_on(LED1);

    while (1);
}

/*!
    \brief      LEDs configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init (LED1);
}

/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA0_Channel3_IRQn, 0, 0);
}
