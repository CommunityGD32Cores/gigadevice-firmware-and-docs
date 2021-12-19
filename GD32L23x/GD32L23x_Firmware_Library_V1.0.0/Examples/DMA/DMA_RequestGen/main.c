/*!
    \file    main.c
    \brief   DMAMUX request generation example

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "gd32l23x.h"
#include "gd32l233r_eval.h"
#include <string.h>

#define DATANUM                  16

uint8_t source_address[DATANUM] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                   0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
                                  };
uint8_t destination_address[DATANUM];
__IO ErrStatus transferflag = ERROR;
__IO uint32_t g_transfer_complete = 0;

void rcu_config(void);
void destbuf_init(void);
void led_config(void);
void nvic_config(void);
void dma_config(void);
ErrStatus uc_data_compare(uint8_t *src, uint8_t *dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* key for DMA request synchronization configuration */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    /* key for DMA request generation configuration */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    /* LEDs configuration */
    led_config();
    /* interrupt configuration */
    nvic_config();
    /* initialize destination buffer */
    destbuf_init();
    /* configure DMA and DMAMUX channel */
    dma_config();

    /* wait for DMA transfer complete */
    while(1U != g_transfer_complete) {
    }
    /* compare the data of source_address with data of destination_address */
    transferflag = uc_data_compare(source_address, destination_address, DATANUM);

    if(SUCCESS != transferflag) {
        /* error */
        gd_eval_led_on(LED1);
    } else {
        /* success */
        gd_eval_led_on(LED2);
    }

    while(1) {
    }
}

/*!
    \brief      system clocks configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
}

/*!
    \brief      interrupt configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA_Channel0_IRQn, 2);
    nvic_irq_enable(DMAMUX_IRQn, 2);
}

/*!
    \brief      initialize destination buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void destbuf_init(void)
{
    memset(destination_address, 0, DATANUM);
}

/*!
    \brief      LEDs configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
}

/*!
    \brief      configure DMA and DMAMUX channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    dmamux_gen_parameter_struct    dmamux_gen_init_struct;
    dmamux_sync_parameter_struct dmamux_sync_init_struct ;

    /* DMA channel configuration */
    /* initialize DMA channel 0 */
    dma_deinit(DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_GENERATOR0;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)source_address;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = DATANUM;
    dma_init_struct.periph_addr  = (uint32_t)destination_address;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);

    /* DMAMUX channel configuration */
    /* initialize DMA request generator channel 0 */
    dmamux_gen_struct_para_init(&dmamux_gen_init_struct);
    dmamux_gen_init_struct.trigger_id       = DMAMUX_TRIGGER_EXTI13;
    dmamux_gen_init_struct.trigger_polarity = DMAMUX_GEN_RISING;
    dmamux_gen_init_struct.request_number   = 1;
    dmamux_request_generator_init(DMAMUX_GENCH0, &dmamux_gen_init_struct);
    dmamux_request_generator_chennel_enable(DMAMUX_GENCH0);

    /* initialize DMA request multiplexer channel 0 with synchronization mode */
    dmamux_sync_struct_para_init(&dmamux_sync_init_struct);
    dmamux_sync_init_struct.sync_id        = DMAMUX_SYNC_EXTI0;
    dmamux_sync_init_struct.sync_polarity  = DMAMUX_SYNC_RISING;
    dmamux_sync_init_struct.request_number = 16;
    dmamux_synchronization_init(DMAMUX_MUXCH0, &dmamux_sync_init_struct);
    dmamux_synchronization_enable(DMAMUX_MUXCH0);

    /* enable DMAMUX interrupt */
    dmamux_interrupt_enable(DMAMUX_INT_MUXCH0_SO);
    dmamux_interrupt_enable(DMAMUX_INT_GENCH0_TO);

    /* enable DMA interrupt */
    dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);
}

/*!
    \brief      data compare function
    \param[in]  src: source data
    \param[in]  dst: destination data
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus uc_data_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
