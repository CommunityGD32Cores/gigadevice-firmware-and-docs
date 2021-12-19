/*!
    \file    usbh_usr.c
    \brief   some user routines

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

#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include <string.h>

extern int16_t XLoc, YLoc;
extern __IO int16_t PrevX, PrevY;

uint16_t KeybrdCharXpos = 0U;
uint16_t KeybrdCharYpos = 0U;

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
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v1.0.0";

/*!
    \brief      user operation for host-mode initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0U;

    if(0U == startup)
    {
        startup = 1U;

        gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_GPIO);

        gd_eval_com_init(EVAL_COM0);

        LCD_UsrLog("\r\n++++USB host library started++++\r\n");
    }
}

/*!
    \brief      deinitialize user state and associated variables
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
    LCD_ErrLog("> Unrecovered Error State.\n");
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
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

    if (0x08 == (*id).bInterfaceClass) {
        LCD_UsrLog("> Mass storage device connected.\n");
    } else if (0x03 == (*id).bInterfaceClass) {
        LCD_UsrLog("> HID device connected.\n");
    } else {

    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  ManufacturerString: manufacturer string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    LCD_DevInformation("Manufacturer: %s\r\n", (char *)manufacturer_string);
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  ProductString: product string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    LCD_DevInformation("Product: %s\r\n", (char *)product_string);
}

/*!
    \brief      user operation when serialnum string exists
    \param[in]  SerialNumString: serialNum string of USB device
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
    \retval     user response for CET key
*/
usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USBH_USER_NO_RESP;

    /*Key CET is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_TAMPER_WAKEUP)) {
        usbh_usr_status = USBH_USER_RESP_OK;
    }

    return usbh_usr_status;
}

/*!
    \brief      user action for device over current detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
    LCD_ErrLog("> Overcurrent detected.\n");
}

/*!
    \brief      initialize mouse window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_init (void)
{
    LCD_UsrLog("> HID Demo Device : Mouse.\n");
}

/*!
    \brief      process mouse data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_process_data (hid_mouse_info *data)
{
    static uint8_t button_pressed_flag = 0;

    if (0x00U == (data->x&0x80)) {
        printf("\r\n MoveRight %x units",data->x);
    } else {
        printf("\r\n MoveLeft %x units",((uint8_t)~(data->x)+1));
    }
    if (0x00U == (data->y&0x80)) {
        printf("---*---MoveDown %x units",data->y);
    } else {
        printf("---*---MoveUp %x units",((uint8_t)~(data->y)+1));
    }
    printf("---*---");

    if (data->buttons[0]) {
        printf("Left button is pressed.");
        data->buttons[0] -= 0x01;
        button_pressed_flag = 1;
    } else if(data->buttons[1]) {
        printf("Right button is pressed.");
        data->buttons[1] -= 0x02;
        button_pressed_flag = 1;
    } else if(data->buttons[2]) {
        printf("The wheel is pressed.");
        data->buttons[2] -= 0x04;
        button_pressed_flag = 1;
    } else {
        if (button_pressed_flag == 1) {
            printf("The button is released.");
            button_pressed_flag = 0;
        } else {
            printf("No button is pressed.");
        }
    }
}

/*!
    \brief      initialize keyboard window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_keybrd_init (void)
{
    LCD_UsrLog("> HID Demo Device : Keyboard.\n");
    LCD_UsrLog("> Use Keyboard to tape characters: \n");
}

/*!
    \brief      process keyboard data
    \param[in]  data: keyboard data to be displayed
    \param[out] none
    \retval     none
*/
void usr_keybrd_process_data (uint8_t data)
{
    printf("\r\n The pressed button is %c",data);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC));
    return ch;
}
