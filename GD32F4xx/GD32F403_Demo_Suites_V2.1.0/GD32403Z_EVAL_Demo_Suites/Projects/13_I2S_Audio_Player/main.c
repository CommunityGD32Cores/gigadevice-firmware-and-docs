/*!
    \file  main.c
    \brief I2S audio player demo

    \version 2017-02-10, V1.0.0, demo for GD32F403
    \version 2018-10-10, V1.1.0, demo for GD32F403
    \version 2018-12-25, V2.0.0, demo for GD32F403
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

#include <stdio.h>
#include "gd32f403.h"
#include "systick.h"
#include "gd32f403z_eval.h"
#include "pcm1770.h"

void test_status_led_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick configuration*/
    systick_config();

    /* configure the led GPIO */
    test_status_led_init();

    nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
    nvic_irq_enable(SPI1_IRQn, 0, 1);

    /* I2S code test */
    {  
        /* initialize SPI, I2S and audio file */
        pcm1770_init(AUDIO_FILE_ADDRESS );
        /* audio play start */
        pcm1770_audio_play();
    }

    while(1);
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
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);

    /* turn on all of leds */
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED4);
    gd_eval_led_on(LED5);
    
    delay_1ms(500);
    
    /* close all of leds */
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
    gd_eval_led_off(LED5);
}
