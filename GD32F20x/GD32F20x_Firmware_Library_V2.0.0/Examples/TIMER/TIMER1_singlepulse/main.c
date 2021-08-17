/*!
    \file  main.c
    \brief TIMER1 single pulse demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include <stdio.h>
#include "gd32f207i_eval.h"

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

    /*configure PA0 PA1(TIMER1 CH0 CH1) as alternate function*/
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER1 configuration: single pulse mode
    the external signal is connected to TIMER1 CH0 pin (PA0) and the falling 
    edge is used as active edge.
    the single pulse signal is output on TIMER1 CH1 pin (PA1).

    the TIMER1CLK frequency is set to systemcoreclock,the prescaler is 
    6,so the TIMER1 counter clock is 20MHz.

    single pulse value = (TIMER1_Period - TIMER1_Pulse) / TIMER1 counter clock
                       = (65535 - 11535) / 20MHz = 2.7 ms.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 5;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* auto-reload preload disable */
    timer_auto_reload_shadow_disable(TIMER1);

    /* CH1 configuration in OC PWM2 mode */
    timer_ocinitpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocinitpara);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,11535);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* TIMER1 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_FALLING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter = 0x00;
    timer_input_capture_config(TIMER1,TIMER_CH_0,&timer_icinitpara);

    /* single pulse mode selection */
    timer_single_pulse_mode_config(TIMER1,TIMER_SP_MODE_SINGLE);

    /* slave mode selection : TIMER1 */
    /* TIMER1 input trigger : external trigger connected to CI0 */
    timer_input_trigger_source_select(TIMER1,TIMER_SMCFG_TRGSEL_CI0FE0);
    timer_slave_mode_select(TIMER1,TIMER_SLAVE_MODE_EVENT);
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
