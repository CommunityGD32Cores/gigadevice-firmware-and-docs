/*!
    \file    main.c
    \brief   deepsleep wakeup through exti interrupt

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
#include "gd32f1x0r_eval.h"
#include "systick.h"
#include "main.h"

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
    /* systick configuration */
    systick_config();
    /* LED configuration */
    led_config();
    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* wakeup key configuration */
    gd_eval_key_init(KEY_WAKEUP,KEY_MODE_GPIO);
    /* tamper key EXTI configuration */
    gd_eval_key_init(KEY_TAMPER,KEY_MODE_EXTI);
    /* press wakeup key to enter deepsleep mode and use tamper key to generate a exti interrupt to wakeup mcu */
    while(1) {
        if(RESET == gpio_input_bit_get(WAKEUP_KEY_GPIO_PORT,WAKEUP_KEY_PIN)) {
            /* The following is to prevent Vcore fluctuations caused by frequency switching. 
               It is strongly recommended to include it to avoid issues caused by self-removal. */
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
            _soft_delay_(100U);
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV4);
            _soft_delay_(100U);
            pmu_to_deepsleepmode(PMU_LDO_LOWPOWER,WFI_CMD);
        }
    }
}

/*!
    \brief      toggle the LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if (timingdelaylocal != 0x00U){
        /* all the LEDs on */
        if(timingdelaylocal < 200U){
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
            gd_eval_led_on(LED3);
            gd_eval_led_on(LED4);
        }else{
            /* all the LEDs off */
            gd_eval_led_off(LED1);
            gd_eval_led_off(LED2);
            gd_eval_led_off(LED3);
            gd_eval_led_off(LED4);
        }
        timingdelaylocal--;
    }else{
        timingdelaylocal = 400U;
    }
}

/*!
    \brief      LED configuration
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
}
