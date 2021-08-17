/*!
    \file    main.c
    \brief   TIMER2 oc inactive demo for gd32f3x0

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
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


#include "gd32f3x0.h"
#include <stdio.h>
#include "gd32f350r_eval.h"

/* configure the GPIO ports */
void gpio_config(void);
/* configure the TIMER interrupt */
void nvic_config(void);
/* configure the TIMER peripheral */
void timer_config(void);

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    /* enable the GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable the GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /*configure PB0(TIMER2 CH2) as alternate function*/
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_0);
    
    /*configure PA7(TIMER2 CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_7);

    /*configure PA6(TIMER2 CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
}

/**
    \brief      configure the TIMER interrupt
    \param[in]  none
    \param[out] none
    \retval     none
  */
void nvic_config(void)
{
    nvic_irq_enable(TIMER2_IRQn, 0,0);
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
    TIMER2CLK = SystemCoreClock / 36000 = 2KHz,
    And generate 3 signals with 3 different delays:
    TIMER2_CH0 delay = 2000/2000 = 1s
    TIMER2_CH1 delay = 4000/2000 = 2s
    TIMER2_CH2 delay = 6000/2000 = 3s
    --------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    /* enable the TIMER clock */
    rcu_periph_clock_enable(RCU_TIMER2);
    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
#ifdef GD32F330
    timer_initpara.prescaler         = 41999;
#endif /* GD32F330 */
#ifdef GD32F350
    timer_initpara.prescaler         = 53999;
#endif /* GD32F350 */
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 10000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER2, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_config(TIMER2, TIMER_CH_2, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 2000);
    /* CH0 configuration in OC inactive mode */
    timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_INACTIVE);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, 4000);
    /* CH1 configuration in OC inactive mode */
    timer_channel_output_mode_config(TIMER2, TIMER_CH_1, TIMER_OC_MODE_INACTIVE);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_2, 6000);
    /* CH2 configuration in OC inactive mode */
    timer_channel_output_mode_config(TIMER2, TIMER_CH_2, TIMER_OC_MODE_INACTIVE);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* enable the TIMER channel0/1/2 interrupt */
    timer_interrupt_enable(TIMER2, TIMER_INT_CH0 | TIMER_INT_CH1 | TIMER_INT_CH2);

    /* turn on selected led */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* enable a TIMER */
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
    /* configure led GPIO */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    
    /* configure the GPIO ports */
    gpio_config();
    /* configure the TIMER interrupt */
    nvic_config();
    /* configure the TIMER peripheral */
    timer_config();

    while (1);
}
