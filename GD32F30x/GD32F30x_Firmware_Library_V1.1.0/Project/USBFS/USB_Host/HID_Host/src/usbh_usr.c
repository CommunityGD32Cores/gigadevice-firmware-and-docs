/*!
    \file  usbh_usr.c
    \brief the user callback function definitions
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "lcd_log.h"
#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include <string.h>

#define KYBRD_FIRST_COLUMN         (uint16_t)0
#define KYBRD_LAST_COLUMN          (uint16_t)320
#define KYBRD_FIRST_LINE           (uint8_t)144
#define KYBRD_LAST_LINE            (uint8_t)36

uint8_t  KeybrdCharXpos = 0;
uint16_t KeybrdCharYpos = 0;
uint8_t  user_input_flag = 0;
uint8_t  button_pressed_flag = 0;

extern int16_t  x_loc, y_loc;
extern __IO int16_t prev_x, prev_y;

extern usb_core_handle_struct usbfs_core_dev;

uint8_t NeedClear = 0;

/* points to the DEVICE_PROP structure of current device */
usbh_user_callback_struct user_callback_funs =
{
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_descavailable,
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

uint16_t LINE = 190;
  
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

        gd_eval_lcd_init();

        lcd_log_init();

        lcd_log_header_set((uint8_t *)MSG_HOST_HEADER, 80);

        lcd_log_print((uint8_t *)MSG_LIB_START, 
                      sizeof(MSG_LIB_START) - 1, 
                      WHITE, BLACK);

        lcd_log_footer_set((uint8_t *)MSG_HOST_FOOTER, 60);
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
    if(NeedClear != 0)
    {
        lcd_log_textzone_clear(30, 0, 210, 320);
        NeedClear = 0;
    }

    lcd_log_print((uint8_t *)MSG_DEV_ATTACHED, sizeof(MSG_DEV_ATTACHED) - 1, WHITE, BLACK);
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    lcd_log_print((uint8_t *)MSG_UNREC_ERROR, sizeof(MSG_UNREC_ERROR) - 1, WHITE, BLACK);
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    lcd_log_textzone_clear(30, 0, 210, 320);

    lcd_rectangle_draw(50, 30, 180, 280, WHITE);

    LINE = 190;

    lcd_log_print((uint8_t *)MSG_DEV_DISCONNECTED, sizeof(MSG_DEV_DISCONNECTED) - 1, WHITE, BLACK);

    NeedClear = 1;
}

/*!
    \brief      user operation for reset USB device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    /* users can do their application actions here for the USB-Reset */
    lcd_log_print((uint8_t *)MSG_RESET_DEV, sizeof(MSG_RESET_DEV) - 1, WHITE, BLACK);
}

/*!
    \brief      user operation for detectting device speed
    \param[in]  device_speed: the device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint8_t device_speed)
{
    if (HPRT_PRTSPD_HIGH_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_HIGHSPEED, sizeof(MSG_DEV_HIGHSPEED) - 1, WHITE, BLACK);
    } else if (HPRT_PRTSPD_FULL_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_FULLSPEED, sizeof(MSG_DEV_FULLSPEED) - 1, WHITE, BLACK);
    } else if (HPRT_PRTSPD_LOW_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_LOWSPEED, sizeof(MSG_DEV_LOWSPEED) - 1, WHITE, BLACK);
    } else {
        lcd_log_print((uint8_t *)MSG_DEV_ERROR, sizeof(MSG_DEV_ERROR) - 1, WHITE, BLACK);
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  device_desc: the device descripter
    \param[out] none
    \retval     none
*/
void usbh_user_device_descavailable(void *device_desc)
{
    uint8_t TempStr[64];
    uint8_t Len = 0;
    usb_descriptor_device_struct *pDevStr = device_desc;

    sprintf((char *)TempStr, "VID: %04Xh", (uint32_t)pDevStr->idVendor);
    Len = strlen((const char *)TempStr);
    lcd_log_print((uint8_t *)TempStr, Len, WHITE, BLACK);

    sprintf((char *)TempStr, "PID: %04Xh" , (uint32_t)pDevStr->idProduct);
    Len = strlen((const char *)TempStr);
    lcd_log_print((uint8_t *)TempStr, Len, WHITE, BLACK);
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
void usbh_user_configuration_descavailable(usb_descriptor_configuration_struct *cfg_desc,
                                          usb_descriptor_interface_struct *itf_desc,
                                          usb_descriptor_endpoint_struct *ep_desc)
{
    usb_descriptor_interface_struct *id = itf_desc;

    if ((*id).bInterfaceClass == 0x08) {
        lcd_log_print((uint8_t *)MSG_MSC_CLASS, sizeof(MSG_MSC_CLASS) - 1, WHITE, BLACK);
    } else if((*id).bInterfaceClass == 0x03) {
        lcd_log_print((uint8_t *)MSG_HID_CLASS, sizeof(MSG_HID_CLASS) - 1, WHITE, BLACK);
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
    uint8_t TempStr[64];
    uint8_t Len = 0;

    sprintf((char *)TempStr, "Manufacturer: %s", (char *)manufacturer_string);
    Len = strlen((const char *)TempStr);
    lcd_log_print((uint8_t *)TempStr, Len, WHITE, BLACK);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    uint8_t TempStr[64];
    uint8_t Len = 0;

    sprintf((char *)TempStr, "Product: %s", (char *)product_string);
    Len = strlen((const char *)TempStr);
    lcd_log_print((uint8_t *)TempStr, Len, WHITE, BLACK);
}

/*!
    \brief      user operatin when serialNum string exists
    \param[in]  serial_num_string: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    uint8_t TempStr[64];
    uint8_t Len = 0;

    sprintf((char *)TempStr, "Serial Number: %s", (char *)serial_num_string);
    Len = strlen((const char *)TempStr);
    lcd_log_print((uint8_t *)TempStr, Len, WHITE, BLACK);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    /* Enumeration complete */
    uint8_t Str1[] = "To start the HID class operations: ";
    uint8_t Str2[] = "Press User Key...                  ";
    uint16_t i = 0, len = 0;

    char_format_struct chara_format =
    {
        .font = CHAR_FONT_8_16,
        .direction = CHAR_DIRECTION_HORIZONTAL,
        .char_color = GREEN,
        .bk_color = BLACK
    };

    lcd_log_print((uint8_t *)MSG_DEV_ENUMERATED, 
                  sizeof(MSG_DEV_ENUMERATED) - 1, 
                  WHITE, BLACK);

    len = sizeof(Str1) - 1;
    for (i = 0; i < len; i++) {
        lcd_char_display(60, 40 + 8 * i, Str1[i], chara_format);
    }

    len = sizeof(Str2) - 1;
    for (i = 0; i < len; i++) {
        lcd_char_display(44, 56 + 8 * i, Str2[i], chara_format);
    }
} 

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    lcd_log_print((uint8_t *)MSG_DEV_NOSUP, sizeof(MSG_DEV_NOSUP) - 1, WHITE, BLACK);
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     none
*/
usbh_user_status_enum usbh_user_userinput(void)
{
    usbh_user_status_enum usbh_usr_status = USBH_USER_NO_RESP;

    /*Key B3 is in polling mode to detect user action */
    if(gd_eval_key_state_get(KEY_USER) == RESET)
    {
        usbh_usr_status = USBH_USER_RESP_OK;
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
    lcd_log_print((uint8_t *)MSG_OVERCURRENT, sizeof(MSG_OVERCURRENT) - 1, WHITE, BLACK);
}

/*!
    \brief      init mouse window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_init (void)
{
    lcd_log_print((uint8_t *)USB_HID_MouseStatus, 
                  sizeof(USB_HID_MouseStatus) - 1, 
                  WHITE, BLACK);

    char_format_struct chara_format =
    {
        .font = CHAR_FONT_8_16,
        .direction = CHAR_DIRECTION_HORIZONTAL,
        .char_color = GREEN,
        .bk_color = BLACK
    };

    lcd_log_textzone_clear(30, 0, 210, 320);
    lcd_rectangle_draw(80, 40, 190, 280, YELLOW);

    lcd_rectangle_fill(40, 40, 70, 110, WHITE);
    lcd_rectangle_fill(40, 130, 70, 190, WHITE);
    lcd_rectangle_fill(40, 210, 70, 280, WHITE);

    hid_mouse_button_released(0);
    hid_mouse_button_released(1);
    hid_mouse_button_released(2);

    lcd_char_display(184, 44, 'x', chara_format);

    x_loc  = 0;
    y_loc  = 0; 
    prev_x = 0;
    prev_y = 0;
}

/*!
    \brief      process mouse data
    \param[in]  data: mouse data to be displayed
    \param[out] none
    \retval     none
*/
void usr_mouse_process_data(hid_mouse_data_struct *data)
{
    uint8_t idx = 1;
    static uint8_t b_state[3] = {0};

    if ((data->x != 0) && (data->y != 0))
    {
        hid_mouse_position_update(data->x, data->y);
    }

    for ( idx = 0; idx < 3; idx ++)
    {
        if(data->button & 1 << idx) 
        {
            if(b_state[idx] == 0)
            {
                hid_mouse_button_pressed (idx);
                b_state[idx] = 1;
            }
        }
        else
        {
            if(b_state[idx] == 1)
            {
                hid_mouse_button_released (idx);
                b_state[idx] = 0;
            }
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
    lcd_rectangle_fill(30, 0, 210, 320, BLACK);

    LINE = 190;

    lcd_log_print((uint8_t *)USB_HID_KeybrdStatus, 
                   sizeof(USB_HID_KeybrdStatus) - 1, 
                   WHITE, BLACK);

    lcd_log_print((uint8_t *)MSG_USE_KYBD, 
                   sizeof(MSG_USE_KYBD) - 1, 
                   WHITE, BLACK);

    lcd_hline_draw(150, 0, 320, GREEN, 2);

    KeybrdCharXpos = KYBRD_FIRST_LINE;
    KeybrdCharYpos = KYBRD_FIRST_COLUMN;

    lcd_hline_draw(30, 0, 320, GREEN, 2);
}

/*!
    \brief      process keyboard data
    \param[in]  data: keyboard data to be displayed
    \param[out] none
    \retval     none
*/
void usr_keybrd_process_data (uint8_t data)
{
    char_format_struct chara_format =
    {
        .font = CHAR_FONT_8_16,
        .direction = CHAR_DIRECTION_HORIZONTAL,
        .char_color = BLACK,
        .bk_color = BLACK
    };

    if(data == '\n')
    {
        KeybrdCharYpos = KYBRD_FIRST_COLUMN;

        /* Increment char X position */
        KeybrdCharXpos -= SMALL_FONT_LINE_WIDTH;
    }
    else if (data == '\r')
    {
        /* Manage deletion of charactter and upadte cursor location */
        if(KeybrdCharYpos == KYBRD_FIRST_COLUMN) 
        {
            /* First character of first line to be deleted */
            if(KeybrdCharXpos == KYBRD_FIRST_LINE)
            {
                KeybrdCharYpos = KYBRD_FIRST_COLUMN;
            }
            else
            {
                KeybrdCharXpos -= SMALL_FONT_LINE_WIDTH;
                KeybrdCharYpos =(KYBRD_LAST_COLUMN+SMALL_FONT_COLUMN_WIDTH); 
            }
        }
        else
        {
            KeybrdCharYpos += SMALL_FONT_COLUMN_WIDTH;
        }

        lcd_char_display(KeybrdCharXpos, KeybrdCharYpos, ' ', chara_format);
    }
    else
    {
        chara_format.char_color = WHITE;

        lcd_char_display(KeybrdCharXpos, KeybrdCharYpos, data, chara_format);

        /* Update the cursor position on LCD */

        /* Increment char Y position */
        KeybrdCharYpos += SMALL_FONT_COLUMN_WIDTH;

        /*Check if the Y position has reached the last column*/
        if(KeybrdCharYpos == KYBRD_LAST_COLUMN)
        {
            KeybrdCharYpos = KYBRD_FIRST_COLUMN;

            /*Increment char X position*/
            KeybrdCharXpos -= SMALL_FONT_LINE_WIDTH;
        }
    }

    if(KeybrdCharXpos <= KYBRD_LAST_LINE)
    {
        lcd_rectangle_fill(36, 0, 144, 320, BLACK);

        KeybrdCharXpos = KYBRD_FIRST_LINE;
    }
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
