/*!
    \file    gd32e50x_usbd_hw.c
    \brief   USB device hardware configuration

    \version 2020-09-04, V1.0.0, demo for GD32E50x
    \version 2021-03-31, V1.1.0, demo for GD32E50x
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

#define TIM_MSEC_DELAY                          0x01U
#define TIM_USEC_DELAY                          0x02U

__IO uint32_t delay_time = 0U;
__IO uint16_t timer_prescaler = 5U;
uint32_t usb_prescaler = 0U;

/* local function prototypes ('static') */
static void hw_time_set (uint8_t unit);
static void hw_delay    (uint32_t ntime, uint8_t unit);

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config (void)
{
    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);

    if (system_clock == 48000000U) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV1;
        timer_prescaler = 3U;
    } else if (system_clock == 72000000U) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
        timer_prescaler = 5U;
    } else if (system_clock == 120000000U) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV2_5;
        timer_prescaler = 9U;
    } else if (system_clock == 168000000U) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV3_5;
        timer_prescaler = 11U;
    } else {
    }
    /* enable USB pull-up pin clock */ 
    rcu_periph_clock_enable(RCU_AHBPeriph_GPIO_PULLUP);

    /* enable the power clock */
    rcu_periph_clock_enable(RCU_PMU);

    rcu_periph_clock_enable(RCU_TIMER2);

#ifndef USE_IRC48M
    /* configure USB model clock from PLL clock */
    rcu_usb_clock_config(usb_prescaler);
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

    /* enable the TIMER2 gloabal interrupt */
    nvic_irq_enable((uint8_t)TIMER2_IRQn, 1U, 0U);
}


/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay (const uint32_t usec)
{
    hw_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milli seconds
    \param[in]  msec: value of delay required in milli seconds
    \param[out] none
    \retval     none
*/
void usb_mdelay (const uint32_t msec)
{
    hw_delay(msec, TIM_MSEC_DELAY);
}

/*!
    \brief      time base IRQ
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_irq (void)
{
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_UP) != RESET){
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

        if (delay_time > 0x00U){
            delay_time--;
        } else {
            timer_disable(TIMER2);
        }
    }
}

/*!
    \brief      delay routine based on TIMER2
    \param[in]  nTime: delay Time 
    \param[in]  unit: delay Time unit = mili sec / micro sec
    \param[out] none
    \retval     none
*/
static void hw_delay(uint32_t ntime, uint8_t unit)
{
    delay_time = ntime;

    hw_time_set(unit);

    while (0U != delay_time) {
    }

    timer_disable(TIMER2);
}

/*!
    \brief      configures TIMER2 for delay routine based on TIMER2
    \param[in]  unit: msec /usec
    \param[out] none
    \retval     none
*/
static void hw_time_set(uint8_t unit)
{
    timer_parameter_struct  timer_basestructure;

    timer_disable(TIMER2);
    timer_interrupt_disable(TIMER2, TIMER_INT_UP);

    if (unit == TIM_USEC_DELAY) {
        timer_basestructure.period = 6U;
    } else if(unit == TIM_MSEC_DELAY) {
        timer_basestructure.period = 6999U;
    } else {
        /* no operation */
    }

    timer_basestructure.prescaler         = timer_prescaler;
    timer_basestructure.alignedmode       = TIMER_COUNTER_EDGE;
    timer_basestructure.counterdirection  = TIMER_COUNTER_UP;
    timer_basestructure.clockdivision     = TIMER_CKDIV_DIV1;
    timer_basestructure.repetitioncounter = 0U;

    timer_init(TIMER2, &timer_basestructure);

    timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

    timer_auto_reload_shadow_enable(TIMER2);

    /* TIMER IT enable */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    /* TIMER2 enable counter */ 
    timer_enable(TIMER2);
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
    /* configure CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_LXTAL);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* configure hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);

    /* configure CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0x05B8);
    /* configure clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x0001);

    /* CTC counter enable */
    ctc_counter_enable();
}

#endif
