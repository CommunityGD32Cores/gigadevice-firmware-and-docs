/*!
    \file    main.c
    \brief   CMP watchdog window

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include <stdio.h>
#include "systick.h"
#include "gd32l23x.h"
#include "gd32l233r_eval.h"
#include "main.h"


void led_config(void);
void cmp_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* configure leds */
    led_config();

    /* configure CMP0 and CMP1 */
    cmp_config();

    /* configure CMP0 and CMP1 */
    check_state();

    while(1) {
        /* input voltage is over the thresholds: higher and lower thresholds */
        if(STATE_OVER_THRESHOLD == check_state()) {
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
            gd_eval_led_off(LED3);
            gd_eval_led_off(LED4);
        }
        /* input voltage is within the thresholds: higher and lower thresholds */
        if(STATE_WITHIN_THRESHOLD == check_state()) {
            delay_1ms(500);
            if(STATE_WITHIN_THRESHOLD == check_state()) {
                gd_eval_led_on(LED1);
                gd_eval_led_off(LED2);
                gd_eval_led_on(LED3);
                gd_eval_led_off(LED4);
            }
        }
        /* input voltage is under the thresholds: higher and lower thresholds */
        if(STATE_UNDER_THRESHOLD == check_state()) {
            gd_eval_led_off(LED1);
            gd_eval_led_off(LED2);
            gd_eval_led_on(LED3);
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
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_1);

    /* enable comparator clock */
    rcu_periph_clock_enable(RCU_CMP);

    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_HIGHSPEED, CMP_VREFINT, CMP_HYSTERESIS_NO);
    cmp_voltage_scaler_enable(CMP0);
    cmp_scaler_bridge_enable(CMP0);

    /* configure comparator channel1 */
    cmp_mode_init(CMP1, CMP_HIGHSPEED, CMP_1_2VREFINT, CMP_HYSTERESIS_NO);
    cmp_voltage_scaler_enable(CMP1);
    cmp_scaler_bridge_enable(CMP1);

    /* enable comparator window */
    cmp_window_enable();


    delay_1ms(1);

    /* configure comparator channel0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);

    /* configure comparator channel1 output */
    cmp_output_init(CMP1, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);

    /* enable comparator channels */
    cmp_enable(CMP0);
    cmp_enable(CMP1);
}

/*!
    \brief      check comparator output state
    \param[in]  none
    \param[out] none
    \retval     cmp_state
*/
cmp_state_enum check_state(void)
{
    cmp_state_enum state = STATE_WITHIN_THRESHOLD;

    /* check if cmp0 output level is high and cmp1 output level is high */
    if((CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP0))
            && (CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP1))) {
        state = STATE_OVER_THRESHOLD;
    }
    /* check if cmp0 output level is low and cmp1 output level is high */
    if((CMP_OUTPUTLEVEL_LOW == cmp_output_level_get(CMP0))
            && (CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP1))) {
        state = STATE_WITHIN_THRESHOLD;
    }
    /* check if cmp0 output level is low and cmp1 output level is low */
    if((CMP_OUTPUTLEVEL_LOW == cmp_output_level_get(CMP0))
            && (CMP_OUTPUTLEVEL_LOW == cmp_output_level_get(CMP1))) {
        state = STATE_UNDER_THRESHOLD;
    }
    return state;
}

/*!
    \brief      configure the leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}
