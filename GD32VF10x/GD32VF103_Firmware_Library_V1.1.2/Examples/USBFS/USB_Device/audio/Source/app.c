/*!
    \file    app.c
    \brief   USB main routine for Audio device

    \version 2020-08-04, V1.1.0, firmware for GD32VF103
    \version 2021-05-19, V1.1.1, firmware for GD32VF103
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
#include "audio_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_USB_AUDIO_MICPHONE
    #include "wave_data.h"
#endif

usb_core_driver usb_audio;

#ifdef USE_USB_AUDIO_MICPHONE
    volatile uint32_t count_data = 0;
#endif

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

    /* configure USB clock */
    usb_rcu_config();

    /* timer nvic initialization */
    usb_timer_init();

    /* USB device stack configure */
    usbd_init (&usb_audio, USB_CORE_ENUM_FS, &audio_desc, &usbd_audio_cb);

    /* USB interrupt configure */
    usb_intr_config();

    /* check if USB device is enumerated successfully */
    while (usb_audio.dev.cur_status != USBD_CONFIGURED) {
    }

#ifdef USE_USB_AUDIO_MICPHONE
    for(__IO uint32_t i = 0; i < 2000; i++){
        for(__IO uint32_t j = 0; j < 10000; j++);
    }

    usbd_ep_send(&usb_audio, AUDIO_IN_EP, (uint8_t*)wavetestdata, MIC_IN_PACKET);
    count_data = MIC_IN_PACKET;
#endif

    while (1) {
    }
}
