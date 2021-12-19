/*!
    \file    main.c
    \brief   HPDF I2S audio demo

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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
#include <stdio.h>

#define BUFFER_SIZE           3072
#define PLAY_BUFFER_SIZE      6144
#define CKOUTDIV_88           87
#define FLT_OVER_SAMPLE_64    64

/* limit the range of data */
#define DATA_LIMIT(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

/* PDM left channel data */
int32_t pcm_left_data[BUFFER_SIZE];
/* PDM right channel data */
int32_t pcm_right_data[BUFFER_SIZE];
/* audio data  */
int16_t pcm_paly_data[PLAY_BUFFER_SIZE]={0};

int32_t hpdf_rc_val;
uint8_t i2s_flag = 0;

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void spi_config(void);
void hpdf_config(void);
void i2s_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i;
    /* open the icache */
    icache_enable();
    /* enable interrupt */
    nvic_irq_enable(SPI1_IRQn, 0, 1);
    /* configure RCU */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure DMA */
    dma_config();
    /* configure I2S */
    i2s_config();
    /* configure HPDF */
    hpdf_config();
    while(1){
        /* wait for DMA half-full transmit complete */
        while(RESET == dma_flag_get(DMA1, DMA_CH1, DMA_FLAG_HTF));
        while(RESET == dma_flag_get(DMA1, DMA_CH2, DMA_FLAG_HTF));
        /* get the PCM stereo data */
        for(i = 0; i < 1536; i++){
            pcm_paly_data[2*i] = DATA_LIMIT((pcm_left_data[i] >> 9), -32768, 32767);
            pcm_paly_data[(2*i)+1] = DATA_LIMIT((pcm_right_data[i] >> 9),-32768, 32767) ;
        }
        /* clear the half transfer finish flag */
        dma_flag_clear(DMA1, DMA_CH1, DMA_FLAG_HTF);
        dma_flag_clear(DMA1, DMA_CH2, DMA_FLAG_HTF);
        /* wait for DMA full transmit complete */
        while(RESET == dma_flag_get(DMA1, DMA_CH1, DMA_FLAG_FTF));
        while(RESET == dma_flag_get(DMA1, DMA_CH2, DMA_FLAG_FTF));
        /* get the PCM stereo data */
        for(i = 1536; i < 3072; i++){
            pcm_paly_data[2*i] = DATA_LIMIT((pcm_left_data[i] >> 9), -32768, 32767);
            pcm_paly_data[(2*i)+1] = DATA_LIMIT((pcm_right_data[i] >> 9),-32768, 32767) ;
        }
        /* clear the full transfer finish flag */
        dma_flag_clear(DMA1, DMA_CH1, DMA_FLAG_FTF);
        dma_flag_clear(DMA1, DMA_CH2, DMA_FLAG_FTF);
        /* enable I2S interrupt */
        if(0 == i2s_flag){
            spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_TBE);
            i2s_flag = 1;
        }
    }
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* cofigure I2S clock */
    rcu_hxtal_plli2s_enable();
    rcu_plli2s_config(48, RCU_PLLI2SSRC_DIV8, RCU_PLLI2S_DIV2);
    rcu_osci_on(RCU_PLLI2S_CK);
    while(0 == (RCU_CTL & RCU_CTL_PLLI2SSTB));
    rcu_i2s_clock_config(RCU_I2SSRC_PLLI2S);

    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable I2S1 clock */
    rcu_periph_clock_enable(RCU_SPI1);
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable HPDF clock */
    rcu_periph_clock_enable(RCU_HPDF);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* connect PC2 to HPDF_CKOUT */
    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_2);
    /* connect PA3 to HPDF_DATA1 */
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_3);
    /* configure GPIO pins of HPDF */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_3);
    
    /* I2S1_MCK(PA5), I2S1_CK(PB8), I2S1_WS(PA7), I2S1_SD(PA4) GPIO pin configuration */
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_4);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_5);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7); 
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_8); 
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

    /* configure serial clock output */
    hpdf_clock_output_config(SERIAL_SYSTEM_CLK, CKOUTDIV_88, CKOUTDM_ENABLE);

    /* initialize HPDF channel0 */
    hpdf_channel_init_struct.data_packing_mode      = DPM_STANDARD_MODE;
    hpdf_channel_init_struct.channel_pin_select     = CHPINSEL_NEXT;
    hpdf_channel_init_struct.ck_loss_detector       = CLK_LOSS_DISABLE;
    hpdf_channel_init_struct.malfunction_monitor    = MM_ENABLE;
    hpdf_channel_init_struct.spi_ck_source          = INTERNAL_CKOUT;
    hpdf_channel_init_struct.channel_multiplexer    = SERIAL_INPUT;
    hpdf_channel_init_struct.serial_interface       = SPI_FALLING_EDGE;
    hpdf_channel_init_struct.calibration_offset     = 0;
    hpdf_channel_init_struct.right_bit_shift        = 0;
    hpdf_channel_init_struct.tm_filter              = TM_FASTSINC;
    hpdf_channel_init_struct.tm_filter_oversample   = TM_FLT_BYPASS;
    hpdf_channel_init_struct.mm_break_signal        = DISABLE;
    hpdf_channel_init_struct.mm_counter_threshold   = 255;
    hpdf_channel_init_struct.plsk_value             = 0;
    hpdf_channel_init(CHANNEL0, &hpdf_channel_init_struct);
    
    /* initialize HPDF channel1 */
    hpdf_channel_init_struct.channel_pin_select     = CHPINSEL_CURRENT;
    hpdf_channel_init_struct.serial_interface       = SPI_RISING_EDGE;
    hpdf_channel_init(CHANNEL1, &hpdf_channel_init_struct);

    /* initialize HPDF filter1 and filter1 */
    hpdf_filter_init_struct.sinc_filter             = FLT_SINC3;
    hpdf_filter_init_struct.sinc_oversample         = FLT_OVER_SAMPLE_64;
    hpdf_filter_init_struct.integrator_oversample   = INTEGRATOR_BYPASS;
    hpdf_filter_init(FLT0, &hpdf_filter_init_struct);
    hpdf_filter_init(FLT1, &hpdf_filter_init_struct);

    /* initialize HPDF filter0 regular conversions */
    hpdf_rc_init_struct.fast_mode       = FAST_ENABLE;
    hpdf_rc_init_struct.rcs_channel     = RCS_CHANNEL0;
    hpdf_rc_init_struct.rcdmaen         = RCDMAEN_ENABLE;
    hpdf_rc_init_struct.continuous_mode = RCCM_ENABLE;
    hpdf_rc_init(FLT0, &hpdf_rc_init_struct);

    /* initialize HPDF filter1 regular conversions */
    hpdf_rc_init_struct.rcs_channel     = RCS_CHANNEL1;
    hpdf_rc_init(FLT1, &hpdf_rc_init_struct);

    /* enable channel */
    hpdf_channel_enable(CHANNEL0);
    hpdf_channel_enable(CHANNEL1);
    /* enable filter */
    hpdf_filter_enable(FLT0);
    hpdf_filter_enable(FLT1);
    /* enable the HPDF module globally */
    hpdf_enable();
    /* enable regular channel conversion by software */
    hpdf_rc_start_by_software(FLT0);
    hpdf_rc_start_by_software(FLT1);
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
    dma_init_parameter.periph_addr         = (int32_t)&HPDF_FLTyRDATA(FLT0);
    dma_init_parameter.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr        = (uint32_t)pcm_right_data;
    dma_init_parameter.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.circular_mode       = DMA_CIRCULAR_MODE_ENABLE;
    dma_init_parameter.direction           = DMA_PERIPH_TO_MEMORY;
    dma_init_parameter.number              = BUFFER_SIZE;
    dma_init_parameter.priority            = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH1, &dma_init_parameter);
    /* connect DMA1_CH1 to HPDF_FLT0 */
    dma_channel_subperipheral_select(DMA1, DMA_CH1, DMA_SUBPERI7);
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH1);

    /* deinitialize DMA1_CH2 */
    dma_deinit(DMA1, DMA_CH2);
    /* configure DMA1_CH2 */
    dma_init_parameter.periph_addr  = (int32_t)&HPDF_FLTyRDATA(FLT1);
    dma_init_parameter.memory0_addr = (uint32_t)pcm_left_data;
    dma_init_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH2, &dma_init_parameter);
    /* connect DMA1_CH2 to HPDF_FLT1 */
    dma_channel_subperipheral_select(DMA1, DMA_CH2, DMA_SUBPERI7);
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH2);
}

/*!
    \brief      configure the I2S peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2s_config()
{
    spi_i2s_deinit(SPI1);

    /* I2S1 peripheral configuration */
    i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_16K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_MSB, I2S_CKPL_HIGH);
    /* enable the I2S1 peripheral */
    i2s_enable(SPI1); 
}
