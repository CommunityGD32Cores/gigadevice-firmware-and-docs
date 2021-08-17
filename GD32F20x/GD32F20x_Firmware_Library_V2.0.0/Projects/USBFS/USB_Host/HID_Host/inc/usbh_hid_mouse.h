/*!
    \file  usbh_hid_mouse.h 
    \brief this file contains all the prototypes for the usbh_hid_mouse.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/


#ifndef USBH_HID_MOUSE_H
#define USBH_HID_MOUSE_H

#include "usbh_hid_core.h"

/* Left Button: Report data: 0x00*/
#define HID_MOUSE_LEFTBUTTON                0x00

/* Right Button: Report data: 0x01*/
#define HID_MOUSE_RIGHTBUTTON               0x01

/* Middle Button: Report data: 0x02*/
#define HID_MOUSE_MIDDLEBUTTON              0x02

#define MOUSE_WINDOW_HEIGHT                 240
#define MOUSE_WINDOW_WIDTH                  272

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

void hid_mouse_update_position (int8_t X, int8_t Y);

void hid_mouse_button_released (uint8_t ButtonIdx);

void hid_mouse_button_pressed  (uint8_t ButtonIdx);

#endif /* USBH_HID_MOUSE_H */
