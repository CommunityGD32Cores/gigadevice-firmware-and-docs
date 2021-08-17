/*!
    \file  main.c
    \brief TIMER2 external trigger demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"

void gpio_config(void);
void timer_config(void);

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);

    /*configure PA7(TIMER2 CH1) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_7);

    /*configure PA6(TIMER2 CH0) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* Timer with an external trigger -------------------------------------
    TIMER2 is configured as slave timer for an external trigger connected
    to TIMER2 CI0 pin :
    - The TIMER2 CI0FE0 is used as trigger input
    - rising edge is used to start the TIMER2: event mode.
    - TIMER2 CH1 is used PWM0 Mode 
    the starts of the TIMER2 counter are controlled by the 
    external trigger.  
    -------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;

    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);

    /* TIMER2 configuration */
    timer_initpara.prescaler         = 167;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 7999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

     /* TIMER2 CH2 configuration in OC PWM0 mode */
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2,TIMER_CH_1,&timer_ocinitpara);

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,3999);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x02;
    timer_input_capture_config(TIMER2,TIMER_CH_0,&timer_icinitpara);

    /* slave mode selection : event mode */
    /* TIMER2 input trigger : external trigger connected to CI0 */
    timer_input_trigger_source_select(TIMER2,TIMER_SMCFG_TRGSEL_CI0FE0);
    timer_slave_mode_select(TIMER2,TIMER_SLAVE_MODE_EVENT);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gpio_config();
    timer_config();

    while (1);
}
