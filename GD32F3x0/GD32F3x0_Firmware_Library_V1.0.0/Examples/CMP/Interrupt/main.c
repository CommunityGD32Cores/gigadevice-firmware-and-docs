/*!
    \file  main.c
    \brief CMP trigger interrupt using an external interrupt line
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include <stdio.h>
#include "gd32f3x0_eval.h"

void led_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure leds */
    led_config();
    
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* configure PA1 as comparator input */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    
    /* enable comparator clock */
    rcu_periph_clock_enable(RCU_CFGCMP);
    
    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_VERYLOWSPEED, CMP_1_4VREFINT, CMP_HYSTERESIS_NO);
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);
    
    /* initialize exti line21 */
    exti_init(EXTI_21, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    
    /* configure ADC_CMP NVIC */
    nvic_irq_enable(ADC_CMP_IRQn, 0, 0);
    
    /* enable comparator channel0 */
    cmp_enable(CMP0);
    
    while(1);
}

/*!
    \brief      configure the leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init (LED2);
}
