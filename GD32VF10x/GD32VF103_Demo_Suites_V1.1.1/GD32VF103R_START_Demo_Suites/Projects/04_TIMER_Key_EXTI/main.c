/*!
    \file    main.c
    \brief   KeyBoard LED and generate TIMER PWM to toggle LED in IRQ demo
    
    \version 2019-06-05, V1.0.0, demo for GD32VF103
    \version 2020-12-18, V1.1.0, demo for GD32VF103
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103.h"
#include "gd32vf103r_start.h"
#include "systick.h"

void led_flash(int times);
void exti_config(void);
void gpio_config(void);
void timer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize KEY and LED, configure SysTick */
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* flash the LED for test */
    led_flash(1);

    /* configure EXTI, TIMER */
    exti_config();
    gpio_config();
    timer_config();
    
    while(1){
    }
}


/*! 
    \brief      configure the EXTI(PB5)
    \param[in]  none
    \param[out] none
    \retval     none
  */
void exti_config(void)
{
    /* enable the clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    

    /* configure button pin as input */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    /* enable and set key EXTI interrupt to the lowest priority */
    eclic_global_interrupt_enable();
    eclic_irq_enable(EXTI5_9_IRQn, 1, 1);
    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_5);
    /* configure EXTI line */
    exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_5);
}

/*! 
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure PA6(TIMER2 CH0) as alternate function */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER2CLK is 100KHz

    TIMER2 channel0 duty cycle = (25000/ 50000)* 100  = 50%
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);

    /* TIMER configuration */
    timer_initpara.prescaler         = 719;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 49999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

    /* configure CH0 in PWM mode0 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;

    timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,24999);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
    /* auto-reload preload enable */
    timer_enable(TIMER2);
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LED
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for (i = 0;i < times;i++){
        /* delay 500 ms */
        delay_1ms(500);
        /* toggle the LED */
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        /* delay 500 ms */
        delay_1ms(500);
        /* toggle the LED */
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
    }
}
