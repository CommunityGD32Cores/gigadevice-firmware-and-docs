/*!
    \file  main.c
    \brief Comparator obtain brightness

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
#include "gd32f150r_eval.h"
#include "systick.h"

void led_config(void);
void led_flash(int times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{

    systick_config();

    /* configure LEDs */ 
    led_config();

    /* flash LED for test */
    led_flash(1);

    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable comparator clock */
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);

    /* configure PA1 as comparator input */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_1);

    /* configure CMP0 */
    cmp_mode_init(CMP0, CMP_HIGHSPEED, CMP_DAC0, CMP_HYSTERESIS_NO);
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);

    /* enable CMP0 */
    cmp_enable(CMP0);

    dac0_trigger_source_config(DAC_TRIGGER_SOFTWARE);
    dac0_output_buffer_enable();

    /* enable DAC */
    dac0_enable();

    /* set DAC channel0 DAC0_L12DH register: DAC_OUT2 = (3.3 * 3000) / 4095 ~ 2.4 V */
    dac0_data_set(DAC_ALIGN_12B_R, (uint16_t)(3000));

    delay_1ms(5);

    /* start DAC channel0 conversion by software */
    dac0_software_trigger_enable();

    /* infinite loop */
    while(1){
        /* get the output level */
        if(CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get(CMP0)){
            gd_eval_led_on(LED1);
        }else{
            gd_eval_led_off(LED1);
        }
    }
}

/*!
    \brief      configure the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config( void )
{
    gd_eval_led_init(LED1);   
    gd_eval_led_init(LED2);    
    gd_eval_led_init(LED3);   
    gd_eval_led_init(LED4);

}

/*!
    \brief      test status LED
    \param[in]  times: specifies the delay time length, in milliseconds
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for(i = 0; i < times; i++){
        /* insert 200 ms delay */
        delay_1ms(200);
    
        /* turn on LED */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);
        
        /* insert 200 ms delay */
        delay_1ms(200);
  
        /* turn off LED */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}
