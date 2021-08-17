/*!
    \file  main.c
    \brief TIMER1 oc active demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F30x
*/

#include "gd32f30x.h"
#include <stdio.h>
#include "gd32f30x_eval.h"

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

    /*Configure PA0 PA1 PA2(TIMER1 CH0 CH1 CH2) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* ---------------------------------------------------------------
    TIMER1 Configuration: 
    TIMER1CLK = SystemCoreClock / 12000 = 10K,
    And generate 3 signals with 3 different delays:
    TIMER1_CH0 delay = 4000/10000  = 0.4s
    TIMER1_CH1 delay = 8000/10000  = 0.8s
    TIMER1_CH2 delay = 12000/10000 = 1.2s
    --------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 11999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH0,CH1 and CH2 configuration in OC active mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocintpara);

    /* CH0 configuration in OC active mode */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,3999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_ACTIVE);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* CH1 configuration in OC active mode */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,7999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_ACTIVE);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* CH2 configuration in OC active mode */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,11999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_ACTIVE);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_enable(TIMER1);
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
