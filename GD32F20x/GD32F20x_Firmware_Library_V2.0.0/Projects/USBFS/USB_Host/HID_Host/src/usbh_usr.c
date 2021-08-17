/*!
    \file  usbh_usr.c
    \brief the user callback function definitions
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include "gd32f207i_lcd_eval.h"
#include "lcd_log.h"
#include <string.h>

#define SMALL_FONT_COLUMN_WIDTH    8
#define SMALL_FONT_LINE_WIDTH      16

#define KYBRD_FIRST_COLUMN         (uint16_t)264
#define KYBRD_LAST_COLUMN          (uint16_t)0
#define KYBRD_FIRST_LINE           (uint8_t)70
#define KYBRD_LAST_LINE            (uint16_t)272

uint16_t keybrd_char_xpos = 0;
uint16_t keybrd_char_ypos = 0;
uint8_t  need_clear = 0;
uint8_t  user_input_flag = 0;
uint8_t  button_pressed_flag = 0;

extern int16_t  x_loc, y_loc;
extern __IO int16_t prev_x, prev_y;
extern uint16_t LINE;

extern usb_core_handle_struct usbfs_core_dev;

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
const uint8_t MSG_HOST_FOOTER[]        = "USB Host Library v2.0.0";

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
        lcd_init();
        lcd_layer_init(LCD_LAYER_FOREGROUND, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);
        lcd_layer_init(LCD_LAYER_BACKGROUND, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);

        tli_layer_enable(LAYER0);
        tli_layer_enable(LAYER1);
        tli_enable();

        lcd_layer_set(LCD_LAYER_BACKGROUND);
        lcd_transparency_set(255);
        lcd_clear(LCD_COLOR_WHITE);

        lcd_layer_set(LCD_LAYER_FOREGROUND);
        lcd_transparency_set(0);
        lcd_clear(LCD_COLOR_WHITE);

        lcd_log_init();
        lcd_layer_set(LCD_LAYER_BACKGROUND);
        lcd_log_header_set((uint8_t *)MSG_HOST_HEADER, 60);

        lcd_log_print((uint8_t *)MSG_LIB_START, 0, LCD_COLOR_BLACK);

        lcd_log_footer_set((uint8_t *)MSG_HOST_FOOTER, 40);
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
    if (need_clear != 0) {
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_log_text_zone_clear(30, 0, 420, 272);
        need_clear = 0;
    }

    lcd_log_print((uint8_t *)MSG_DEV_ATTACHED, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    lcd_log_print((uint8_t *)MSG_UNREC_ERROR, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    LINE = 30;
    lcd_text_color_set(LCD_COLOR_WHITE);
    lcd_log_text_zone_clear(30, 0, 420, 272);

    lcd_log_print((uint8_t *)MSG_DEV_DISCONNECTED, 0, LCD_COLOR_BLACK);

    need_clear = 1;
}

/*!
    \brief      user operation for reset USB device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    lcd_log_print((uint8_t *)MSG_RESET_DEV, 0, LCD_COLOR_BLACK);
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
        lcd_log_print((uint8_t *)MSG_DEV_HIGHSPEED, 0, LCD_COLOR_BLACK);
    } else if (HPRT_PRTSPD_FULL_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_FULLSPEED, 0, LCD_COLOR_BLACK);
    } else if (HPRT_PRTSPD_LOW_SPEED == device_speed) {
        lcd_log_print((uint8_t *)MSG_DEV_LOWSPEED, 0, LCD_COLOR_BLACK);
    } else {
        lcd_log_print((uint8_t *)MSG_DEV_ERROR, 0, LCD_COLOR_BLACK);
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
    usb_descriptor_device_struct *pDevStr = device_desc;

    sprintf((char *)TempStr, "VID: %04Xh", (uint32_t)pDevStr->idVendor);
    lcd_log_print((uint8_t *)TempStr, 0, LCD_COLOR_BLACK);

    sprintf((char *)TempStr, "PID: %04Xh" , (uint32_t)pDevStr->idProduct);
    lcd_log_print((uint8_t *)TempStr, 0, LCD_COLOR_BLACK);
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

    if (0x08U  == (*id).bInterfaceClass) {
        lcd_log_print((uint8_t *)MSG_MSC_CLASS, 0, LCD_COLOR_BLACK);
    } else if (0x03U  == (*id).bInterfaceClass) {
        lcd_log_print((uint8_t *)MSG_HID_CLASS, 0, LCD_COLOR_BLACK);
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

    sprintf((char *)TempStr, "Manufacturer: %s", (char *)manufacturer_string);
    lcd_log_print((uint8_t *)TempStr, 0, LCD_COLOR_BLACK);
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

    sprintf((char *)TempStr, "Product: %s", (char *)product_string);
    lcd_log_print((uint8_t *)TempStr, 0, LCD_COLOR_BLACK);
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

    sprintf((char *)TempStr, "Serial Number: %s", (char *)serial_num_string);
    lcd_log_print((uint8_t *)TempStr, 0, LCD_COLOR_BLACK);
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
    uint8_t Str1[] = "To start the HID class operations: ";
    uint8_t Str2[] = "Press User Key...                  ";

    lcd_log_print((uint8_t *)MSG_DEV_ENUMERATED, 0, LCD_COLOR_BLACK);
    lcd_log_print((uint8_t *)Str1, 0, LCD_COLOR_RED);
    lcd_log_print((uint8_t *)Str2, 0, LCD_COLOR_RED);
} 

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    lcd_log_print((uint8_t *)MSG_DEV_NOSUP, 0, LCD_COLOR_BLACK);
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
    if (0U == user_input_flag) {
        lcd_log_print((uint8_t *)MSG_WAIT_INPUT, 0, LCD_COLOR_BLACK);
        user_input_flag = 1U;
    }
    /*key B3 is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_USER)) {
        LINE = 190;
        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_log_text_zone_clear(30, 0, 420, 272);
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
    lcd_log_print((uint8_t *)MSG_OVERCURRENT, 0, LCD_COLOR_BLACK);
}

/*!
    \brief      init mouse window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_init (void)
{
    lcd_text_color_set(LCD_COLOR_GREEN);
    lcd_rectangle_fill(30, 0, 420, 272);
  
    lcd_log_print((uint8_t *)USB_HID_MouseStatus, 0, LCD_COLOR_BLACK);
    lcd_text_color_set(LCD_COLOR_GREY);
    lcd_rectangle_fill(50, 0, 190, 280);
  
    lcd_rectangle_fill(280, 15, 40, 60);
    lcd_rectangle_fill(280, 105, 40, 60);
    lcd_rectangle_fill(280, 195, 40, 60);
  
    lcd_text_color_set(LCD_COLOR_BLACK);
    lcd_vertical_char_display(50, 0, 'x');

    hid_mouse_button_released(0);
    hid_mouse_button_released(1);
    hid_mouse_button_released(2);

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

    if ((data->x != 0) && (data->y != 0)) {
        hid_mouse_update_position(data->x, data->y);
    }

    for ( idx = 0; idx < 3; idx ++) {
        if (data->button & 1 << idx) {
            if (b_state[idx] == 0) {
                hid_mouse_button_pressed(idx);
                b_state[idx] = 1;
            }
        } else {
            if (b_state[idx] == 1) {
                hid_mouse_button_released(idx);
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
    LINE = 30;

    lcd_log_print((uint8_t *)USB_HID_KeybrdStatus, 0, LCD_COLOR_BLACK);

    lcd_log_print((uint8_t *)MSG_USE_KYBD, 0, LCD_COLOR_BLACK);
  
    lcd_text_color_set(LCD_COLOR_GREY);
    lcd_rectangle_fill(70, 0, 210, 320);
    
    LINE = 70;

    keybrd_char_xpos = KYBRD_FIRST_LINE;
    keybrd_char_ypos = KYBRD_FIRST_COLUMN;
}

/*!
    \brief      process keyboard data
    \param[in]  data: keyboard data to be displayed
    \param[out] none
    \retval     none
*/
void usr_keybrd_process_data (uint8_t data)
{
    if (data == '\n') {
        keybrd_char_ypos = KYBRD_FIRST_COLUMN;

        /* increment char x position */
        keybrd_char_xpos += SMALL_FONT_LINE_WIDTH;
    } else if (data == '\r') {
        /* manage deletion of charactter and upadte cursor location */
        if (keybrd_char_ypos == KYBRD_FIRST_COLUMN) {
            /* first character of first line to be deleted */
            if (keybrd_char_xpos == KYBRD_FIRST_LINE) {
                keybrd_char_ypos = KYBRD_FIRST_COLUMN;
            } else {
                keybrd_char_xpos += SMALL_FONT_LINE_WIDTH;
                keybrd_char_ypos =(KYBRD_LAST_COLUMN+SMALL_FONT_COLUMN_WIDTH); 
            }
        } else {
            keybrd_char_ypos -= SMALL_FONT_COLUMN_WIDTH;
        }

        lcd_text_color_set(LCD_COLOR_BLACK);
        lcd_vertical_char_display(keybrd_char_xpos, keybrd_char_ypos, ' ');
    } else {
        lcd_text_color_set(LCD_COLOR_BLACK);
        lcd_vertical_char_display(keybrd_char_xpos, keybrd_char_ypos, data);

        /* update the cursor position on lcd */
        /* check if the y position has reached the last column*/
        if(keybrd_char_ypos == KYBRD_LAST_COLUMN) {
            keybrd_char_ypos = KYBRD_FIRST_COLUMN;

            /* increment char X position */
            keybrd_char_xpos += SMALL_FONT_LINE_WIDTH;
        } else {
            /* increment char Y position */
            keybrd_char_ypos -= SMALL_FONT_COLUMN_WIDTH;
        }
        
    }

    if (keybrd_char_xpos >= KYBRD_LAST_LINE) {
        lcd_text_color_set(LCD_COLOR_GREY);
        lcd_rectangle_fill(70, 0, 210, 320);

        keybrd_char_xpos = KYBRD_FIRST_LINE;
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
