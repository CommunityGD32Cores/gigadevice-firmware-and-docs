/*!
    \file    gd32f20x_hw.c
    \brief   USB hardware configuration for GD32F20x

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2019-04-01, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
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

#include "drv_usb_hw.h"

#define TIM_MSEC_DELAY                          0x01U
#define TIM_USEC_DELAY                          0x02U

__IO uint32_t delay_time = 0U;
__IO uint16_t timer_prescaler;
__IO uint32_t usbfs_prescaler = 0U;

/* local function prototypes ('static') */
static void hw_time_set (uint8_t unit);
static void hw_delay    (uint32_t ntime, uint8_t unit);

extern enum
{
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
}key_char_enum;

#ifdef USE_USBFS
    #define USBFS_CORE
#endif /* USE_USBFS */

#ifdef USBFS_CORE
    #define RX_FIFO_FS_SIZE                         128U
    #define TX0_FIFO_FS_SIZE                        64U
    #define TX1_FIFO_FS_SIZE                        128U
    #define TX2_FIFO_FS_SIZE                        0U
    #define TX3_FIFO_FS_SIZE                        0U

    #define USBFS_LOW_PWR_MGMT_SUPPORT
    #define USBFS_SOF_OUTPUT_ENABLED

    #define USE_IRC48M /* IRC48M supported */
#endif /* USBFS_CORE */

/*!
    \brief      get usb keyboard state
    \param[in]  none
    \param[out] none
    \retval     the char
*/
uint8_t key_state (void)
{
    /* have pressed user key */
    if (!gd_eval_key_state_get(KEY_USER)) {
        usb_mdelay(100U);

        if (!gd_eval_key_state_get(KEY_USER)) {
            return CHAR_C;
        }
    }

    /* no pressed any key */
    return 0U;
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config(void)
{
    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);
  
    if (48000000U == system_clock) {
        timer_prescaler = 3U;
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1;
    } else if (72000000U == system_clock) {
        timer_prescaler = 5U;
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
    } else if (96000000U == system_clock) {
        timer_prescaler = 7U;
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2;
    } else if (120000000U == system_clock) {
        timer_prescaler = 9U;
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2_5;
    }  else {
        /*  reserved  */
    }

    rcu_usbfs_trng_clock_config(usbfs_prescaler);
    rcu_periph_clock_enable(RCU_USBFS);
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config(void)
{
    nvic_irq_enable((uint8_t)USBFS_IRQn, 3U, 0U);

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 0U, 0U);
}

/*!
    \brief      initializes delay unit using Timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_init (void)
{
    /* set the vector table base address at 0x08000000 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00U);

    /* configure the priority group to 2 bits */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    
    /* enable the TIMER2 global interrupt */
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
    hw_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milliseconds
    \param[in]  msec: value of delay required in milliseconds
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
    if (RESET != timer_flag_get(TIMER2, TIMER_FLAG_UP)){
        timer_flag_clear(TIMER2, TIMER_FLAG_UP);

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
    \param[in]  unit: delay Time unit = milliseconds / microseconds
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
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);
    timer_deinit(TIMER2);
  
    if (TIM_USEC_DELAY == unit) {
        timer_initpara.period = 11U;
    } else if(TIM_MSEC_DELAY == unit) {
        timer_initpara.period = 11999U;
    }

    timer_initpara.prescaler         = timer_prescaler;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0U;
    timer_init(TIMER2, &timer_initpara);

    timer_update_event_enable(TIMER2);
    timer_interrupt_enable(TIMER2,TIMER_INT_UP);
    timer_flag_clear(TIMER2, TIMER_FLAG_UP);
    timer_update_source_config(TIMER2, TIMER_UPDATE_SRC_GLOBAL);

    /* TIMER2 counter enable */
    timer_enable(TIMER2);
}