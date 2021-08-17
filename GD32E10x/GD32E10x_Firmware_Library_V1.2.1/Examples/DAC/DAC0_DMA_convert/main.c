/*!
    \file    main.c
    \brief   DAC0 DMA convert demo
    
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
    \version 2020-09-30, V1.1.0, firmware for GD32E10x
    \version 2020-12-31, V1.2.0, firmware for GD32E10x
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

#include "gd32e10x.h"
#include "systick.h"

#define CONVERT_NUM          (16)
#define DAC0_R8DH_ADDRESS    (0x40007408)
const uint16_t convertarr[CONVERT_NUM] = {0x000, 0x111, 0x222, 0x333, 0x444, 0x555, 0x666, 0x777,
                                          0x888, 0x999, 0xAAA, 0xBBB, 0xCCC, 0xDDD, 0xEEE, 0xFFF};

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void dac_config(void);
void timer1_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    rcu_config();
    gpio_config();
    dma_config();
    dac_config();
    timer1_config();
    while (1){
    }
}

/*!
    \brief      configure the RCU of peripherals
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the clock of peripherals */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DAC);
    rcu_periph_clock_enable(RCU_TIMER1);
}

/*!
    \brief      configure the related GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* once enabled the DAC, the corresponding GPIO pin is connected to the DAC converter automatically */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
}

/*!
    \brief      configure the DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_struct;
    /* clear all the interrupt flags */
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_GIF);
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_FTFIF);
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_HTFIF);
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_ERRIF);
    
    /* configure the DMA1 channel 2 */
    dma_struct.periph_addr  = DAC0_R8DH_ADDRESS;
    dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_struct.memory_addr  = (uint32_t)convertarr;
    dma_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_struct.number       = CONVERT_NUM;
    dma_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init(DMA1, DMA_CH2, &dma_struct);

    dma_circulation_enable(DMA1, DMA_CH2);
    dma_channel_enable(DMA1, DMA_CH2);
}

/*!
    \brief      configure the DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_source_config(DAC0, DAC_TRIGGER_T1_TRGO);
    dac_trigger_enable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);
    
    /* enable DAC0 and DMA for DAC0 */
    dac_enable(DAC0);
    dac_dma_enable(DAC0);
}

/*!
    \brief      configure the TIMER1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer1_config(void)
{
    /* configure the TIMER5 */
    timer_prescaler_config(TIMER1, 119, TIMER_PSC_RELOAD_UPDATE);
    timer_autoreload_value_config(TIMER1, 999);
    timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_UPDATE);
    
    timer_enable(TIMER1);
}
