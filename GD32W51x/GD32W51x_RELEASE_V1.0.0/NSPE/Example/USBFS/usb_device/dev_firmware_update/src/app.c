/*!
    \file    app.c
    \brief   main routine

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

#include "dfu_core.h"
#include "drv_usb_hw.h"

usb_core_driver usb_dfu_dev;

/*!
    \brief      main routine will construct a USB DFU device
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    app_func application;
    uint32_t app_addr;

    /* configure tamper key to run firmware */
    gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_GPIO);

    if(1U == gd_eval_key_state_get(KEY_TAMPER_WAKEUP)){
        /* test if user code is programmed starting from address 0x08008000 */
        if (0x20000000U == ((*(__IO uint32_t*)APP_LOADED_ADDR) & 0x2FFE0000U)) {
            app_addr = *(__IO uint32_t*) (APP_LOADED_ADDR + 4U);
            application = (app_func) app_addr;

            /* initialize user application's stack pointer */
            __set_MSP(*(__IO uint32_t*) APP_LOADED_ADDR);

            /* jump to user application */
            application();
        }
    }

    usb_gpio_config();

    usb_rcu_config();

    usb_timer_init();

    /* USB device stack configure */
    usbd_init(&usb_dfu_dev, &dfu_desc, &dfu_class);

    usb_intr_config();

    /* check if USB device is enumerated successfully */
    while (USBD_CONFIGURED != usb_dfu_dev.dev.cur_status) {
    }

    while (1) {
    }
}

