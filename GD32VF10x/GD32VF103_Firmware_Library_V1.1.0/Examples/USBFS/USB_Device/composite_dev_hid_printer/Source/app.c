/*!
    \file    app.c
    \brief   USB main routine for composite hid_printer

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riscv_encoding.h"
#include "hid_printer_wrapper.h"

usb_core_driver usbd_hid_printer;

/* local function prototypes ('static') */
static void key_config(void);
static void led_config(void);

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

    key_config();

    led_config();

    usb_timer_init();

    /* configure USB clock */
    usb_rcu_config();

    usb_intr_config();

    /* USB device stack configure */
    usbd_init (&usbd_hid_printer, USB_CORE_ENUM_FS, &hid_printer_desc, &usbd_hid_printer_cb);

    /* check if USB device is enumerated successfully */
    while (usbd_hid_printer.dev.cur_status != USBD_CONFIGURED) {
    }

    while (1) {
    }
}

/*!
    \brief      configure the keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void key_config(void)
{
    /* keys configuration */
    gd_eval_key_init(KEY_A, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_B, KEY_MODE_EXTI);
}

/*!
    \brief      configure the leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void led_config(void)
{
    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}
