/*!
    \file    main.c
    \brief   USB OTG_FS host-mode HID class (mouse and keyboard) demo main file

    \version 2020-09-04, V1.0.0, demo for GD32E50x
    \version 2021-03-31, V1.1.0, demo for GD32E50x
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
#include "drv_usb_core.h"
#include "usbh_hid_core.h"
#include "usbh_usr.h"

usbh_host usb_host;

/*!
    \brief      main routine for HID mouse / keyboard class application
    \param[in]  none
    \param[out] none
    \retval     int
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM);

    usb_rcu_config();

    usb_timer_init();

    /* configure GPIO pin used for switching VBUS power and charge pump I/O */
    usb_vbus_config();

    /* register device class */
    usbh_class_register(&usb_host, &usbh_hid);

    usbh_init(&usb_host, &usr_cb);

#ifdef USE_USB_HS
    pllusb_rcu_config();
#endif /* USE_USB_HS */

    /* enable interrupts */
    usb_intr_config();

    while(1){
        /* Host state handler */
        usbh_core_task(&usb_host);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}
