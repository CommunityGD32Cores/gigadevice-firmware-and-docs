/*!
    \file  main.c
    \brief systick demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "gd32f207i_eval.h"
#include <stdio.h>
#include "systick.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    
    /* turn on led1,led3 */
    gd_eval_led_on(LED1);
    gd_eval_led_on(LED3);

    if(SysTick_Config(SystemCoreClock / 1000)){ 
        while(1){
        }
    }

    while(1){
        /* toggle led2,led4 */
        gd_eval_led_toggle(LED2);
        gd_eval_led_toggle(LED4);

        /* insert 100 ms delay */
        delay_1ms(100);

        /* toggle led1,led3 */
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED3);

        /* insert 200 ms delay */
        delay_1ms(200);
    }
}
