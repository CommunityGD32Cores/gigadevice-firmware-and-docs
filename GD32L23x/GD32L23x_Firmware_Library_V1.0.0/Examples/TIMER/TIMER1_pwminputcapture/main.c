/*!
    \file    main.c
    \brief   TIMER1 PWM input capture demo

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "gd32l23x.h"
#include <stdio.h>
#include "systick.h"
#include "gd32l233r_eval.h"

extern __IO uint16_t dutycycle;
extern __IO uint16_t frequency;
void gpio_config(void);
void timer_config(void);
void nvic_config(void);
int fputc(int ch, FILE *f);

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t) ch);
    while(usart_flag_get(EVAL_COM, USART_FLAG_TBE) == RESET);
    return ch;
}
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    systick_config();
    gpio_config();
    nvic_config();
    timer_config();
    gd_eval_com_init(EVAL_COM);

    while(1) {
        delay_1ms(1000);
        printf("\r /**** TIMER1 PWM Input Capture Demo ****/\r\n");
        printf("the dutycycle is %d\n", dutycycle);
        printf("the frequence is %d\n", frequency);
    }
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
    /* TIMER1 GPIO */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_0);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(TIMER1_IRQn, 0);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* TIMER1 configuration: PWM input mode ------------------------
        the external signal is connected to TIMER1 CH0 pin(PA0)
        the rising edge is used as active edge
        the TIMER1 CH0CV is used to compute the frequency value
        the TIMER1 CH1CV is used to compute the duty cycle value
     ------------------------------------------------------------ */
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);

    /* deinit a TIMER */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler        = 63;
    timer_initpara.alignedmode      = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period           = 65535;
    timer_initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* TIMER1 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter = 0x00;
    timer_input_pwm_capture_config(TIMER1, TIMER_CH_0, &timer_icinitpara);
    /* slave mode selection: TIMER1 */
    timer_input_trigger_source_select(TIMER1, TIMER_SMCFG_TRGSEL_CI0FE0);
    timer_slave_mode_select(TIMER1, TIMER_SLAVE_MODE_RESTART);
    /* select the master slave mode */
    timer_master_slave_mode_config(TIMER1, TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_CH0);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(TIMER1, TIMER_INT_CH0);

    /* enable a TIMER */
    timer_enable(TIMER1);
}
