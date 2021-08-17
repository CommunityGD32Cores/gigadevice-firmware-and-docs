/*!
    \file  main.c
    \brief CMP port output
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

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int i;
    /* configure RCU */
    rcu_config();
    
    /* configure GPIO */
    gpio_config();
    
    gd_eval_led_init(LED2);
    
    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_VERYLOWSPEED, CMP_1_4VREFINT, CMP_HYSTERESIS_NO);
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);
    
    /* enable comparator channel0 */
    cmp_enable(CMP0);
    
    for( i=0; i<100; i++);
    
    /* get the output level */
    if(CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP0)){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED2);
    }
    
    while (1);
}

/*!
    \brief      configure RCU 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable comparator clock */
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
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_1);

    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_6);
}
