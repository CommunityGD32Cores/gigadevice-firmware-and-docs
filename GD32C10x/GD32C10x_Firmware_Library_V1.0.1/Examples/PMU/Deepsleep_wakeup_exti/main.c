/*!
    \file    main.c
    \brief   deepsleep wakeup through exti interrupt
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#include "gd32c10x.h"
#include "gd32c10x_eval.h"
#include "systick.h"
#include "main.h"

void led_config(void);
void rcu_config(void);
void key_config(void);
void ckout0_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick config */
    systick_config();

    /* configure leds */
    led_config();

    /* enable the peripheral clock */
    rcu_config();

    /* configure keys */
    key_config();

    /* configure the ckout0 to measure the system clock on PA8 */
    ckout0_config();
    
    /* press wakeup key to enter Deep-sleep mode and use tamper key generate a exti interrupt to wakeup mcu */
    while(1){
        if(RESET == gpio_input_bit_get(WAKEUP_KEY_GPIO_PORT, WAKEUP_KEY_PIN)){
            /* enter Deep-sleep mode */
            pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);

            /* wait until press the user key */
            while(RESET != gpio_input_bit_get(USER_KEY_GPIO_PORT, USER_KEY_PIN)){
            }

            /* reinitialize the system clock after waking up from Deep-sleep mode.
               if not, the CK_SYS source will be forced to IRC8M when leaving Deep-sleep mode */
            SystemInit();

            /* reconfigure ckout0 */
            rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS);
        }
    }
}

/*!
    \brief      configure leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* PMU clock enable */
    rcu_periph_clock_enable(RCU_PMU);
}

/*!
    \brief      configure keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    /* wakeup key config */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    /* tamper key EXTI config */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    /* user key config */
    gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);
}

/*!
    \brief      configure the ckout0 to measure the system clock on PA8
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ckout0_config(void)
{
    /* enable the PA8 clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure ckout0 GPIO port */ 
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    /* configure ckout0 source in RCU */
    rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS);
}

/*!
    \brief      toggle the led
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0;

    if (timingdelaylocal != 0x00){
        /* all the leds on */
        if(timingdelaylocal < 200){
            gd_eval_led_on(LED2);
            gd_eval_led_on(LED3);
            gd_eval_led_on(LED4);
            gd_eval_led_on(LED5);
        }else{
            /* all the leds off */
            gd_eval_led_off(LED2);
            gd_eval_led_off(LED3);
            gd_eval_led_off(LED4);
            gd_eval_led_off(LED5);
        }
        timingdelaylocal--;
    }else{
        timingdelaylocal = 400;
    }
}
