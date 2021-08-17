/*!
    \file  main.c
    \brief keyboard polling mode
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"
#include "systick.h"
#include "gd32f3x0_eval.h"

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
    gd_eval_key_init(KEY_TAMPER,KEY_MODE_GPIO);

    while(1){
        /* check whether the button is pressed */
        if(RESET == gd_eval_key_state_get(KEY_TAMPER)){
            delay_1ms(100);
            /* check whether the button is pressed */
            if(RESET == gd_eval_key_state_get(KEY_TAMPER)){
                gd_eval_led_toggle(LED1);
            }
        }
    }
}
