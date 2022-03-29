/*!
    \file    main.c
    \brief   CEC intercommunication

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
    \version 2022-01-06, V2.2.0, firmware for GD32F3x0
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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
#include <stdlib.h>
#include <stdio.h>
#include "gd32f350r_eval.h"
#include "systick.h"

void cec_config(void);

extern uint8_t rcvdata[10];
extern __IO uint8_t rcvstatus;
extern uint8_t rcv_inc;
extern uint8_t bytenum;
__IO uint8_t NUM = 0U;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* delay time initialize */
    systick_config();

    /* configure the CEC peripheral */
    cec_config();

    /* configure the EXTI */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);

    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);

    /* LED1 LED2 initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* main loop */
    while(1) {
        /* wait receive data */
        while(rcvstatus == 0U);

        if(rcvstatus == 1U) {
            if((rcvdata[1] == 0xA5U) && (rcvdata[2] == 0x5AU)) {
                gd_eval_led_on(LED1);
                delay_1ms(50U);
                gd_eval_led_off(LED1);
            }
            if((rcvdata[1] == 0x5AU) && (rcvdata[2] == 0xA5U)) {
                gd_eval_led_on(LED2);
                delay_1ms(50U);
                gd_eval_led_off(LED2);
            }
        }
        /* a reception error occured */
        rcvstatus = 0U;
    }
}

/*!
    \brief      configure the CEC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_config(void)
{
    /* enable clocks */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_CEC);

    /* configure CEC_LINE_GPIO as output open drain */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_8);

    /* configure priority group */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    /* enable the CEC global interrupt (with higher priority) */
    nvic_irq_enable(CEC_IRQn, 0U, 0U);

    /* configure CEC */
    cec_init(CEC_SFT_START_STAOM, CEC_SFT_1POINT5_PERIOD, CEC_OWN_ADDRESS2);
    cec_error_config(CEC_BROADCAST_ERROR_BIT_OFF, CEC_LONG_PERIOD_ERROR_BIT_OFF, CEC_RISING_PERIOD_ERROR_BIT_OFF, CEC_STOP_RISING_ERROR_BIT_OFF);
    cec_reception_tolerance_disable();

    /* activate CEC interrupts associated to the set of TX and RX flags */
    cec_interrupt_enable(CEC_INTEN_TENDIE | CEC_INTEN_TBRIE | CEC_INTEN_RENDIE | CEC_INTEN_BRIE);

    /* activate CEC interrupts associated to the set of TX and RX error */
    cec_interrupt_enable(CEC_INTEN_ROIE | CEC_INTEN_BREIE | CEC_INTEN_BPSEIE | CEC_INTEN_BPLEIE
                         | CEC_INTEN_RAEIE | CEC_INTEN_ARBFIE | CEC_INTEN_TUIE | CEC_INTEN_TERRIE | CEC_INTEN_TAERRIE);

    /* enable CEC */
    cec_enable();
}
