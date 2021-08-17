/*!
    \file  main.c
    \brief led spark with systick 
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "gd32f30x.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f30x_eval.h"

void gpio_configuration(void);
void timer_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    dbg_periph_disable(DBG_TIMER1_HOLD);

    gpio_configuration();
    gd_eval_led_init(LED1);
    systick_config();
    timer_configuration();

    /* keep timer1 counter when the mcu is in debug mode */
    dbg_periph_enable(DBG_TIMER1_HOLD);
  
    while (1){
        /* toggle LED1 output status every second */
        gd_eval_led_toggle(LED1);
        /* you can set breakpoint here, then look over the register in timer1 */
        delay_1ms(1000);
    }
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_configuration(void)
{
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, ENABLE);
    /* configure PB3 PB10 PB11(TIMER1 CH1 CH2 CH3) as alternate function */
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_11);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_configuration(void)
{
    
    /* TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = SystemCoreClock / 168 = 1MHz

    TIMER1 channel 1 duty cycle = 4000/ 16000  = 25%
    TIMER1 channel 2 duty cycle = 8000/ 16000  = 50%
    TIMER1 channel 3 duty cycle = 12000/ 16000 = 75% */

    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocintpara;
  
    rcu_periph_clock_enable(RCU_TIMER1);

    /* TIMER1  configuration */
    timer_deinit(TIMER1);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 167;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 15999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

     /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_3,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,3999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,7999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,11999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);
}
