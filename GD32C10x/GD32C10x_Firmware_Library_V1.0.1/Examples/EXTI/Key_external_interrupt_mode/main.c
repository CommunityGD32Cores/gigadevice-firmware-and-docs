/*!
    \file    main.c
    \brief   the example of EXTI which generates an interrupt request and toggle the LED

    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#include "gd32c10x.h"

void key_wakeup_interrupt_config(void);
void key_temper_interrupt_config(void);
void key_user_interrupt_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable GPIOE clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    
    /* configure the leds */
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    
    /* turn on leds */
    gpio_bit_set(GPIOC,GPIO_PIN_0);
    gpio_bit_set(GPIOC,GPIO_PIN_2);
    gpio_bit_set(GPIOE,GPIO_PIN_0);
    gpio_bit_set(GPIOE,GPIO_PIN_1);

    /* configure the keys in interrupt mode*/
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    key_wakeup_interrupt_config();
    key_temper_interrupt_config();
    key_user_interrupt_config();

    while(1){
	}
}

/*!
    \brief      configure the key wakeup(PA0) in interrupt mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_wakeup_interrupt_config(void)
{
    /* enable the key wakeup clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure button pin as input */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    
    /* enable and set key wakeup EXTI interrupt to the lowest priority */
    nvic_irq_enable(EXTI0_IRQn, 2U, 0U);

    /* connect key wakeup EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0);

    /* configure key wakeup EXTI line */
    exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_0);
}

/*!
    \brief      configure the key temper(PC13) in interrupt mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_temper_interrupt_config(void)
{
    /* enable the key temper clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure button pin as input */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    
    /* enable and set key temper EXTI interrupt to the lowest priority */
    nvic_irq_enable(EXTI10_15_IRQn, 2U, 1U);

    /* connect key temper EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_13);

    /* configure key temper EXTI line */
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_13);
}

/*!
    \brief      configure the key user1(PC5) in interrupt mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_user_interrupt_config(void)
{
    /* enable the key user clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure button pin as input */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    
    /* enable and set key user EXTI interrupt to the lowest priority */
    nvic_irq_enable(EXTI10_15_IRQn, 2U, 1U);

    /* connect key user EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_14);

    /* configure key user EXTI line */
    exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_14);
}
