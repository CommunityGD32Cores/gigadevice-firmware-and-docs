/*!
    \file    usbd_core.c
    \brief   USB device driver

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

#include "usbd_core.h"
#include "usbd_enum.h"
#include "usbd_transc.h"
#include "usbd_lld_core.h"

#include <stdio.h>

usbd_int_cb_struct *usbd_int_fops = NULL;

/*!
    \brief      configure USB device initialization
    \param[in]  udev: pointer to USB core instance
    \param[in]  desc: pointer to USB descriptor
    \param[in]  usbc: USB class
    \param[out] none
    \retval     none
*/
void usbd_init(usb_dev *udev, usb_desc *desc, usb_class *usbc)
{
    /* configure USBD core basic attributes */
    usbd_core.basic.max_ep_count = 8U;
    usbd_core.basic.twin_buf = 1U;
    usbd_core.basic.ram_size = 512U;

    usbd_core.dev = udev;

    udev->desc = desc;
    udev->class_core = usbc;
    udev->drv_handler = &usbd_drv_handler;

    udev->control.ctl_state = USBD_CTL_IDLE;

    udev->ep_transc[0][TRANSC_SETUP] = _usb_setup_transc;
    udev->ep_transc[0][TRANSC_OUT] = _usb_out0_transc;
    udev->ep_transc[0][TRANSC_IN] = _usb_in0_transc;

    /* configure power management */
    udev->pm.power_mode = (udev->desc->config_desc[7] & BIT(6)) >> 6;

    /* enable USB suspend */
    udev->pm.suspend_enabled = 1U;

    /* USB low level initialization */
    udev->drv_handler->init();

    /* create serial string */
    serial_string_get((uint16_t *)udev->desc->strings[STR_IDX_SERIAL]);
}

/*!
    \brief      endpoint prepare to receive data
    \param[in]  udev: pointer to USB core instance
    \param[in]  ep_addr: endpoint address
                  in this parameter:
                    bit0..bit6: endpoint number (0..7)
                    bit7: endpoint direction which can be IN(1) or OUT(0)
    \param[in]  pbuf: user buffer address pointer
    \param[in]  buf_len: buffer length
    \param[out] none
    \retval     none
*/
void usbd_ep_recev(usb_dev *udev, uint8_t ep_addr, uint8_t *pbuf, uint16_t buf_len)
{
    /* configure the transaction level parameters */
    usb_transc *transc = &udev->transc_out[EP_ID(ep_addr)];

    usb_transc_config(transc, pbuf, buf_len, 0U);

    /* enable endpoint to receive */
    udev->drv_handler->ep_rx_enable(udev, ep_addr);
}

/*!
    \brief      endpoint prepare to transmit data
    \param[in]  udev: pointer to USB core instance
    \param[in]  ep_addr: endpoint address
                  in this parameter:
                    bit0..bit6: endpoint number (0..7)
                    bit7: endpoint direction which can be IN(1) or OUT(0)
    \param[in]  pbuf: transmit buffer address pointer
    \param[in]  buf_len: buffer length
    \param[out] none
    \retval     none
*/
void usbd_ep_send(usb_dev *udev, uint8_t ep_addr, uint8_t *pbuf, uint16_t buf_len)
{
    uint16_t len;
    uint8_t ep_num = EP_ID(ep_addr);
    usb_transc *transc = &udev->transc_in[ep_num];

    if(USBD_EP_DBL_BUF_GET(ep_num)) {
        /* at the start of the transaction, fill in the RX buffer and TX buffer for the first time */
        len = USB_MIN(buf_len, 2U * transc->max_len);

        /* write data from user FIFO to USB RAM */
        udev->drv_handler->ep_dbl_write(pbuf, ep_num, len, 1U);

        /* calculate the number of packets to be transmitted */
        if(0U != buf_len) {
            transc->xfer_packet_num = ((buf_len - 1U) + transc->max_len) / transc->max_len;
        } else {
            transc->xfer_packet_num = 1U;
        }

        if(2U == transc->xfer_packet_num) {
            /* update transaction parameter, record the size of the last packet to transc->xfer_count */
            usb_transc_config(transc, pbuf + len, buf_len - len, len - transc->max_len);
        } else {
            /* update transaction parameter */
            usb_transc_config(transc, pbuf + len, buf_len - len, len);
        }
    } else {
        len = USB_MIN(buf_len, transc->max_len);

        /* write data from user FIFO to USB RAM */
        udev->drv_handler->ep_write(pbuf, ep_num, len);

        /* update transaction parameter */
        usb_transc_config(transc, pbuf + len, buf_len - len, len);
    }
}
