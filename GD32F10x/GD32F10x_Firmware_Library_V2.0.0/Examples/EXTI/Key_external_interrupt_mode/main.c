/*!
    \file  main.c
    \brief the example of EXTI which generates an interrupt request and toggle the LED
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "gd32f10x.h"
#include "gd32f10x_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* initialize the LEDs and turn on LED1 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_on(LED1);
    
    /* configure the tamper key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);

    while (1);
}
