/*!
    \file  main.c
    \brief USART printf

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

#include "gd32f1x0.h"
#include <stdlib.h>
#include <stdio.h>
#include "gd32f190r_eval.h"
#include "systick.h"

void test_status_led_init(void);
void led_flash(int times);
void test_key_init(void);

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
    /* init test status led */ 
    test_status_led_init();
    /* flash led for test */
    led_flash(1);
    /* test key init */ 
    test_key_init();
    /* USART initialize */
    gd_eval_com_init(EVAL_COM2);
    /* output a message on hyperterminal using printf function */
    printf("\n\rplease press the Wakeup key \n\r");

    /* the software must wait until TC=1, the TC flag remains cleared during all data
    transfers and it is set by hardware at the last frame end of transmission*/
    while (RESET == usart_flag_get(EVAL_COM2, USART_FLAG_TC));

    while (1){
        if(0 == gd_eval_key_state_get(KEY_WAKEUP)){
            delay_1ms( 50 );

            if(0 == gd_eval_key_state_get(KEY_WAKEUP)){
                delay_1ms( 50 );

                if(0 == gd_eval_key_state_get(KEY_WAKEUP)){
                    /* turn on LED1 */
                    gd_eval_led_on(LED1);

                    /* output a message on hyperterminal using printf function */
                    printf("\n\rUSART printf example \n\r");

                    /* the software must wait until TC=1. the TC flag remains cleared during all data
                    transfers and it is set by hardware at the last frames end of transmission*/
                    while (RESET == usart_flag_get(EVAL_COM2, USART_FLAG_TC));
                    
                    /* wait until the button is released */
                    while(0 == gd_eval_key_state_get(KEY_WAKEUP));
                }else{
                    /* turn off LED1 */
                    gd_eval_led_off(LED1);
                }
            }else{
                /* turn off LED1 */
                gd_eval_led_off(LED1);
            }
        }else{
            /* turn off LED1 */
            gd_eval_led_off(LED1);
        }
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM2, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM2, USART_FLAG_TC));
    return ch;
}

/*!
    \brief      test status led init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_status_led_init(void)
{
    /*LEDs initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/*!
    \brief      led blink function
    \param[in]  times: the blink times of LEDs
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for(i = 0;i < times;i ++){
        /* insert 400 ms delay */
        delay_1ms(400);

        /* turn on LEDs */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* insert 400 ms delay */
        delay_1ms(400);

        /* turn off LEDs */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}

/*!
    \brief      test key init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_key_init( void )
{
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
}
