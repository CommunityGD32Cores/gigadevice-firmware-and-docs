/*!
    \file    hid_printer_wrapper.h
    \brief   the header file of hid_printer_wrapper.c file

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

#ifndef __HID_PRINTER_WRAPPER_H
#define __HID_PRINTER_WRAPPER_H

#include "custom_hid_core.h"
#include "printer_core.h"

#define NO_CMD                      0xFFU

/* USB configuration descriptor structure */
typedef struct
{
    usb_desc_config         config;

    usb_desc_itf            hid_itf;
    usb_desc_hid            hid_vendor;
    usb_desc_ep             hid_epin;
    usb_desc_ep             hid_epout;

    usb_desc_itf            printer_itf;
    usb_desc_ep             printer_epin;
    usb_desc_ep             printer_epout;
} usb_hid_printer_desc_config_set;

extern usb_desc hid_printer_desc;
extern usb_class_core usbd_hid_printer_cb;

#endif /* __HID_PRINTER_WRAPPER_H */
