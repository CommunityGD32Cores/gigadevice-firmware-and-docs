/*!
    \file    main.c
    \brief   DMA transfer with security and non-security, non-secure code with TZEN = 1

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
#include "gd32w515p_eval.h"

#define BUFFER_SIZE             32U

const uint32_t ns_buffer0[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

uint32_t ns_buffer1[BUFFER_SIZE] = {0};
uint32_t ns_buffer2[BUFFER_SIZE] = {0};
FlagStatus ch1_complete = RESET;
FlagStatus ch2_complete = RESET;
FlagStatus ch3_complete = RESET;
uint32_t check_result = 1;

void dma1_ch1_complete(void);
void dma1_ch2_complete(void);
ErrStatus ns_data_check(uint32_t *src, uint32_t *des);
ErrStatus last_data_check(void);
void ns_dma1_config(void);

/* led spark function */
extern void dma1_config(dma_channel_enum channelx, uint32_t address, void *func);
extern ErrStatus data_check(uint32_t *src, uint32_t *des);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure LED3 */
    gd_eval_led_init(LED3);

    dma1_config(DMA_CH1, (uint32_t)ns_buffer1, dma1_ch1_complete);
    /* wait until transfer complete */
    while(ch1_complete == RESET){
    }
    if(ERROR == ns_data_check((uint32_t *)ns_buffer0, ns_buffer1)){
        check_result = 0;
    }

    /* configure none secure DMA */
    ns_dma1_config();
    /* wait until transfer complete */
    while(ch3_complete == RESET){
    }
    if(ERROR == ns_data_check(ns_buffer1, ns_buffer2)){
        check_result = 0;
    }

    dma1_config(DMA_CH2, (uint32_t)ns_buffer2, dma1_ch2_complete);
    /* wait until transfer complete */
    while(ch2_complete == RESET){
    }
    if(ERROR == last_data_check()){
        check_result = 0;
    }

    if(check_result){
        gd_eval_led_on(LED3);
    }

  while(1){
  }
}

/*!
    \brief      configure the DMA1 channel3
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ns_dma1_config(void)
{
    dma_single_data_parameter_struct dma_init_parameter;
    dma_single_data_para_struct_init(&dma_init_parameter);
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1, DMA_CH3);

    /* configure DMA1 channel3 */
    dma_init_parameter.periph_addr = (uint32_t)ns_buffer1;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_parameter.memory0_addr = (uint32_t)ns_buffer2;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_MEMORY;
    dma_init_parameter.number = BUFFER_SIZE;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH3, &dma_init_parameter);

    /* configure security attribute */
    dma_security_config(DMA1, DMA_CH3, DMA_CHANNEL_NONSECURE | DMA_CHANNEL_SOURCE_NONSECURE | DMA_CHANNEL_DESTINATION_NONSECURE);

    /* configure DMA interrupt */
    nvic_irq_enable(DMA1_Channel3_IRQn, 0, 0);
    dma_interrupt_enable(DMA1, DMA_CH3, DMA_CHXCTL_FTFIE);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH3);
}

/*!
    \brief      DMA1 channel1 interrupt callback function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma1_ch1_complete(void)
{
    ch1_complete = SET;
}

/*!
    \brief      DMA1 channel2 interrupt callback function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma1_ch2_complete(void)
{
    ch2_complete = SET;
}

/*!
    \brief      none secure check data
    \param[in]  none
    \param[out] none
    \retval     none
*/
ErrStatus ns_data_check(uint32_t *src, uint32_t *des)
{
    uint32_t i = BUFFER_SIZE;

    while(i--){
        if(*src != *des){
            return ERROR;
        }
        src++;
        des++;
    }

    return SUCCESS;
}

/*!
    \brief      this function handles DMA1_Channel3_IRQHandler interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA1_Channel3_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA1, DMA_CH3, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA1, DMA_CH3, DMA_INT_FLAG_FTF);
        dma_interrupt_flag_clear(DMA1, DMA_CH3, DMA_INT_FLAG_HTF);
        ch3_complete = SET;
    }
}
