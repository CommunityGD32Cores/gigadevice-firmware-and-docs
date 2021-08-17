/*!
    \file  main.c
    \brief ADC analog watchdog
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F03
*/

#include "gd32f403.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f403_eval.h"


#define BOARD_ADC_CHANNEL ADC_CHANNEL_3
#define ADC_GPIO_PORT GPIOA
#define ADC_GPIO_PIN  GPIO_PIN_3

int data;

void rcu_config(void);
void gpio_config(void);
void nvic_config(void);
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
    /* NVIC configuration */
    nvic_config();
    /* ADC configuration */
    adc_config();

    while(1){
        if(1 == data){
            data = 0;
            delay_1ms(3000);
            gd_eval_led_off(LED2);
        }      
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
    /* enable ADC1 clock */
    rcu_periph_clock_enable(RCU_ADC1);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure led GPIO */
    gd_eval_led_init(LED2);
    /* config the GPIO as analog mode */
    gpio_init(ADC_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_MAX, ADC_GPIO_PIN);
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(ADC0_1_IRQn, 0, 0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC continuous function enable */
    adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC1, ADC_SCAN_MODE, DISABLE);   
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC channel length config */
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 1);
 
  
    /* ADC regular channel config */
    adc_regular_channel_config(ADC1, 0, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_55POINT5);
  
    adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);
    
    /* enable ADC interface */
    adc_enable(ADC1);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);


    /* ADC analog watchdog threshold config */
    adc_watchdog_threshold_config(ADC1, 0x0400, 0x0A00);
    /* ADC analog watchdog single channel config */
    adc_watchdog_single_channel_enable(ADC1, BOARD_ADC_CHANNEL);
    /* ADC interrupt config */
    adc_interrupt_enable(ADC1, ADC_INT_WDE);


    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);
}
