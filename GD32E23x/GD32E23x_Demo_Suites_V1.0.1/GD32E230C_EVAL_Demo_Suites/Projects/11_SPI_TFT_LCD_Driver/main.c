/*!
    \file  main.c
    \brief SPI LCD test
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32e23x.h"
#include "gd32e230c_lcd_eval.h"
#include "tft_test.h"
#include "systick.h"

void test_status_led_init(void);
void flash_led(uint32_t times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure the systick */
    systick_config();
    /* initialize test status led */ 
    test_status_led_init();
    /* flash led for test */
    flash_led(1);

    /* initialize lcd */
    lcd_init();
  
    while(1){
        /* clear the lcd to blue*/
        lcd_clear(BLUE);
        /* font display test*/
        font_test();
        /* num display test*/
        num_test();
        /* draw display test*/
        draw_test();
        /* color display test*/
        color_test();
    }
}

/*!
    \brief      test status led initialize
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_status_led_init(void)
{
    /* initialize the leds */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* close all of leds */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      flash leds
    \param[in]  times: leds blink times
    \param[out] none
    \retval     none
*/
void flash_led(uint32_t times)
{
    int i;

    for(i = 0;i < times;i ++){
        /* insert 200 ms delay */
         delay_ms(200);

        /* turn on leds */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* insert 200 ms delay */
         delay_ms(200);

        /* turn off leds */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}
