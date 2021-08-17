/*!
    \file  main.c
    \brief CTC is used to trim internal 48MHz RC oscillator with GPIO
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include "gd32f403_eval.h"

void ctc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init led1*/
    gd_eval_led_init(LED1);
    /* enable IRC48M clock */
    RCU_ADDCTL |= RCU_ADDCTL_IRC48MEN; 
    /* wait till IRC48M is ready */
    while(0 == (RCU_ADDCTL & RCU_ADDCTL_IRC48MSTB)){
    }

    /* configure PA8 as external reference signal source input with 32khz */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);
    /* CTC config */
    ctc_config();

    while(1){
        /* if the clock trim is OK */
        if(RESET != ctc_flag_get(CTC_FLAG_CKOK)){
            gd_eval_led_on(LED1);
        }else{
            gd_eval_led_off(LED1);
        }
    }
}

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* config CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_DIV32);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_GPIO);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* config hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);
    
    /* config CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0xBB7F);
    /* config clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x1D);

    /* CTC counter enable */
    ctc_counter_enable();
}
