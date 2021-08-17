/*!
    \file    app.c
    \brief   USB main routine for DFU

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

#include "drv_usb_hw.h"
#include "dfu_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riscv_encoding.h"

usb_core_driver usb_dfu_dev;

/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    eclic_global_interrupt_enable();

    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);

    /* configure tamper key to run firmware */
    gd_eval_key_init(KEY_A, KEY_MODE_GPIO);

    usb_timer_init();

    /* CET key must be pressed on GD32V103V-EVAL when power on */
    if (0 == gd_eval_key_state_get(KEY_A)) {
        /* test if user code is programmed starting from address 0x08010000 */
        if (0x0001AAB1 == (*(__IO uint32_t*)APP_LOADED_ADDR)) {
            clear_csr(mstatus, MSTATUS_MIE);
            asm volatile ("jr %0 " :: "r"(APP_LOADED_ADDR));
        }
    }

    /* configure USB clock */
    usb_rcu_config();

    /* USB device stack configure */
    usbd_init(&usb_dfu_dev, USB_CORE_ENUM_FS, &dfu_desc, &dfu_class);

    usb_intr_config();

    /* check if USB device is enumerated successfully */
    while (usb_dfu_dev.dev.cur_status != USBD_CONFIGURED) {
    }

    while (1) {
    }
}
