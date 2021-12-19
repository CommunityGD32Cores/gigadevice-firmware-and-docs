/*!
    \file    main.c
    \brief   Deepsleep wakeup

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
#include "gd32l233r_eval.h"
#include "systick.h"

extern __IO uint8_t counter0;

static void system_clock_reconfig(void);
void com_lpuart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();

    gd_eval_led_init(LED1);

    /* configure the CK_IRC16M as LPUART clock */
    rcu_lpuart_clock_config(RCU_LPUARTSRC_IRC16MDIV);
    com_lpuart_init();

    nvic_irq_enable(LPUART_WKUP_IRQn, 0);
    exti_init(EXTI_28, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    delay_1ms(2000);

    /* use start bit wakeup MCU */
    lpuart_wakeup_mode_config(LPUART_WUM_STARTB);
    lpuart_enable();
    /* ensure LPUART is enabled */
    while(RESET == lpuart_flag_get(LPUART_FLAG_REA)) {
    }
    /* check LPUART is not transmitting */
    while(SET == lpuart_flag_get(LPUART_FLAG_BSY)) {
    }

    lpuart_wakeup_enable();
    /* enable the WUIE interrupt */
    lpuart_interrupt_enable(LPUART_INT_WU);

    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enter deep-sleep mode */
    pmu_to_deepsleepmode(PMU_LDNPDSP_LOWDRIVE, WFI_CMD, PMU_DEEPSLEEP);

    /* wait a WUIE interrupt event */
    while(0x00 == counter0) {
    }

    /* disable LPUART peripheral in deepsleep mode */
    lpuart_wakeup_disable();

    while(RESET == lpuart_flag_get(LPUART_FLAG_RBNE));
    lpuart_data_receive();

    lpuart_receive_config(LPUART_RECEIVE_ENABLE);

    while(RESET == lpuart_flag_get(LPUART_FLAG_TC)) {
    }

    /* disable the LPUART */
    lpuart_disable();

    /* reconfigure systemclock */
    system_clock_reconfig();

    systick_config();

    while(1) {
    }
}

/*!
    \brief      configure LPUART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_lpuart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable LPUART clock */
    rcu_periph_clock_enable(RCU_LPUART);

    /* connect port to LPUART TX */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_2);
    /* connect port to LPUART RX */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_3);

    /* configure LPUART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure LPUART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

    /* LPUART configure */
    lpuart_deinit();
    lpuart_word_length_set(LPUART_WL_8BIT);
    lpuart_stop_bit_set(LPUART_STB_1BIT);
    lpuart_parity_config(LPUART_PM_NONE);
    lpuart_baudrate_set(115200U);
    lpuart_receive_config(LPUART_RECEIVE_ENABLE);
    lpuart_transmit_config(LPUART_TRANSMIT_ENABLE);

    lpuart_enable();
}

/*!
    \brief      reconfigure the system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_reconfig(void)
{
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    while(SUCCESS != rcu_osci_stab_wait(RCU_HXTAL)) {
    }

    /* configure AHB */
    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);

    /* configure APB1, APB2 */
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;
}

/*!
    \brief      LED spark
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t time_delay = 0;

    if(0x00 != time_delay) {
        if(time_delay < 500) {
            /* light on */
            gd_eval_led_on(LED1);
        } else {
            /* light off */
            gd_eval_led_off(LED1);
        }
        time_delay--;
    } else {
        time_delay = 1000;
    }
}
