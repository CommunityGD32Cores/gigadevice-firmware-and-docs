/*!
    \file    main.c
    \brief   standby wakeup through wakeup pin

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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
#include <stdio.h>
#include "gd32f1x0r_eval.h"

void led_config(void);
static void _soft_delay_(uint32_t time);

/* software delay to prevent the impact of Vcore fluctuations.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0U; i<time*10U; i++){
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* LED configuration and turn on all LEDs */
    led_config();
    gd_eval_led_on(LED1);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED4);
    /* tamper key configuration */
    gd_eval_key_init(KEY_TAMPER,KEY_MODE_GPIO);
    /* enable wakeup pin */
    pmu_wakeup_pin_enable(PMU_WAKEUP_PIN0);
    pmu_flag_clear(PMU_FLAG_RESET_STANDBY);
    /* press tamper key to enter standby mode and use wakeup key to wakeup mcu */
    while(1){
        if(RESET == gpio_input_bit_get(TAMPER_KEY_GPIO_PORT,TAMPER_KEY_PIN)) {
            /* The following is to prevent Vcore fluctuations caused by frequency switching. 
               It is strongly recommended to include it to avoid issues caused by self-removal. */
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
            _soft_delay_(100U);
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV4);
            _soft_delay_(100U);
            pmu_to_standbymode();
        }
    }

}

/*!
    \brief      configure LED 
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
