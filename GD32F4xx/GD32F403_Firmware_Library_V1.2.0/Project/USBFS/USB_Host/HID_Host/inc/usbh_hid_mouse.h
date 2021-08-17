/*!
    \file  usbh_hid_mouse.h 
    \brief this file contains all the prototypes for the usbh_hid_mouse.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-05-10, V1.0.0, firmware for GD32F403
*/


#ifndef USBH_HID_MOUSE_H
#define USBH_HID_MOUSE_H

#include "usbh_hid_core.h"

/* Left Button: Report data: 0x00*/
#define HID_MOUSE_LEFTBUTTON                    0x00

/* Right Button: Report data: 0x01*/
#define HID_MOUSE_RIGHTBUTTON                   0x01

/* Middle Button: Report data: 0x02*/
#define HID_MOUSE_MIDDLEBUTTON                  0x02

/* Mouse directions */
#define MOUSE_TOP_DIR                       0x80
#define MOUSE_BOTTOM_DIR                    0x00
#define MOUSE_LEFT_DIR                      0x80
#define MOUSE_RIGHT_DIR                     0x00

#define MOUSE_WINDOW_X                      190
#define MOUSE_WINDOW_Y                      40
#define MOUSE_WINDOW_X_MAX                  181
#define MOUSE_WINDOW_Y_MIN                  101
#define MOUSE_WINDOW_HEIGHT                 110
#define MOUSE_WINDOW_WIDTH                  240

#define HID_MOUSE_BUTTON_HEIGHT             10
#define HID_MOUSE_BUTTON_WIDTH              24
#define HID_MOUSE_BUTTON_XCHORD             201
#define HID_MOUSE_BUTTON1_YCHORD            220
#define HID_MOUSE_BUTTON3_YCHORD            166
#define HID_MOUSE_BUTTON2_YCHORD            116

#define MOUSE_LEFT_MOVE                     1
#define MOUSE_RIGHT_MOVE                    2
#define MOUSE_UP_MOVE                       3
#define MOUSE_DOWN_MOVE                     4

#define HID_MOUSE_HEIGHTLSB                 2
#define HID_MOUSE_WIDTHLSB                  2
#define HID_MOUSE_RES_X                     4
#define HID_MOUSE_RES_Y                     4

#define SMALL_FONT_COLUMN_WIDTH             8
#define SMALL_FONT_LINE_WIDTH               16

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
void usr_mouse_init (void);
/* process mouse data */
void usr_mouse_process_data (hid_mouse_data_struct *data);
/* handle mouse scroll to upadte the mouse position on display window */
void hid_mouse_position_update (int8_t x, int8_t y);
/* handle mouse button press */
void hid_mouse_button_pressed(uint8_t button_idx);
/* handle mouse button release */
void hid_mouse_button_released(uint8_t button_idx);

#endif /* USBH_HID_MOUSE_H */
