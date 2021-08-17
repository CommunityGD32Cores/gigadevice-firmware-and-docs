/*!
    \file  usbh_hid_mouse.h 
    \brief this file contains all the prototypes for the usbh_hid_mouse.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/


#ifndef USBH_HID_MOUSE_H
#define USBH_HID_MOUSE_H

#include "usbh_hid_core.h"

typedef struct
{
    uint8_t              x; 
    uint8_t              y;
    uint8_t              z;               /* not supported */ 
    uint8_t              button; 
}hid_mouse_data_struct;

extern hid_cb_struct hid_mouse_cb;
extern hid_mouse_data_struct hid_mouse_data;

/* function declarations */
/* init mouse window */
void  usr_mouse_init (void);
/* process mouse data */
void  usr_mouse_process_data (hid_mouse_data_struct *data);

#endif /* USBH_HID_MOUSE_H */
