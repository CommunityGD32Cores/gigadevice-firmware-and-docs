/*!
    \file  main.c
    \brief TIMERs cascade synchro demo
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
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*configure PA6(TIMER2 CH0) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);

    /*configure PB7(TIMER3 CH1) as alternate function*/
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_7);

    /*configure PA8(TIMER0 CH0) as alternate function*/
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* timers synchronisation in cascade mode ----------------------------
       1/TIMER3 is configured as master timer:
       - PWM mode is used
       - The TIMER3 update event is used as trigger output  

       2/TIMER2 is slave for TIMER3 and master for TIMER0,
       - PWM mode is used
       - The ITR3(TIMER3) is used as input trigger 
       - external clock mode is used,the counter counts on the rising edges of
         the selected trigger.
       - the TIMER2 update event is used as trigger output. 

       3/TIMER0 is slave for TIMER2,
       - PWM mode is used
       - The ITR2(TIMER2) is used as input trigger
       - external clock mode is used,the counter counts on the rising edges of
         the selected trigger.
      -------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    rcu_periph_clock_enable(RCU_TIMER3);
    rcu_periph_clock_enable(RCU_TIMER2);

    /* TIMER1  configuration */
    timer_deinit(TIMER3);

    timer_initpara.prescaler         = 8399;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 3999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3,&timer_initpara);

    /* CH1 configuration in PWM0 mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER3,TIMER_CH_1,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_1,1999);
    timer_channel_output_mode_config(TIMER3,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER3);
    /* select the master slave mode */
    timer_master_slave_mode_config(TIMER3,TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* TIMER1 update event is used as trigger output */
    timer_master_output_trigger_source_select(TIMER3,TIMER_TRI_OUT_SRC_UPDATE);

    /* TIMER2  configuration */
    timer_deinit(TIMER2);

    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

    /* CH0 configuration in PWM0 mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,1);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
    /* slave mode selection: TIMER2 */
    timer_slave_mode_select(TIMER2,TIMER_SLAVE_MODE_EXTERNAL0);
    timer_input_trigger_source_select(TIMER2,TIMER_SMCFG_TRGSEL_ITI3);
    /* select the master slave mode */
    timer_master_slave_mode_config(TIMER2,TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* TIMER2 update event is used as trigger output */
    timer_master_output_trigger_source_select(TIMER2,TIMER_TRI_OUT_SRC_UPDATE);

    /* TIMER0 configuration */
    timer_deinit(TIMER0);

    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

    /* CH0 configuration in PWM0 mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,1);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* TIMER0 output enable */
    timer_primary_output_config(TIMER0,ENABLE);
    /* slave mode selection: TIMER0 */
    timer_slave_mode_select(TIMER0,TIMER_SLAVE_MODE_EXTERNAL0);
    timer_input_trigger_source_select(TIMER0,TIMER_SMCFG_TRGSEL_ITI2);

    /* TIMER counter enable */
    timer_enable(TIMER3);
    timer_enable(TIMER2);
    timer_enable(TIMER0);
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
