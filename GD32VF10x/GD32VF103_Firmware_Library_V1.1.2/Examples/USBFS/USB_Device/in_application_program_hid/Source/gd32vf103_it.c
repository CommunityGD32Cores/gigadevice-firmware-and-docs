/*!
    \file    gd32vf103_it.c
    \brief   main interrupt service routines

    \version 2020-08-04, V1.1.0, firmware for GD32VF103
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

#include "drv_usbd_int.h"
#include "drv_usb_hw.h"
#include "gd32vf103_it.h"

extern usb_core_driver usb_iap_dev;
extern uint32_t usbfs_prescaler;

void usb_timer_irq(void);

/* local function prototypes ('static') */
static void resume_mcu_clk(void);

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler (void)
{
    usbd_isr (&usb_iap_dev);
}

/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (usb_iap_dev.bp.low_power) {
        resume_mcu_clk();

        rcu_usb_clock_config(usbfs_prescaler);

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_active(&usb_iap_dev);
    }

    exti_interrupt_flag_clear(EXTI_18);
}

/*!
    \brief      this function handles Timer0 updata interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}

/*!
    \brief      resume mcu clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HSE */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HSE is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)){
    }

    if(HXTAL_VALUE == 25000000){
        /* enable PLL1 */
        rcu_osci_on(RCU_PLL1_CK);

        /* wait till PLL1 is ready */
        while(RESET == rcu_flag_get(RCU_FLAG_PLL1STB)){
        }
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLL != rcu_system_clock_source_get()){
    }
}
