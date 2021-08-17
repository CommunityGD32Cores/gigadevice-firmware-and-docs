/*!
    \file  main.c
    \brief transfer data from RAM to RAM
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h" 
#include "gd32f403_eval.h"
#include <string.h>

#define DATANUM                  16

__IO ErrStatus transferflag1 = ERROR;
__IO ErrStatus transferflag2 = ERROR;
__IO ErrStatus transferflag3 = ERROR;
__IO ErrStatus transferflag4 = ERROR;
uint8_t source_address[DATANUM]= {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                                  0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10};
uint8_t destination_address1[DATANUM];
uint8_t destination_address2[DATANUM];
uint8_t destination_address3[DATANUM];
uint8_t destination_address4[DATANUM];

void destbuf_init(void);
void led_config(void);
ErrStatus uc_data_compare(uint8_t* src, uint8_t* dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int i = 0;
    dma_parameter_struct dma_init_struct;
    
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* initialize LED */
    led_config();
    /* all LED off */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED4);
    
    destbuf_init();
    /* initialize DMA channel1 */
    dma_deinit(DMA0, DMA_CH1);
    
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)destination_address1;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = DATANUM;
    dma_init_struct.periph_addr = (uint32_t)source_address;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH1, dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_enable(DMA0, DMA_CH1);
        
    /* initialize DMA channel2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_init_struct.memory_addr = (uint32_t)destination_address2;
    dma_init(DMA0, DMA_CH2, dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_enable(DMA0, DMA_CH2);
        
    /* initialize DMA channel3 */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.memory_addr = (uint32_t)destination_address3;
    dma_init(DMA0, DMA_CH3, dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_enable(DMA0, DMA_CH3);
        
    /* initialize DMA channel4 */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.memory_addr = (uint32_t)destination_address4;
    dma_init(DMA0, DMA_CH4, dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_enable(DMA0, DMA_CH4);

    /* enable DMA channel1~channel4 */
    dma_channel_enable(DMA0, DMA_CH1);
    dma_channel_enable(DMA0, DMA_CH2);
    dma_channel_enable(DMA0, DMA_CH3);
    dma_channel_enable(DMA0, DMA_CH4);

    /* wait for DMA transfer complete */
    for(i = 0; i < 200; i++);
    /* compare the data of source_address with data of destination_address */
    transferflag1 = uc_data_compare(source_address, destination_address1, DATANUM);
    transferflag2 = uc_data_compare(source_address, destination_address2, DATANUM);
    transferflag3 = uc_data_compare(source_address, destination_address3, DATANUM);
    transferflag4 = uc_data_compare(source_address, destination_address4, DATANUM);

    /* if DMA channel1 transfer success,light LED1 */
    if(SUCCESS == transferflag1){
        gd_eval_led_on(LED1);
    }
    /* if DMA channel2 transfer success,light LED2 */
    if(SUCCESS == transferflag2){
        gd_eval_led_on(LED2);
    }
    /* if DMA channel3 transfer success,light LED3 */
    if(SUCCESS == transferflag3){
        gd_eval_led_on(LED3);
    }
    /* if DMA channel4 transfer success,light LED4 */
    if(SUCCESS == transferflag4){
        gd_eval_led_on(LED4);
    }
    
    while (1);
}

/*!
    \brief      initialize destination buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void destbuf_init(void)
{
    memset(destination_address1, 0, DATANUM);
    memset(destination_address2, 0, DATANUM);
    memset(destination_address3, 0, DATANUM);
    memset(destination_address4, 0, DATANUM);
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
    gd_eval_led_init (LED2);
    gd_eval_led_init (LED3);
    gd_eval_led_init (LED4);
}

/*!
    \brief      data compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus uc_data_compare(uint8_t* src, uint8_t* dst, uint16_t length)
{
    while (length--){
        if (*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
