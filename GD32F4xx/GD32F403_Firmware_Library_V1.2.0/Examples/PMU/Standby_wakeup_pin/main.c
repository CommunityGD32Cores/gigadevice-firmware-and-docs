/*!
    \file  main.c
    \brief standby wakeup through wakeup pin
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"

void led_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* clock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* led configuration and turn on all led */
    led_config();
    gd_eval_led_on(LED1);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED4);
    /* tamper key configuration */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    /* wakeup pin enable */
    pmu_wakeup_pin_enable();
    /* press tamper key to enter standby mode and use wakeup key to wakeup mcu */
    while(1){
        if(RESET == gpio_input_bit_get(TAMPER_KEY_GPIO_PORT, TAMPER_KEY_PIN)){
            pmu_to_standbymode(WFI_CMD);
        }
    }
}

/*!
    \brief      configure led 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}
