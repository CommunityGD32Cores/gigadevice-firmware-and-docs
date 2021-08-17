/*!
    \file    app.c
    \brief   USB main routine for HID/printer composite device

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

#include "hid_printer_wrapper.h"
#include "drv_usb_hw.h"

usb_core_driver usbd_hid_printer;

extern hid_fop_handler fop_handler;

/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure USB clock */
    usb_rcu_config();

    /* timer initialization */
    usb_timer_init();
    
    custom_hid_itfop_register(&usbd_hid_printer, &fop_handler);

    /* USB device stack configure */
    usbd_init (&usbd_hid_printer, USB_CORE_ENUM_FS, &hid_printer_desc, &usbd_hid_printer_cb);

    /* USB interrupt configure */
    usb_intr_config();
    
#ifdef USE_IRC48M
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC configure */
    ctc_config();

    while (RESET == ctc_flag_get(CTC_FLAG_CKOK)) {
    }
#endif /* USE_IRC48M */

    while(1){
    }
}
