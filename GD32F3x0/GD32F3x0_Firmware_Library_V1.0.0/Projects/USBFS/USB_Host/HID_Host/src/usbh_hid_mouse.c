/*!
    \file  usbh_hid_mouse.c 
    \brief this file is the application layer for usb host hid mouse handling
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "usbh_hid_mouse.h"

static void mouse_init    (void);
static void mouse_decode  (uint8_t *data);

hid_mouse_data_struct hid_mouse_data;

hid_cb_struct hid_mouse_cb = 
{
    mouse_init,
    mouse_decode,
};

/*!
    \brief      init mouse state.
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void  mouse_init (void)
{
    /* call user init */
    usr_mouse_init();
}

/*!
    \brief      decode mouse data
    \param[in]  data: pointer to mouse hid data buffer
    \param[out] none
    \retval     none
*/
static void  mouse_decode(uint8_t *data)
{
    hid_mouse_data.button = data[0];

    hid_mouse_data.x = data[1];
    hid_mouse_data.y = data[2];

    usr_mouse_process_data(&hid_mouse_data);
}

