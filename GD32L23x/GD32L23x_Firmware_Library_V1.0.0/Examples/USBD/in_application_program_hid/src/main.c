/*!
    \file    main.c
    \brief   USB IAP device

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "usb_iap_core.h"
#include "usbd_hw.h"
#include "systick.h"

usb_dev usbd_iap;

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t app_address;
    app_func application;
    uint32_t exe_add = 0x0;

    /* configure wakeup key to run firmware */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);

    /* wakeup key must be pressed on board when power on */
    if(0U != gd_eval_key_state_get(KEY_WAKEUP)) {
        /* test if user code is programmed starting from address 0x8008000 */
        if(((*(__IO uint32_t *)APP_LOADED_ADDR) & 0x2FFE0000U) == 0x20000000U) {

            SCB->VTOR = APP_LOADED_ADDR;
            delay_decrement();
            exe_add = *(uint32_t *)(APP_LOADED_ADDR + 0x4);
            (*((void (*)())exe_add))();
        }
    }

    /* system clocks configuration */
    rcu_config();

    /* USB device configuration */
    usbd_init(&usbd_iap, &iap_desc, &iap_class);

    /* NVIC configuration */
    nvic_config();

    /* enabled USB pull-up */
    usbd_connect(&usbd_iap);

    while(1) {
    }
}
