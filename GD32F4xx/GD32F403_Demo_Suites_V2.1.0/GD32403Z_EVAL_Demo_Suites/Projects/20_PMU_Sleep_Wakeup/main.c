/*!
    \file  main.c
    \brief deepsleep wakeup through exti interrupt

    \version 2017-02-10, V1.0.0, firmware for GD32F403
    \version 2018-12-25, V2.0.0, firmware for GD32F403
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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


#include "gd32f403.h"
#include "gd32f403z_eval.h"
#include "main.h"
#include <stdio.h>

void delay(uint32_t count);
void led_config(void);
void led_flash(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* led config */
    led_config();
    /* clock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* USART interrupt configuration */
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    /* enable USART0 receive interrupt */
    usart_interrupt_enable(EVAL_COM0, USART_INT_RBNE);
    printf("Please transmit one byte data from hyperterminal to board");
    pmu_to_sleepmode(WFI_CMD);
    while(1){
        led_flash();
    }
}

/*!
    \brief      toggle the led
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_flash(void)
{
    /* all the leds on */
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED4);
    gd_eval_led_on(LED5);

    delay(6000000);

    /* all the leds off */
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
    gd_eval_led_off(LED5);
    
    delay(6000000);
}

/*!
    \brief      led config
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

void delay(uint32_t count)
{
    while(count--);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
