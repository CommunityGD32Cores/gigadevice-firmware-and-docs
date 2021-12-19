/*!
    \file    gd32w51x_usb_hw.c
    \brief   this file implements the board support package for the USB host library

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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
#define HOST_OVRCURR_PORT                       GPIOE
#define HOST_OVRCURR_LINE                       GPIO_PIN_1
#define HOST_OVRCURR_PORT_SOURCE                GPIO_PORT_SOURCE_GPIOE
#define HOST_OVRCURR_PIN_SOURCE                 GPIO_PINSOURCE1
#define HOST_OVRCURR_PORT_RCC                   RCC_APB2PERIPH_GPIOE
#define HOST_OVRCURR_EXTI_LINE                  EXTI_LINE1
#define HOST_OVRCURR_IRQn                       EXTI1_IRQn

#define HOST_POWERSW_PORT_RCC                   RCU_GPIOC             
#define HOST_POWERSW_PORT                       GPIOC
#define HOST_POWERSW_VBUS                       GPIO_PIN_4

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
    \brief      configure USB data line gpio
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* USBFS_DM(PB12) and USBFS_DP(PB13) GPIO pin configuration */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_12 | GPIO_PIN_13);

    gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_12 | GPIO_PIN_13);
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config (void)
{
    rcu_usbfs_clock_config(RCU_USBFSSRC_PLL);

    rcu_usbfs_div_config(RCU_USBFS_DIV7);

    rcu_periph_clock_enable(RCU_USBFS);
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
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

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
    \brief      drives the VBUS signal through GPIO
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

    gpio_mode_set(HOST_POWERSW_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, HOST_POWERSW_VBUS);
    gpio_output_options_set(HOST_POWERSW_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, HOST_POWERSW_VBUS);
    /* by default, disable is needed on output of the power switch */
    gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);

    /* delay is need for stabilizing the VBUS low in reset condition,
     * when VBUS=1 and reset-button is pressed by user 
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
    \brief      delay in milliseconds
    \param[in]  msec: value of delay required in milliseconds
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
    \param[in]  unit: delay Time unit = milliseconds / microseconds
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
    \brief      configures TIMER2 for delay routine based on TIMER2
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
        timer_basestructure.period = 7U;
    } else if (TIM_MSEC_DELAY == unit) {
        timer_basestructure.period = 7999U;
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
