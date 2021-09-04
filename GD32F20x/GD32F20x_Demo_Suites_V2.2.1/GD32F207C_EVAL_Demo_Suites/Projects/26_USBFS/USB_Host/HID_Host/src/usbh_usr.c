/*!
    \file  usbh_usr.c
    \brief the user callback function definitions

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2019-04-01, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
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

#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include <string.h>

#define KYBRD_FIRST_COLUMN         (uint16_t)0
#define KYBRD_LAST_COLUMN          (uint16_t)320
#define KYBRD_FIRST_LINE           (uint8_t)150
#define KYBRD_LAST_LINE            (uint8_t)30

uint8_t  KeybrdCharXpos = 0;
uint16_t KeybrdCharYpos = 0;
uint8_t  user_input_flag = 0;

extern int16_t  XLoc, YLoc;
extern __IO int16_t PrevX, PrevY;

extern usb_core_driver usb_hid_core;

/* points to the DEVICE_PROP structure of current device */
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

/* printf messages */
const uint8_t MSG_HOST_INIT[]          = "> Host Library Initialized.";
const uint8_t MSG_DEV_ATTACHED[]       = "> Device Attached.";
const uint8_t MSG_DEV_DISCONNECTED[]   = "> Device Disconnected.";
const uint8_t MSG_DEV_ENUMERATED[]     = "> Enumeration completed.";
const uint8_t MSG_DEV_HIGHSPEED[]      = "> High speed device detected.";
const uint8_t MSG_DEV_FULLSPEED[]      = "> Full speed device detected.";
const uint8_t MSG_DEV_LOWSPEED[]       = "> Low speed device detected.";
const uint8_t MSG_DEV_ERROR[]          = "> Device fault.";

const uint8_t MSG_MSC_CLASS[]          = "> Mass storage device connected.";
const uint8_t MSG_HID_CLASS[]          = "> HID device connected.";

const uint8_t USB_HID_MouseStatus[]    = "> HID Demo Device : Mouse.";
const uint8_t USB_HID_KeybrdStatus[]   = "> HID Demo Device : Keyboard.";
const uint8_t MSG_UNREC_ERROR[]        = "> UNRECOVERED ERROR STATE.";

const uint8_t MSG_HOST_HEADER[]        = "USBFS HID Host";
const uint8_t MSG_HOST_FOOTER[]        = "USB Host Library v1.0.0";

const uint8_t MSG_LIB_START[]          = "##### USB Host library started #####";
const uint8_t MSG_DEV_NOSUP[]          = "> Device not supported.";
const uint8_t MSG_OVERCURRENT[]        = "> Overcurrent detected.";
const uint8_t MSG_USE_KYBD[]           = "> Use Keyboard to tape characters: ";
const uint8_t MSG_RESET_DEV[]          = "> Reset the USB device.";

const uint8_t MSG_WAIT_INPUT[]         = "> Wait for user input!";
const uint8_t MSG_USER_INPUT[]         = "> User has input!";
  
/*!
    \brief      user operation for host-mode initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0;

    if (0U == startup) {
        startup = 1U;

        /* configure the User key */
        gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);
        printf("\r\n %s ",(uint8_t *)MSG_LIB_START);
    }
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
    printf("\r\n %s ",(uint8_t *)MSG_DEV_ATTACHED);
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    printf("\r\n %s ",(uint8_t *)MSG_UNREC_ERROR);
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    printf("\r\n %s ",(uint8_t *)MSG_DEV_DISCONNECTED);
}

/*!
    \brief      user operation for reset USB device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    printf("\r\n %s ",(uint8_t *)MSG_RESET_DEV);
}

/*!
    \brief      user operation for detectting device speed
    \param[in]  device_speed: the device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if (PORT_SPEED_HIGH == device_speed) {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_HIGHSPEED);
    } else if (PORT_SPEED_FULL == device_speed) {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_FULLSPEED);
    } else if (PORT_SPEED_LOW == device_speed) {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_LOWSPEED);
    } else {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_ERROR);
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  device_desc: the device descripter
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
    usb_desc_dev *p_devstr = device_desc;

    printf("\r\n > VID: %04Xh ",(uint32_t)p_devstr->idVendor);

    printf("\r\n > PID: %04Xh ",(uint32_t)p_devstr->idProduct);
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
    \param[in]  cfg_desc: pointer to configuration descriptor
    \param[in]  itf_desc: pointer to interface descriptor
    \param[in]  ep_desc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc,
                                           usb_desc_itf *itf_desc,
                                           usb_desc_ep *ep_desc)
{
    usb_desc_itf *id = itf_desc;

    if (0x08U == (*id).bInterfaceClass) {
        printf("\r\n %s ",(uint8_t *)MSG_MSC_CLASS);
    } else if(0x03U == (*id).bInterfaceClass) {
        printf("\r\n %s ",(uint8_t *)MSG_HID_CLASS);
    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  manufacturer_string: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    printf("\r\n > Manufacture string is : %s ",(uint8_t *)manufacturer_string);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    printf("\r\n > Product string is : %s ",(uint8_t *)product_string);
}

/*!
    \brief      user operatin when serialNum string exists
    \param[in]  serial_num_string: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    printf("\r\n > Serial Number string is : %s ",(uint8_t *)serial_num_string);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    /* enumeration complete */
    uint8_t Str1[] = "> To start the HID class operations: ";
    uint8_t Str2[] = "> Press User Key...                  ";
    printf("\r\n %s ",(uint8_t *)MSG_DEV_ENUMERATED);
  
    printf("\r\n %s ",(uint8_t *)Str1);
    printf("\r\n %s ",(uint8_t *)Str2);
} 

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    printf("\r\n %s ",(uint8_t *)MSG_DEV_NOSUP);
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     none
*/
usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USBH_USER_NO_RESP;
    if (0U == user_input_flag) {
        printf("\r\n %s", (uint8_t *)MSG_WAIT_INPUT);
        user_input_flag = 1U;
    }
    /*key B3 is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_USER)) {
        usbh_usr_status = USBH_USER_RESP_OK;
        printf("\r\n %s", (uint8_t *)MSG_USER_INPUT);
    }

    return usbh_usr_status;
}

/*!
    \brief      user operation for device overcurrent detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
    printf("\r\n %s ",(uint8_t *)MSG_OVERCURRENT);
}

/*!
    \brief      init mouse window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_init (void)
{
    printf("\r\n %s ",(uint8_t *)USB_HID_MouseStatus);
}

/*!
    \brief      process mouse data
    \param[in]  data: mouse data to be displayed
    \param[out] none
    \retval     none
*/
void usr_mouse_process_data(hid_mouse_info *data)
{
    static uint8_t button_pressed_flag = 0;

    if ((data->x&0x80) == 0x00) {
        printf("\r\n MoveRight %x units",data->x);
    } else {
        printf("\r\n MoveLeft %x units",((uint8_t)~(data->x)+1));
    }
    if ((data->y&0x80) == 0x00) {
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
    \brief      init keyboard window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  usr_keybrd_init (void)
{
    printf("\r\n %s ",(uint8_t *)USB_HID_KeybrdStatus);
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

/*!
    \brief      deinit user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
}
