/*!
    \file  main.c
    \brief ADC0 ADC1 Regular Parallel mode
    
    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include "systick.h"
#include <stdio.h>

uint32_t adc_value[2];

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void adc_config(void);
void timer_config(void);

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
    /* GPIO configuration */
    gpio_config();
    /* TIMER configuration */
    timer_config();
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM0);
    /* configure systick */
    systick_config();

    while(1){
        delay_1ms(2000);
        printf(" the data adc_value[0] is %08X \r\n",adc_value[0]);
        printf(" the data adc_value[1] is %08X \r\n",adc_value[1]);
        printf("\r\n");
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable TIMER1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    /* enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable ADC1 clock */
    rcu_periph_clock_enable(RCU_ADC1);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_3);
    gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_5);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;
    
    /* ADC DMA_channel deinit */
    dma_deinit(DMA1,DMA_CH0);
    
    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_SYNCDATA);
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_single_data_parameter.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = 2;
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1,DMA_CH0, &dma_single_data_parameter);
    /* DMA channel 0 peripheral select */
    dma_channel_subperipheral_select(DMA1,DMA_CH0,DMA_SUBPERI0);

    /* enable DMA channel */
    dma_channel_enable(DMA1,DMA_CH0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC0,ADC_REGULAR_CHANNEL,2);
    adc_channel_length_config(ADC1,ADC_REGULAR_CHANNEL,2);
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0,0,ADC_CHANNEL_13,ADC_SAMPLETIME_144);
    adc_regular_channel_config(ADC0,1,ADC_CHANNEL_15,ADC_SAMPLETIME_144);
    adc_regular_channel_config(ADC1,1,ADC_CHANNEL_13,ADC_SAMPLETIME_144);
    adc_regular_channel_config(ADC1,0,ADC_CHANNEL_15,ADC_SAMPLETIME_144);
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0,ADC_REGULAR_CHANNEL,EXTERNAL_TRIGGER_RISING);
    adc_external_trigger_config(ADC1,ADC_REGULAR_CHANNEL,EXTERNAL_TRIGGER_DISABLE);
    adc_external_trigger_source_config(ADC0,ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_T1_CH1);
    
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC1,ADC_DATAALIGN_RIGHT);
    /* configure the ADC sync mode */
    adc_sync_mode_config(ADC_DAUL_REGULAL_PARALLEL);
    adc_sync_dma_config(ADC_SYNC_DMA_MODE1);
    adc_sync_dma_request_after_last_enable();
    /* ADC scan mode function enable */
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);
    adc_special_function_config(ADC1,ADC_SCAN_MODE,ENABLE);
    
    /* enable ADC interface */
    adc_enable(ADC0);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    /* enable ADC interface */
    adc_enable(ADC1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
}

/*!
    \brief      configure the timer peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 19999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH1 configuration in PWM mode0 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,3999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
    
    /* enable TIMER1 */
    timer_enable(TIMER1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM0,USART_FLAG_TBE));
    return ch;
}
