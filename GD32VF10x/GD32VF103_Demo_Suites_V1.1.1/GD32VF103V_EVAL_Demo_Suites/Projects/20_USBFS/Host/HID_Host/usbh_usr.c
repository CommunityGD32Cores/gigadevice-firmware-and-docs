/*!
    \file    usbh_usr.c
    \brief   some user routines

    \version 2020-08-04, V1.1.0, demo for GD32VF103
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

#include "lcd_log.h"
#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include "usb_lcd_conf.h"

#include <string.h>

extern int16_t XLoc, YLoc;
extern __IO int16_t PrevX, PrevY;

uint16_t KeybrdCharXpos = 0;
uint16_t KeybrdCharYpos = 0;

extern usb_core_driver usb_hid_core;

/* Points to the DEVICE_PROP structure of current device */
usbh_user_cb usr_cb =
{
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    NULL,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};

const uint8_t MSG_HOST_HEADER[] = "USB OTG FS HID Host";
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v2.1.0";

/*!
    \brief      user operation for host-mode initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0;

    if(startup == 0 ){
        startup = 1;

        /* configure the User key */
        gd_eval_key_init(KEY_CET, KEY_MODE_GPIO);

        gd_eval_lcd_init();

        lcd_log_init();

        lcd_log_header_set((uint8_t *)MSG_HOST_HEADER, 60);

        LCD_UsrLog("USB host library started\n");

        lcd_log_footer_set((uint8_t *)MSG_HOST_FOOTER, 40);
    }
}

/*!
    \brief      de-init user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{

}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
    LCD_UsrLog("> Device Attached.\n");
}

/*!
    \brief      user operation for unrecovered error happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    LCD_ErrLog("> UNRECOVERED ERROR STATE.\n");
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    lcd_background_color_set(LCD_COLOR_BLACK);
    lcd_text_color_set(LCD_COLOR_BLACK);

    lcd_log_textzone_clear (LCD_TEXT_ZONE_X, 
                            LCD_TEXT_ZONE_Y, 
                            LCD_TEXT_ZONE_HEIGHT, 
                            LCD_TEXT_ZONE_WIDTH);

    LCD_UsrLog("> Device Disconnected.\n");
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    /* users can do their application actions here for the USB-Reset */
    LCD_UsrLog("> Reset the USB device.\n");
}

/*!
    \brief      user operation for detecting device speed
    \param[in]  DeviceSpeed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if (PORT_SPEED_HIGH == device_speed) {
        LCD_UsrLog("> High speed device detected.\r\n");
    } else if(PORT_SPEED_FULL == device_speed) {
        LCD_UsrLog("> Full speed device detected.\r\n");
    } else if(PORT_SPEED_LOW == device_speed) {
        LCD_UsrLog("> Low speed device detected.\r\n");
    } else {
        LCD_ErrLog("> Device Fault.\r\n");
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  DeviceDesc: device descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
    usb_desc_dev *pDevStr = device_desc;

    LCD_DevInformation("VID: %04Xh\n", (uint32_t)pDevStr->idVendor);
    LCD_DevInformation("PID: %04Xh\n", (uint32_t)pDevStr->idProduct);
}

/*!
    \brief      USB device is successfully assigned the address
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_address_assigned(void)
{

}

/*!
    \brief      user operation when configuration descriptor is available
    \param[in]  cfgDesc: pointer to configuration descriptor
    \param[in]  itfDesc: pointer to interface descriptor
    \param[in]  epDesc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc,
                                           usb_desc_itf *itf_desc,
                                           usb_desc_ep *ep_desc)
{
    usb_desc_itf *id = itf_desc;

    if ((*id).bInterfaceClass == 0x08) {
        LCD_UsrLog("> Mass storage device connected.\n");
    } else if ((*id).bInterfaceClass == 0x03) {
        LCD_UsrLog("> HID device connected.\n");
    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  ManufacturerString: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    LCD_DevInformation("Manufacturer: %s\r\n", (char *)manufacturer_string);
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  ProductString: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    LCD_DevInformation("Product: %s\r\n", (char *)product_string);
}

/*!
    \brief      user operation when serialnum string exists
    \param[in]  SerialNumString: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    LCD_DevInformation("Serial Number: %s\r\n", (char *)serial_num_string);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    LCD_UsrLog("> Enumeration completed.\n");

    lcd_text_color_set(LCD_COLOR_RED);
    lcd_vertical_string_display(LCD_HINT_LINE0, 0, (uint8_t *)"---------------------------------------");
    lcd_text_color_set(LCD_COLOR_GREEN);
    lcd_vertical_string_display(LCD_HINT_LINE1, 0, (uint8_t *)"To start the HID class operations:  ");
    lcd_vertical_string_display(LCD_HINT_LINE2, 0, (uint8_t *)"Press CET Key...             ");
}

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    LCD_ErrLog("> Device not supported.\n");
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     user response for user key
*/
usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USBH_USER_NO_RESP;

    /*Key B3 is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_CET))
    {
        usbh_usr_status = USBH_USER_RESP_OK;
    }

    return usbh_usr_status;
}

/*!
    \brief      user action for device overcurrent detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
    LCD_ErrLog("> Overcurrent detected.\n");
}

/*!
    \brief      init mouse window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USR_MOUSE_Init (void)
{
    LCD_UsrLog("> HID Demo Device : Mouse.\n");

    lcd_text_color_set(LCD_COLOR_BLACK);

    lcd_log_textzone_clear (LCD_TEXT_ZONE_X, 
                            LCD_TEXT_ZONE_Y, 
                            LCD_TEXT_ZONE_HEIGHT, 
                            LCD_TEXT_ZONE_WIDTH);

    lcd_text_color_set(LCD_COLOR_GREY);
    lcd_rectangle_fill(MOUSE_WINDOW_X, MOUSE_WINDOW_Y, MOUSE_WINDOW_WIDTH, MOUSE_WINDOW_HEIGHT);

    lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, MOUSE_BUTTON1_YCHORD, MOUSE_BUTTON_WIDTH, MOUSE_BUTTON_HEIGHT);
    lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, MOUSE_BUTTON3_YCHORD, MOUSE_BUTTON_WIDTH, MOUSE_BUTTON_HEIGHT);
    lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, MOUSE_BUTTON2_YCHORD, MOUSE_BUTTON_WIDTH, MOUSE_BUTTON_HEIGHT);

    lcd_text_color_set(LCD_COLOR_BLACK);
    lcd_background_color_set(LCD_COLOR_WHITE);
    lcd_vertical_char_display(MOUSE_SPL_X, MOUSE_SPL_Y, 'x');

    HID_MOUSE_ButtonReleased(0);
    HID_MOUSE_ButtonReleased(1);
    HID_MOUSE_ButtonReleased(2);

    XLoc  = 0;
    YLoc  = 0; 
    PrevX = 0;
    PrevY = 0;
}

/*!
    \brief      process mouse data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USR_MOUSE_ProcessData (hid_mouse_info *data)
{
    if ((data->x != 0) && (data->y != 0)) {
        HID_MOUSE_UpdatePosition(data->x, data->y);
    }

    for (uint8_t index = 0; index < 3; index++) {
        if (data->buttons[index]) {
            HID_MOUSE_ButtonPressed(index);
        } else {
            HID_MOUSE_ButtonReleased(index);
        }
    }
}

/*!
    \brief      init keyboard window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  USR_KEYBRD_Init (void)
{
    LCD_UsrLog("> HID Demo Device : Keyboard.\n");
    LCD_UsrLog("> Use Keyboard to tape characters: \n");

    lcd_text_color_set(LCD_COLOR_BLACK);

    lcd_log_textzone_clear (LCD_TEXT_ZONE_X, 
                            LCD_TEXT_ZONE_Y, 
                            LCD_TEXT_ZONE_HEIGHT, 
                            LCD_TEXT_ZONE_WIDTH);

    lcd_text_color_set(LCD_COLOR_GREY);
    lcd_rectangle_fill(KYBRD_WINDOW_X, KYBRD_WINDOW_Y, KYBRD_WINDOW_WIDTH, KYBRD_WINDOW_HEIGHT);

    KeybrdCharXpos = KYBRD_FIRST_LINE;
    KeybrdCharYpos = KYBRD_FIRST_COLUMN;
}

/*!
    \brief      process keyboard data
    \param[in]  data: keyboard data to be displayed
    \param[out] none
    \retval     none
*/
void USR_KEYBRD_ProcessData (uint8_t data)
{
    if(data == '\n')
    {
        KeybrdCharYpos = KYBRD_FIRST_COLUMN;

        /* Increment char X position */
        UPDATE_XP(KeybrdCharXpos);
    }
    else if (data == '\r')
    {
        /* Manage deletion of character and update cursor location */
        if(KeybrdCharYpos == KYBRD_FIRST_COLUMN) 
        {
            /* First character of first line to be deleted */
            if(KeybrdCharXpos == KYBRD_FIRST_LINE)
            {
                KeybrdCharYpos = KYBRD_FIRST_COLUMN;
            }
            else
            {
                UPDATE_XP(KeybrdCharXpos);
                KeybrdCharYpos =(KYBRD_LAST_COLUMN + SMALL_FONT_COLUMN_WIDTH); 
            }
        }
        else
        {
            UPDATE_YP(KeybrdCharYpos);
        }

        lcd_text_color_set(LCD_COLOR_BLACK);
        lcd_background_color_set(LCD_COLOR_WHITE);
        lcd_vertical_char_display(CHAR_CURSOR(KeybrdCharXpos, KeybrdCharYpos), ' ');
    }
    else
    {
        lcd_text_color_set(LCD_COLOR_BLACK);
        lcd_background_color_set(LCD_COLOR_WHITE);
        lcd_vertical_char_display(CHAR_CURSOR(KeybrdCharXpos, KeybrdCharYpos), data);

        /* Update the cursor position on LCD */

        /* Increment char Y position */
        UPDATE_YP(KeybrdCharYpos);

        /*Check if the Y position has reached the last column*/
        if(KeybrdCharYpos == KYBRD_LAST_COLUMN)
        {
            KeybrdCharYpos = KYBRD_FIRST_COLUMN;

            /*Increment char X position*/
            UPDATE_XP(KeybrdCharXpos);
        }
    }

    if(KYBRD_LINE_LIMIT(KeybrdCharXpos))
    {
        lcd_text_color_set(LCD_COLOR_GREY);
        lcd_rectangle_fill(KYBRD_WINDOW_X, KYBRD_WINDOW_Y, KYBRD_WINDOW_WIDTH, KYBRD_WINDOW_HEIGHT);

        KeybrdCharXpos = KYBRD_FIRST_LINE;
    }
}
