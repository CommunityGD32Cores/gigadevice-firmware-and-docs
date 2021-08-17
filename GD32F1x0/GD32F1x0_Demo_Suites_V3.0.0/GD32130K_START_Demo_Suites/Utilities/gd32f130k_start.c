 /*!
    \file  gd32f130k_start.c
    \brief firmware functions to manage leds, keys, COM ports

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32f130k_start.h"

/* private variables */
static const uint32_t GPIO_PORT[LEDn]       = {LED1_GPIO_PORT,
                                               LED2_GPIO_PORT};

static const uint32_t GPIO_PIN[LEDn]        = {LED1_PIN,
                                               LED2_PIN};

static const rcu_periph_enum GPIO_CLK[LEDn] = {LED1_GPIO_CLK,
                                               LED2_GPIO_CLK};

static const uint32_t KEY_PORT[KEYn]        = {USER_KEY_GPIO_PORT};

static const uint32_t KEY_PIN[KEYn]         = {USER_KEY_PIN};

static const rcu_periph_enum KEY_CLK[KEYn]  = {USER_KEY_GPIO_CLK};

static const exti_line_enum KEY_EXTI_LINE[KEYn] = {USER_KEY_EXTI_LINE};

static const uint8_t KEY_PORT_SOURCE[KEYn]      = {USER_KEY_EXTI_PORT_SOURCE};

static const uint8_t KEY_PIN_SOURCE[KEYn]       = {USER_KEY_EXTI_PIN_SOURCE};

static const uint8_t KEY_IRQn[KEYn]             = {USER_KEY_EXTI_IRQn};

/* eval board low layer private functions */
/*!
    \brief      configure led GPIO
    \param[in]  lednum: specify the led to be configured
                only one parameter can be selected which is shown as below:
      \arg        LED1
      \arg        LED2
    \param[out] none
    \retval     none
*/
void gd_eval_led_init(led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
    /* configure led GPIO port */ 
    gpio_mode_set(GPIO_PORT[lednum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN[lednum]);
    gpio_output_options_set(GPIO_PORT[lednum], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN[lednum]);

    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn on selected led
    \param[in]  lednum: specify the led to be turned on
                only one parameter can be selected which is shown as below:
      \arg        LED1
      \arg        LED2
    \param[out] none
    \retval     none
*/
void gd_eval_led_on(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn off selected led
    \param[in]  lednum: specify the led to be turned off
                only one parameter can be selected which is shown as below:
      \arg        LED1
      \arg        LED2
    \param[out] none
    \retval     none
*/
void gd_eval_led_off(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      toggle selected led
    \param[in]  lednum: specify the led to be toggled
                only one parameter can be selected which is shown as below:
      \arg        LED1
      \arg        LED2
    \param[out] none
    \retval     none
*/
void gd_eval_led_toggle(led_typedef_enum lednum)
{
    gpio_bit_write(GPIO_PORT[lednum], GPIO_PIN[lednum], (bit_status)((1-gpio_output_bit_get(GPIO_PORT[lednum], GPIO_PIN[lednum]))));
}

/*!
    \brief      configure key
    \param[in]  keynum: specify the key to be configured
                only one parameter can be selected which is shown as below:
      \arg        KEY_USER: Wakeup key
    \param[in]  keymode: specify button mode
                only one parameter can be selected which is shown as below:
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode)
{
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[keynum]);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure button pin as input */
    gpio_mode_set(KEY_PORT[keynum], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY_PIN[keynum]);

    if (keymode == KEY_MODE_EXTI){
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(KEY_IRQn[keynum], 2U, 0U);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);

        /* configure key EXTI line */
        exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
    }
}

/*!
    \brief      return the selected key state
    \param[in]  keynum: specify the key to be checked
                only one parameter can be selected which is shown as below:
      \arg        KEY_USER: Wakeup key
    \param[out] none
    \retval     the key's GPIO pin value
*/
uint8_t gd_eval_key_state_get(key_typedef_enum keynum)
{
    return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
}
