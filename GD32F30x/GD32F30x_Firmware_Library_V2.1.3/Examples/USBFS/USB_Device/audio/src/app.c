/*!
    \file    app.c
    \brief   USB main routine for Audio device

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
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
#include "audio_out_itf.h"

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
    usb_rcu_config();

    usb_timer_init();

    usbd_init (&usb_audio, USB_CORE_ENUM_FS, &audio_desc, &usbd_audio_cb);

    usb_intr_config();
    
#ifdef USE_IRC48M
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC configure */
    ctc_config();

    while (RESET == ctc_flag_get(CTC_FLAG_CKOK)) {
    }
#endif /* USE_IRC48M */

    while(USBD_CONFIGURED != usb_audio.dev.cur_status);

#ifdef USE_USB_AUDIO_MICPHONE
    for(__IO uint32_t i = 0; i < 2000; i++){
        for(__IO uint32_t j = 0; j < 10000; j++);
    }

    usbd_ep_send(&usb_audio, AUDIO_IN_EP, (uint8_t*)wavetestdata, MIC_IN_PACKET);
    count_data = MIC_IN_PACKET;
#endif

    /* Main loop */
    while (1) {
    }
}
