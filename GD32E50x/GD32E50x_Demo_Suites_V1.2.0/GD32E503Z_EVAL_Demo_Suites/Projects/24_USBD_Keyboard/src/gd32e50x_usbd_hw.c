/*!
    \file    gd32e50x_usbd_hw.c
    \brief   USB device hardware configuration

    \version 2020-06-30, V1.0.0, demo for GD32E50x
    \version 2020-08-26, V1.1.0, demo for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
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

#include "usbd_hw.h"

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config (void)
{
    /* enable USB pull-up pin clock */ 
    rcu_periph_clock_enable(RCU_AHBPeriph_GPIO_PULLUP);

    /* enable the power clock */
    rcu_periph_clock_enable(RCU_PMU);

#ifndef USE_IRC48M
    /* configure USB model clock from PLL clock */
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV3_5);
#else
    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);

    /* wait till IRC48M is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)){
    }

    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
#endif

    /* enable USB APB1 clock */
    rcu_periph_clock_enable(RCU_USBD);
}

/*!
    \brief      configure the gpio peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config (void)
{
    /* configure USB pull-up pin */
    gpio_init(USB_PULLUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, USB_PULLUP_PIN);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config (void)
{
    /* enable the USB low priority interrupt */
    nvic_irq_enable((uint8_t)USBD_LP_CAN0_RX0_IRQn, 2U, 0U);

    /* enable the USB Wake-up interrupt */
    nvic_irq_enable((uint8_t)USBD_WKUP_IRQn, 1U, 0U);
}

#ifdef USE_IRC48M

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config (void)
{
    /* config CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_USBSOF);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* config hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);

    /* config CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0xBB7FU);
    /* config clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x1DU);

    /* CTC counter enable */
    ctc_counter_enable();
}

#endif
