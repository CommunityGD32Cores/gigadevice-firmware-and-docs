/*!
    \file    gd32f1x0_usbd_hw.c
    \brief   usb device hardware configuration

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

#include "usbd_hw.h"

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);

    /* enable USB pull-up pin clock */
    rcu_periph_clock_enable(RCU_PULLUP);

    if(48000000U == system_clock) {
        rcu_usbd_clock_config(RCU_USBD_CKPLL_DIV1);
    } else if(72000000U == system_clock) {
        rcu_usbd_clock_config(RCU_USBD_CKPLL_DIV1_5);
    } else {
        /* reserved */
    }

    /* enable USB APB1 clock */
    rcu_periph_clock_enable(RCU_USBD);
}

/*!
    \brief      configure USB pull-up GPIO port
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure usb pull-up pin */
    gpio_mode_set(USB_PULLUP, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, USB_PULLUP_PIN);
    gpio_output_options_set(USB_PULLUP, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USB_PULLUP_PIN);

#ifdef USBD_LOWPWR_MODE_ENABLE
    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
#endif /* USBD_LOWPWR_MODE_ENABLE */
}

/*!
    \brief      configure USB interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* 1 bit for preemption priority, 3 bits for sub-priority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    /* enable the USB low priority interrupt */
    nvic_irq_enable(USBD_LP_IRQn, 1U, 0U);

#ifdef USBD_LOWPWR_MODE_ENABLE
    /* enable the USB Wake-up interrupt */
    nvic_irq_enable(USBDWakeUp_IRQChannel, 0U, 0U);
#endif /* USBD_LOWPWR_MODE_ENABLE */

    /* enable TAMPER key EXTI interrupt */
    nvic_irq_enable(TAMPER_KEY_EXTI_IRQn, 0U, 0U);
}
