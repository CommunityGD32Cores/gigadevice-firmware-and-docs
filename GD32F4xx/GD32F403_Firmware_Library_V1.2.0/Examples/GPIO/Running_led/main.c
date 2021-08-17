/*!
    \file  main.c
    \brief running led
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include "gd32f403_eval.h"
#include "systick.h"
#include <stdio.h>

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

    while(1){
        /* turn on led1, turn off led4 */
        gd_eval_led_on(LED1);
        gd_eval_led_off(LED4);
        delay_1ms(1000);
        /* turn on led2, turn off led1 */
        gd_eval_led_on(LED2);
        gd_eval_led_off(LED1);
        delay_1ms(1000);
        /* turn on led3, turn off led2 */
        gd_eval_led_on(LED3);
        gd_eval_led_off(LED2);
        delay_1ms(1000);
        /* turn on led4, turn off led3 */
        gd_eval_led_on(LED4);
        gd_eval_led_off(LED3);
        delay_1ms(1000);
    }
}
