/*!
    \file  main.c
    \brief TIMER2 oc active demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
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
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*Configure PA6 PA7 PB0(TIMER2 CH0 CH1 CH2) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_7);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0);
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
    TIMER2 Configuration: 
    TIMER2CLK = SystemCoreClock / 16800 = 10K,
    And generate 3 signals with 3 different delays:
    TIMER2_CH0 delay = 4000/10000  = 0.4s
    TIMER2_CH1 delay = 8000/10000  = 0.8s
    TIMER2_CH2 delay = 12000/10000 = 1.2s
    --------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);
    timer_deinit(TIMER2);

    /* TIMER2 configuration */
    timer_initpara.prescaler         = 16799;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

    /* TIMER2 CH0,CH1 and CH2 configuration in OC active mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER2,TIMER_CH_1,&timer_ocintpara);
    timer_channel_output_config(TIMER2,TIMER_CH_2,&timer_ocintpara);

    /* TIMER2 CH0 configuration in OC active mode */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,3999);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_ACTIVE);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* TIMER2 CH1 configuration in OC active mode */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,7999);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_1,TIMER_OC_MODE_ACTIVE);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* TIMER2 CH2 configuration in OC active mode */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_2,11999);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_2,TIMER_OC_MODE_ACTIVE);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_enable(TIMER2);
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
