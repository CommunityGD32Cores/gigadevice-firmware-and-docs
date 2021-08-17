/*!
    \file  usbh_hid_keybd.h
    \brief this file contains all the prototypes for the usbh_hid_keybd.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBH_HID_KEYBD_H
#define USBH_HID_KEYBD_H

#include "usb_conf.h"
#include "usbh_hid_core.h"

//#define AZERTY_KEYBOARD
#define QWERTY_KEYBOARD

#define KBD_LEFT_CTRL              0x01U
#define KBD_LEFT_SHIFT             0x02U
#define KBD_LEFT_ALT               0x04U
#define KBD_LEFT_GUI               0x08U
#define KBD_RIGHT_CTRL             0x10U
#define KBD_RIGHT_SHIFT            0x20U
#define KBD_RIGHT_ALT              0x40U
#define KBD_RIGHT_GUI              0x80U

#define KBR_MAX_NBR_PRESSED        6U

extern hid_cb_struct HID_KEYBRD_cb;

/* function declarations */
/* init keyboard window */
void  usr_keybrd_init (void);
/* process keyboard data */
void  usr_keybrd_process_data (uint8_t pbuf);

#endif /* USBH_HID_KEYBD_H */
