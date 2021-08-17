/*!
    \file  main.c
    \brief CMP switch
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include <stdio.h>
#include "gd32f3x0_eval.h"

void rcu_config(void);
void gpio_config(void);
void dac_config(void);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    /* configure RCU */
    rcu_config();
    
    /* configure GPIO */
    gpio_config();
    
    /* configure DAC */
    dac_config();
    
    /* set data for DAC0 */
    dac_data_set(DAC_ALIGN_12B_R, 0x7FF);
    
    dac_software_trigger_enable();
    
    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_VERYLOWSPEED, CMP_1_4VREFINT, CMP_HYSTERESIS_NO);
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);
    
    /* enable channel0 switch */
    cmp_switch_enable();
    
    /* enable comparator channel0 */
    cmp_enable(CMP0);
    
    while(1)
    {
        if(cmp_output_level_get(CMP0) == CMP_OUTPUTLEVEL_HIGH)
        {
            gd_eval_led_on(LED1);
        }

    }
}

/*!
    \brief      configure RCU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);
    rcu_periph_clock_enable(RCU_CFGCMP);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PA4 */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_4);
    /* configure PA6 */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_6);
}

/*!
    \brief      dac TIMER
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    dac_trigger_source_config(DAC_TRIGGER_SOFTWARE);
    dac_output_buffer_enable();
    dac_enable();
}
