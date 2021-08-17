/*!
    \file  main.c
    \brief CMP watchdog window
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include <stdio.h>
#include "gd32f3x0_eval.h"
#include "main.h"

uint32_t delayms;
void led_config(void);
void deepsleep_mode_config(void);
void cmp_config(void);
void delay_ms(uint32_t time);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    SysTick_Config(72000000/1000);
    
    /* configure leds */
    led_config();

    /* configure CMP0 and CMP1 */
    cmp_config();
    
    /* configure CMP0 and CMP1 */
    check_state();
    
    while(1)
    {
        /* input voltage is over the thresholds: higher and lower thresholds */
        if(STATE_OVER_THRESHOLD == check_state()){
            gd_eval_led_on(LED1);
            gd_eval_led_off(LED2);
            gd_eval_led_on(LED3);
            gd_eval_led_off(LED4);
        }
        /* input voltage is within the thresholds: higher and lower thresholds */
        if(STATE_WITHIN_THRESHOLD == check_state()){
            delay_ms(500);
            if(STATE_WITHIN_THRESHOLD == check_state()){
                gd_eval_led_off(LED1);
                gd_eval_led_off(LED2);
                gd_eval_led_off(LED3);
                gd_eval_led_off(LED4);
                /* enter deepsleep mode */
                deepsleep_mode_config();
            }
        }
        /* input voltage is under the thresholds: higher and lower thresholds */
        if(STATE_UNDER_THRESHOLD == check_state()){
            gd_eval_led_off(LED1);
            gd_eval_led_on(LED2);
            gd_eval_led_off(LED3);
            gd_eval_led_on(LED4);
        }
    }
}

/*!
    \brief      configure comparator
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* configure PA1 as comparator input */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    
    /* enable comparator clock */
    rcu_periph_clock_enable(RCU_CFGCMP);
    
    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_LOWSPEED, CMP_VREFINT, CMP_HYSTERESIS_HIGH);
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);
    
    /* configure comparator channel1 */
    cmp_mode_init(CMP1, CMP_LOWSPEED, CMP_1_2VREFINT, CMP_HYSTERESIS_HIGH);
    cmp_output_init(CMP1, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);
    
    /* configure exti line */
    exti_init(EXTI_21, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(EXTI_22, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    
    /* configure ADC_CMP nvic */
    nvic_irq_enable(ADC_CMP_IRQn, 0, 0);
    
    /* enable comparator window */
    cmp_window_enable();
    
    /* enable comparator channels */
    cmp_enable(CMP0);
    cmp_enable(CMP1);
}

/*!
    \brief      configure deepsleep mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void deepsleep_mode_config(void)  
{
    /* enable pmu clock */
    rcu_periph_clock_enable(RCU_PMU);

   /* enter to deepsleep mode */
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);  
}

/*!
    \brief      check comparator output state
    \param[in]  none
    \param[out] none
    \retval     cmp_state
*/
cmp_state_enum check_state(void)
{
    cmp_state_enum state;

    /* check if cmp0 output level is high and cmp1 output level is high */
    if ((cmp_output_level_get(CMP0) == CMP_OUTPUTLEVEL_HIGH)
    && (cmp_output_level_get(CMP1) == CMP_OUTPUTLEVEL_HIGH)){
        state = STATE_OVER_THRESHOLD;
    }
    /* check if cmp0 output level is low and cmp1 output level is high */
    if ((cmp_output_level_get(CMP0) == CMP_OUTPUTLEVEL_LOW) 
    && (cmp_output_level_get(CMP1) == CMP_OUTPUTLEVEL_HIGH)){
        state = STATE_WITHIN_THRESHOLD;
    }
    /* check if cmp0 output level is low and cmp1 output level is low */
    if ((cmp_output_level_get(CMP0) == CMP_OUTPUTLEVEL_LOW)
    && (cmp_output_level_get(CMP1) == CMP_OUTPUTLEVEL_LOW)){
        state = STATE_UNDER_THRESHOLD;
    }
    return state;
}

/*!
    \brief      delay function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_ms(uint32_t time)
{
    delayms = time;
    while(delayms);
}

/*!
    \brief      configure the leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init (LED1);
    gd_eval_led_init (LED2);
    gd_eval_led_init (LED3);
    gd_eval_led_init (LED4);
}
