/*!
    \file  main.c
    \brief ADC0_ADC1_regular_parallel
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f403_eval.h"

uint32_t adc_value[2];

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
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();  
    /* GPIO configuration */
    gpio_config();
    /* TIMER configuration */
    timer_config();
    /* DMA configuration */
    dma_config();  
    /* ADC configuration */
    adc_config();

    /* enable TIMER0 */
    timer_enable(TIMER0);
  
    while(1){ 
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
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable ADC1 clock */
    rcu_periph_clock_enable(RCU_ADC1);  
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);  
    /* enable timer0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* enable AF clock */
    rcu_periph_clock_enable(RCU_AF);   
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configures led GPIO */
    gd_eval_led_init(LED2);  
    /* config the GPIO as analog mode */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0|GPIO_PIN_3);

    /*configure PA8(TIMER0 CH0) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8);  
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
    dma_parameter_struct dma_data_parameter;
    
    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);
    
    /* initialize DMA data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;  
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 2;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 8399;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

    /* CH0 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 3999);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
  
    /* TIMER0 primary output enable */
    timer_primary_output_config(TIMER0,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);    
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC continous function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE); 
    adc_special_function_config(ADC1, ADC_SCAN_MODE, ENABLE);  
    adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, DISABLE);  
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T0_CH0);
    adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);  
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);  
    /* ADC mode config */
    adc_mode_config(ADC_DAUL_REGULAL_PARALLEL); 
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 2);
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 2);
  
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC1, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC1, 1, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);

  
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);    

    /* enable ADC interface */
    adc_enable(ADC0);  
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    /* enable ADC interface */    
    adc_enable(ADC1);    
    delay_1ms(1);
    /* ADC calibration and reset calibration */    
    adc_calibration_enable(ADC1);    


    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);
}
