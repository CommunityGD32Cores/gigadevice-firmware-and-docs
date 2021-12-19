/*!
    \brief   USART printf demo

    \version 2021-08-15, V1.0.0, demo for GD32L23x
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

#include "gd32l23x.h"
#include "gd32l233r_eval.h"
#include "systick.h"
#include <stdio.h>

static void test_status_led_init(void);
static void led_flash(uint8_t times);
static void usart_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LEDs */
    test_status_led_init();

    /* configure systick */
    systick_config();

    /* flash the LEDs for 2 time */
    led_flash(2);

    /* configure USART1 */
    usart_config();

    /* configure TAMPER key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);

    /* output a message on hyperterminal using printf function */
    printf("\r\n USART printf example: please press the Tamper key \r\n");

    /* wait for completion of USART transmission */
    while(RESET == usart_flag_get(USART0, USART_FLAG_TC)) {
    }
    while(1) {
        /* check if the tamper key is pressed */
        if(RESET == gd_eval_key_state_get(KEY_TAMPER)) {
            delay_1ms(50);
            if(RESET == gd_eval_key_state_get(KEY_TAMPER)) {
                delay_1ms(50);
                if(RESET == gd_eval_key_state_get(KEY_TAMPER)) {
                    /* turn on LED2 */
                    gd_eval_led_on(LED2);
                    /* output a message on hyperterminal using printf function */
                    printf("\r\n USART printf example \r\n");
                    /* wait for completion of USART transmission */
                    while(RESET == usart_flag_get(USART1, USART_FLAG_TC)) {
                    }
                } else {
                    /* turn off LED2 */
                    gd_eval_led_off(LED2);
                }
            } else {
                /* turn off LED2 */
                gd_eval_led_off(LED2);
            }
        } else {
            /* turn off LED2 */
            gd_eval_led_off(LED2);
        }
    }
}

/*!
    \brief      test status led initialize
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void test_status_led_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LEDs
    \param[out] none
    \retval     none
*/
static void led_flash(uint8_t times)
{
    uint8_t i;
    for(i = 0; i < times; i++) {
        /* delay 400 ms */
        delay_1ms(400);

        /* turn on LEDs */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* delay 400 ms */
        delay_1ms(400);
        /* turn off LEDs */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}

/*!
    \brief      usart configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void usart_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USART1 TX */
    gpio_af_set(GPIOA, EVAL_COM_AF, GPIO_PIN_2);

    /* connect port to USART1 RX */
    gpio_af_set(GPIOA, EVAL_COM_AF, GPIO_PIN_3);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

    /* USART configure */
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_enable(USART1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t)ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}
