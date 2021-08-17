/*!
    \file    gd32e50x_usb_hw.c
    \brief   this file implements the board support package for the USB host library

    \version 2020-08-14, V1.0.0, demo for GD32E50x
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

#include "drv_usb_hw.h"

#define TIM_MSEC_DELAY                          0x01U
#define TIM_USEC_DELAY                          0x02U
#define HOST_OVRCURR_PORT                       GPIOC
#define HOST_OVRCURR_LINE                       GPIO_PIN_9
#define HOST_OVRCURR_PORT_SOURCE                GPIO_PORT_SOURCE_GPIOC
#define HOST_OVRCURR_PIN_SOURCE                 GPIO_PINSOURCE9
#define HOST_OVRCURR_PORT_RCC                   RCC_APB2PERIPH_GPIOC
#define HOST_OVRCURR_EXTI_LINE                  EXTI_LINE9
#define HOST_OVRCURR_IRQn                       EXTI9_IRQn

#define HOST_POWERSW_PORT_RCC                   RCU_GPIOC
#define HOST_POWERSW_PORT                       GPIOC
#define HOST_POWERSW_VBUS                       GPIO_PIN_9

#define HOST_SOF_OUTPUT_RCC                     RCC_APB2PERIPH_GPIOA
#define HOST_SOF_PORT                           GPIOA
#define HOST_SOF_SIGNAL                         GPIO_PIN_8

__IO uint32_t delay_time = 0U;
__IO uint32_t usb_prescaler = 0U;
__IO uint32_t timer_prescaler = 5U;

/* local function prototypes ('static') */
static void hwp_time_set         (uint8_t unit);
static void hwp_delay            (uint32_t ntime, uint8_t unit);

/*!
    \brief      configure USB GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_config (void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                                                         GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3);

    /* ULPI_D0(PA3), ULPI_CK(PA5) GPIO pin configuration */
    gpio_afio_port_config(AFIO_PA3_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PA5_USBHS_CFG, ENABLE);

    /* ULPI_D1(PB0), ULPI_D2(PB1), ULPI_D3(PB10), ULPI_D4(PB11) \
       ULPI_D5(PB12), ULPI_D6(PB13) and ULPI_D7(PB5) GPIO pin configuration */
    gpio_afio_port_config(AFIO_PB0_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PB1_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PB5_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PB10_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PB11_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PB12_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PB13_USBHS_CFG, ENABLE);

    /* ULPI_STP(PC0), ULPI_DIR(PC2), ULPI_NXT(PC3) GPIO pin configuration */
    gpio_afio_port_config(AFIO_PC0_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC2_USBHS_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC3_USBHS_CFG, ENABLE);
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config (void)
{
    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);

    if (48000000U == system_clock) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV1;
        timer_prescaler = 3U;
    } else if (72000000U == system_clock) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
        timer_prescaler = 5U;
    } else if (120000000U == system_clock) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV2_5;
        timer_prescaler = 9U;
    } else if (168000000U == system_clock) {
        usb_prescaler = RCU_CKUSB_CKPLL_DIV3_5;
        timer_prescaler = 11U;
    } else {
    }

#ifndef USE_IRC48M
    rcu_usb_clock_config(usb_prescaler);
#else
    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);

    /* wait till IRC48M is ready */
    while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
    }

    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
#endif /* USE_IRC48M */

    rcu_periph_clock_enable(RCU_USBHS);
}

/*!
    \brief      configure USB global interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config (void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)USBHS_IRQn, 2U, 0U);

#ifdef USB_LOW_PWR_MGMT_SUPPORT

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);

#endif /* USB_LOW_PWR_MGMT_SUPPORT */
}

/*!
    \brief      drives the VBUS signal through gpio
    \param[in]  state: VBUS states
    \param[out] none
    \retval     none
*/
void usb_vbus_drive (uint8_t state)
{
    if (0U == state) {
        /* disable is needed on output of the power switch */
        gpio_bit_reset(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    } else {
        /* enable the power switch by driving the enable low */
        gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    }
}

/*!
    \brief      configures the GPIO for the VBUS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_vbus_config (void)
{
    rcu_periph_clock_enable(HOST_POWERSW_PORT_RCC);

    gpio_init(HOST_POWERSW_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HOST_POWERSW_VBUS);

    /* by default, disable is needed on output of the power switch */
    gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);

    /* delay is need for stabilizing the vbus low in reset condition,
     * when vbus=1 and reset-button is pressed by user 
     */
    usb_mdelay (200U);
}

/*!
    \brief      initializes delay unit using Timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_init (void)
{
    /* configure the priority group to 2 bits */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* enable the TIM2 global interrupt */
    nvic_irq_enable((uint8_t)TIMER2_IRQn, 1U, 0U);

    rcu_periph_clock_enable(RCU_TIMER2);
}

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay (const uint32_t usec)
{
    hwp_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milli seconds
    \param[in]  msec: value of delay required in milli seconds
    \param[out] none
    \retval     none
*/
void usb_mdelay (const uint32_t msec)
{
    hwp_delay(msec, TIM_MSEC_DELAY);
}

/*!
    \brief      timer base IRQ
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_irq (void)
{
    if (RESET != timer_interrupt_flag_get(TIMER2, TIMER_INT_UP)) {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

        if (delay_time > 0x00U){
            delay_time--;
        } else {
            timer_disable(TIMER2);
        }
    }
}

/*!
    \brief      delay routine based on TIM2
    \param[in]  ntime: delay Time 
    \param[in]  unit: delay Time unit = mili sec / micro sec
    \param[out] none
    \retval     none
*/
static void hwp_delay(uint32_t ntime, uint8_t unit)
{
    delay_time = ntime;
    hwp_time_set(unit);

    while(0U != delay_time){
    }

    timer_disable(TIMER2);
}

/*!
    \brief      configures TIM2 for delay routine based on TIM2
    \param[in]  unit: msec /usec
    \param[out] none
    \retval     none
*/
static void hwp_time_set(uint8_t unit)
{
    timer_parameter_struct  timer_basestructure;

    timer_disable(TIMER2);
    timer_interrupt_disable(TIMER2, TIMER_INT_UP);

    if (TIM_USEC_DELAY == unit) {
        timer_basestructure.period = 11U;
    } else if (TIM_MSEC_DELAY == unit) {
        timer_basestructure.period = 11999U;
    } else {
        /* no operation */
    }

    timer_basestructure.prescaler         = (uint16_t)timer_prescaler;
    timer_basestructure.alignedmode       = TIMER_COUNTER_EDGE;
    timer_basestructure.counterdirection  = TIMER_COUNTER_UP;
    timer_basestructure.clockdivision     = TIMER_CKDIV_DIV1;
    timer_basestructure.repetitioncounter = 0U;

    timer_init(TIMER2, &timer_basestructure);

    timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

    timer_auto_reload_shadow_enable(TIMER2);

    /* timer2 interrupt enable */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    /* timer2 enable counter */
    timer_enable(TIMER2);
}

/*!
    \brief      configure the pll of usb
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pllusb_rcu_config(void)
{
    if (SET != rcu_flag_get(RCU_FLAG_HXTALSTB)) {
        rcu_pllusbpresel_config(RCU_PLLUSBPRESRC_IRC48M);
        rcu_pllusbpredv_config(RCU_PLLUSBPREDVSRC_HXTAL_IRC48M, RCU_PLLUSBPREDV_DIV4);
        rcu_pllusb_config(RCU_PLLUSB_MUL40);
    } else {
        rcu_pllusbpresel_config(RCU_PLLUSBPRESRC_HXTAL);
        rcu_pllusbpredv_config(RCU_PLLUSBPREDVSRC_HXTAL_IRC48M, RCU_PLLUSBPREDV_DIV5);
        rcu_pllusb_config(RCU_PLLUSB_MUL96);
    }

    RCU_ADDCTL |= RCU_ADDCTL_PLLUSBEN;
    while(0U == (RCU_ADDCTL & RCU_ADDCTL_PLLUSBSTB)) {
    }
}
