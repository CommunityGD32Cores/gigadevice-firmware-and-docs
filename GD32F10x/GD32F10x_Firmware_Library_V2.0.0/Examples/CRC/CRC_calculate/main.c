/*!
    \file  main.c
    \brief CRC calculate demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "gd32f10x.h"
#include "gd32f10x_eval.h"

uint32_t val = 0, valcrc = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    val = (uint32_t)0xabcd1234;

    /* initialize the LED1 and LED2, turn off them */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    rcu_periph_clock_enable(RCU_CRC);
    
    crc_data_register_reset();
    valcrc = crc_single_data_calculate(val);

    if(0xf7018a40 == valcrc){
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
    }

    while (1){
    }
}
