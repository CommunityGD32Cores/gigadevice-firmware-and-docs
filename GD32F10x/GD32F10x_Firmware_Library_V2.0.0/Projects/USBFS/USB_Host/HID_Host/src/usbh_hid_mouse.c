/*!
    \file  usbh_hid_mouse.c 
    \brief this file is the application layer for usb host hid mouse handling
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "usbh_hid_mouse.h"

int16_t  x_loc = 0, y_loc = 0;
int16_t  prev_x = 0, prev_y = 0;

static void mouse_init    (void);
static void mouse_decode  (uint8_t *data);

extern char_format_struct charform;

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

/*!
    \brief      the function is to handle mouse button press
    \param[in]  button_idx : mouse button pressed
    \param[out] none
    \retval     none
*/
void hid_mouse_button_pressed(uint8_t button_idx)
{
    /* change the color of button pressed to indicate button press*/
    switch (button_idx) {
        /* left button pressed */
        case HID_MOUSE_LEFTBUTTON:
            lcd_rectangle_fill(40, 40, 70, 110, MAGENTA);
            break;

        /* right button pressed */  
        case HID_MOUSE_RIGHTBUTTON:
            lcd_rectangle_fill(40, 210, 70, 280, MAGENTA);
            break;

        /* middle button pressed */
        case HID_MOUSE_MIDDLEBUTTON:
            lcd_rectangle_fill(40, 130, 70, 190, MAGENTA);
            break;

        default:
            break;
    }
}

/*!
    \brief      the function is to handle mouse button release
    \param[in]  button_idx : mouse button released
    \param[out] none
    \retval     none
*/
void hid_mouse_button_released(uint8_t button_idx)
{
    /* change the color of button released to default button color*/
    switch (button_idx) {
        /* left button released */
        case HID_MOUSE_LEFTBUTTON:
            lcd_rectangle_fill(40, 40, 70, 110, WHITE);
            break;

        /* right button released */  
        case HID_MOUSE_RIGHTBUTTON:
            lcd_rectangle_fill(40, 210, 70, 280, WHITE);
            break;

        /* middle button released */
        case HID_MOUSE_MIDDLEBUTTON:
            lcd_rectangle_fill(40, 130, 70, 190, WHITE);
            break;

        default:
            break;
    }
}

/*!
    \brief      the function is to handle mouse scroll to upadte the mouse position on display window
    \param[in]  x: USB HID mouse X co-ordinate
    \param[in]  y: USB HID mouse Y co-ordinate
    \param[out] none
    \retval     none
*/
void hid_mouse_update_position (int8_t x, int8_t y)
{
    if ((x != 0) || (y != 0)) {
        x_loc += x / 2;
        y_loc += y / 2;

        if(y_loc > MOUSE_WINDOW_HEIGHT - 16) {
            y_loc = MOUSE_WINDOW_HEIGHT - 16;
        }

        if(x_loc > MOUSE_WINDOW_WIDTH - 8) {
            x_loc = MOUSE_WINDOW_WIDTH - 8;
        }

        if(y_loc < 2) {
            y_loc = 2;
        }

        if(x_loc < 2) {
            x_loc = 2;
        }

        lcd_rectangle_fill(81, 41, 190, 280, BLACK);

        charform.char_color = GREEN;

        lcd_char_display (MOUSE_WINDOW_X - y_loc, 
                          MOUSE_WINDOW_Y + x_loc,
                          'x', charform);

        prev_x = x_loc;
        prev_y = x_loc;
    }
}
