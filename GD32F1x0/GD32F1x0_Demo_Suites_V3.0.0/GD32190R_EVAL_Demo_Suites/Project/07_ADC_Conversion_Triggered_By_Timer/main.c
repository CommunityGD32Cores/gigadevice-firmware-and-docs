/*!
    \file  main.c
    \brief ADC demo

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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
#include "stdio.h"
#include "systick.h"
#include "gd32f190r_eval.h"

__IO uint16_t ad_value;
float value;

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void timer_config(void);
void adc_config(void);  
  
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* RCU configuration */
    rcu_config();
    /* SYSTICK configuration */
    systick_config();
    gpio_config();  
    gd_eval_com_init(EVAL_COM2);
    /* DMA configuration */
    dma_config();
    /* TIMER configuration */
    timer_config();
    /* ADC configuration */
    adc_config();

    printf("\r\n ADC_Conversion_Triggered_By_Timer demo:\r\n");

    /* TIMER1 counter enable */
    timer_enable(TIMER1);
    /* infinite loop */
    while(1){
        /* test on channel1 transfer complete flag */
        while( !dma_flag_get(DMA_CH0,DMA_FLAG_FTF ));

        /* clear channel1 transfer complete flag */
        dma_flag_clear(DMA_CH0,DMA_FLAG_FTF );

        value = ((float)(ad_value)*5.0/4095.0);
        /* output ADC value on Hyperterminal using printf function */
        printf("\r\n ADC value is %5.3fV\r\n", value);
    }
}

/*!
    \brief      RCU configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* ADCCLK = PCLK2/6 */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    /* enable dma1 and gpioc clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* enable adc1 clock */
    rcu_periph_clock_enable(RCU_ADC);

    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
}

/*!
    \brief      GPIO configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PC0 (adc channel10) as analog input */
    gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_0);
}

/*!
    \brief      DMA configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA channel0 */
    dma_deinit(DMA_CH0);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)&(ad_value);
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_DISABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.number = 1;
    dma_init_struct.periph_addr = (uint32_t)&(ADC_RDATA);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0,&dma_init_struct);
    
    /* configure DMA mode */
    dma_circulation_enable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);
    
    /* enable DMA channel0 */
    dma_channel_enable(DMA_CH0);
}

/*!
    \brief      TIMER configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 7199;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 10000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH1 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
}

/*!
    \brief      ADC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL,1);

    /* ADC regular channel config */
    adc_regular_channel_config(0, ADC_CHANNEL_10, ADC_SAMPLETIME_55POINT5);

    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_T1_CH1);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);  
    /* ADC SCAN function enable */
    adc_special_function_config(ADC_SCAN_MODE,ENABLE);
    /* ADC DMA enable */
    adc_dma_mode_enable();

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);  

    /* enable ADC interface */
    adc_enable();
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM2, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM2,USART_FLAG_TC));
    return ch;
}
