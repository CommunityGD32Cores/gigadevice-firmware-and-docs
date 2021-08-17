/*!
    \file  main.c
    \brief ADC discontinuous mode for regular channel 
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f403_eval.h"

uint16_t adc_value[16];

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
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
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();
  
    while(1){
        /* delay 1s */
        delay_1ms(1000);
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);      
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
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);  
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
    /* config the GPIO as analog mode */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);  
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
    
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 16;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC continuous function disable */
    adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, DISABLE);
    adc_special_function_config(ADC1, ADC_SCAN_MODE, DISABLE);   
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 8);
 
  
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_2, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_4, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 6, ADC_CHANNEL_6, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 7, ADC_CHANNEL_7, ADC_SAMPLETIME_55POINT5);

    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    
    /* ADC discontinuous mode */
    adc_discontinuous_mode_config(ADC0, ADC_REGULAR_CHANNEL, 3);


    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);


    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);
}
