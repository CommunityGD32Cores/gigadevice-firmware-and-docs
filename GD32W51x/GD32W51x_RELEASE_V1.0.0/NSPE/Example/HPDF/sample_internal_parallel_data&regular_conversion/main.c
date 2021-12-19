/*!
    \file    main.c
    \brief   HPDF sample internal data by regular conversion
    
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
#include "gd32w515p_eval.h"

#define FLT_OVER_SAMPLE_32    32
#define FLT_ORDER             FLT_SINC3 
#define HPDF_OUTPUT_BUFFER    2
#define HPDF_INPUT_BUFFER     ((((FLT_ORDER >> 29) * (FLT_OVER_SAMPLE_32)) + 4) * (HPDF_OUTPUT_BUFFER))

/* limit the range of data */
#define DATA_LIMIT(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

int16_t internal_data[HPDF_INPUT_BUFFER] = {0};
int16_t final_data[HPDF_OUTPUT_BUFFER] = {0};
int32_t conversion_data[HPDF_OUTPUT_BUFFER] = {0};

int32_t hpdf_rc_val;

void rcu_config(void);
void gpio_config(void);
void hpdf_config(void);
void dma_config(void);
void i2s_play_audio(void);
void led_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i;
    for(i = 0; i < (HPDF_INPUT_BUFFER / 2); i++){
        internal_data[i] = -1;
    }
    for(i = (HPDF_INPUT_BUFFER / 2); i < HPDF_INPUT_BUFFER; i++){
        internal_data[i] = 1;
    }
    /* configure RCU */
    rcu_config();
    /* configure USART */
    gd_eval_com_init(EVAL_COM0);
    printf(" The HPDF test:\r\n ");
    /* configure DMA */
    dma_config();
    /* configure HPDF */
    hpdf_config();
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH1);
    for(i = 0; i < HPDF_INPUT_BUFFER; i++){
        hpdf_write_parallel_data_standard_mode(CHANNEL0, internal_data[i]);
    }
    /* wait for the DMA transmit complete */
    while(RESET == dma_flag_get(DMA1, DMA_CH1, DMA_FLAG_FTF));
    printf(" The range of maximum output resolution:\n ");
    /* get the PCM stereo data */
    for(i = 0; i < HPDF_OUTPUT_BUFFER; i++){
        final_data[i] = DATA_LIMIT((conversion_data[i] >> 8), -32768, 32767);
        printf("%d ", final_data[i]);
    }
    while(1){
    }
}

/*!
    \brief      configure the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable HPDF clock */
    rcu_periph_clock_enable(RCU_HPDF);
}

/*!
    \brief      configure the HPDF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hpdf_config(void)
{
    hpdf_channel_parameter_struct hpdf_channel_init_struct;
    hpdf_filter_parameter_struct hpdf_filter_init_struct;
    hpdf_rc_parameter_struct hpdf_rc_init_struct;

    /* reset HPDF */
    hpdf_deinit();

    /*  initialize the parameters */
    hpdf_channel_struct_para_init(&hpdf_channel_init_struct);
    hpdf_filter_struct_para_init(&hpdf_filter_init_struct);
    hpdf_rc_struct_para_init(&hpdf_rc_init_struct);

    /* initialize HPDF channel0 */
    hpdf_channel_init_struct.data_packing_mode      = DPM_STANDARD_MODE;
    hpdf_channel_init_struct.channel_multiplexer    = INTERNAL_INPUT;
    hpdf_channel_init_struct.calibration_offset     = 0;
    hpdf_channel_init_struct.right_bit_shift        = 0;
    hpdf_channel_init(CHANNEL0, &hpdf_channel_init_struct);

    /* initialize HPDF filter0 and filter1 */
    hpdf_filter_init_struct.sinc_filter             = FLT_ORDER;
    hpdf_filter_init_struct.sinc_oversample         = FLT_OVER_SAMPLE_32;
    hpdf_filter_init_struct.integrator_oversample   = INTEGRATOR_BYPASS;
    hpdf_filter_init(FLT0, &hpdf_filter_init_struct);

    /* initialize HPDF filter0 regular conversions */
    hpdf_rc_init_struct.fast_mode       = FAST_DISABLE;
    hpdf_rc_init_struct.rcs_channel     = RCS_CHANNEL0;
    hpdf_rc_init_struct.rcdmaen         = RCDMAEN_ENABLE;
    hpdf_rc_init_struct.continuous_mode = RCCM_ENABLE;
    hpdf_rc_init(FLT0, &hpdf_rc_init_struct);

    /* enable channel */
    hpdf_channel_enable(CHANNEL0);
    /* enable filter */
    hpdf_filter_enable(FLT0);
    /* enable the HPDF module globally */
    hpdf_enable();
    /* enable regular channel conversion by software */
    hpdf_rc_start_by_software(FLT0);
}

/*!
    \brief      configure DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_parameter;
    dma_single_data_para_struct_init(&dma_init_parameter);
    
    /* deinitialize DMA1_CH1 */
    dma_deinit(DMA1, DMA_CH1);
    /* configure DMA1_CH1 */
    dma_init_parameter.periph_addr = (int32_t)&HPDF_FLTyRDATA(FLT0);
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr = (uint32_t)conversion_data;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_parameter.number = HPDF_OUTPUT_BUFFER;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH1, &dma_init_parameter);
    /* connect DMA1_CH1 to HPDF_FLT0 */
    dma_channel_subperipheral_select(DMA1, DMA_CH1, DMA_SUBPERI7);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}
