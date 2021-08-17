/*!
    \file    main.c
    \brief   transfer data from RAM to RAM

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h" 
#include "gd32f1x0r_eval.h"

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

void led_config(void);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);

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
    /* initialize LED */
    led_config();

    /* initialize DMA channel1 */
    dma_deinit(DMA_CH1);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)destination_address1;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = DATANUM;
    dma_init_struct.periph_addr = (uint32_t)source_address;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH1);
    dma_memory_to_memory_enable(DMA_CH1);
        
    /* initialize DMA channel2 */
    dma_deinit(DMA_CH2);
    dma_init_struct.memory_addr = (uint32_t)destination_address2;
    dma_init(DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH2);
    dma_memory_to_memory_enable(DMA_CH2);
        
    /* initialize DMA channel3 */
    dma_deinit(DMA_CH3);
    dma_init_struct.memory_addr = (uint32_t)destination_address3;
    dma_init(DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH3);
    dma_memory_to_memory_enable(DMA_CH3);
        
    /* initialize DMA channel4 */
    dma_deinit(DMA_CH4);
    dma_init_struct.memory_addr = (uint32_t)destination_address4;
    dma_init(DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH4);
    dma_memory_to_memory_enable(DMA_CH4);

    /* enable DMA channel1~channel4 */
    dma_channel_enable(DMA_CH1);
    dma_channel_enable(DMA_CH2);
    dma_channel_enable(DMA_CH3);
    dma_channel_enable(DMA_CH4);

    /* wait for DMA transfer complete */
    while(RESET != dma_flag_get(DMA_CH1, DMA_FLAG_FTF)){
        dma_flag_clear(DMA_CH1, DMA_FLAG_G);
        dma_flag_clear(DMA_CH1, DMA_FLAG_FTF);
        dma_flag_clear(DMA_CH1, DMA_FLAG_HTF);
    }
    while(RESET != dma_flag_get(DMA_CH2, DMA_FLAG_FTF)){
        dma_flag_clear(DMA_CH2, DMA_FLAG_G);
        dma_flag_clear(DMA_CH2, DMA_FLAG_FTF);
        dma_flag_clear(DMA_CH2, DMA_FLAG_HTF);
    }
    while(RESET != dma_flag_get(DMA_CH3, DMA_FLAG_FTF)){
        dma_flag_clear(DMA_CH3, DMA_FLAG_G);
        dma_flag_clear(DMA_CH3, DMA_FLAG_FTF);
        dma_flag_clear(DMA_CH3, DMA_FLAG_HTF);
    }
    while(RESET != dma_flag_get(DMA_CH4, DMA_FLAG_FTF)){
        dma_flag_clear(DMA_CH4, DMA_FLAG_G);
        dma_flag_clear(DMA_CH4, DMA_FLAG_FTF);
        dma_flag_clear(DMA_CH4, DMA_FLAG_HTF);
    }

    /* compare the data of source_address with data of destination_address */
    transferflag1 = memory_compare(source_address, destination_address1, DATANUM);
    transferflag2 = memory_compare(source_address, destination_address2, DATANUM);
    transferflag3 = memory_compare(source_address, destination_address3, DATANUM);
    transferflag4 = memory_compare(source_address, destination_address4, DATANUM);

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
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length)
{
    while (length--){
        if (*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
