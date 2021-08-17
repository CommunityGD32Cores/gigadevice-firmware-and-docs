/*!
    \file    usbh_usr.c
    \brief   USB host user callbacks

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#ifndef __USBH_USR_H
#define __USBH_USR_H

#include "usbh_core.h"
#include "usb_conf.h"
#include "usbh_core.h"
#include <stdio.h>

#define NB_BAUD_RATES                     10
#define NB_OF_FILES                       3

#define LCD_LOG_TEXT_COLOR            LCD_COLOR_WHITE
#define LCD_LOG_DEFAULT_COLOR         LCD_COLOR_WHITE

#define LCD_TEXT_ZONE_HEIGHT          LCD_PIXEL_HEIGHT

#define LCD_HEADER_LINE               LCD_HEADER_X + LCD_TEXT_OFFSET
#define LCD_FOOTER_LINE               LCD_FOOTER_X + LCD_TEXT_OFFSET

#define LCD_MENU_LINE0                190
#define LCD_MENU_LINE1                170
#define LCD_MENU_LINE2                150
#define LCD_MENU_LINE3                130
#define LCD_MENU_LINE4                110
#define LCD_MENU_LINE5                90
#define LCD_MENU_LINE6                70

#define LCD_CURSOR_LINE0              LCD_MENU_LINE2
#define LCD_CURSOR_LINE1              LCD_MENU_LINE4
#define LCD_CURSOR_LINE2              LCD_MENU_LINE6

#define LCD_MENU_Y                    20

#define LCD_MENU_ZONE_X               LCD_MENU_LINE6-40
#define LCD_MENU_ZONE_Y               0
#define LCD_MENU_ZONE_WIDTH           140
#define LCD_MENU_ZONE_HEIGHT          LCD_PIXEL_HEIGHT

#define START_LINE                    210

#define LCD_HINT_LINE                 LCD_MENU_LINE6

#define LCD_LINE_UPDATE(line)         ((line) -= 20)

typedef enum {
    CDC_DEMO_IDLE = 0,
    CDC_DEMO_WAIT,
    CDC_DEMO_SEND,
    CDC_DEMO_RECEIVE,
    CDC_DEMO_CONFIGURATION,
} cdc_demo_state;

typedef enum {
    CDC_SEND_IDLE = 0,
    CDC_SEND_WAIT,
    CDC_SEND_FILE1,
    CDC_SEND_FILE2,
    CDC_SEND_FILE3,
} cdc_send_state;

typedef enum {
    CDC_RECEIVE_IDLE = 0,
    CDC_RECEIVE_WAIT,
    CDC_RECEIVE_RECEIVE,
} cdc_receive_state;

typedef enum {
    CDC_CONFIGURATION_IDLE = 0,
    CDC_CONFIGURATION_WAIT,
    CDC_CONFIGURATION_SET_BAUD_RATE,
    CDC_CONFIGURATION_SET_DATA_BITS,
    CDC_CONFIGURATION_SET_PARITY,
    CDC_CONFIGURATION_SET_STOP_BIT,
} cdc_configuration_state;

typedef struct _demo_state_machine
{
  __IO cdc_demo_state              state;
  __IO cdc_send_state              Send_state;
  __IO cdc_receive_state           Receive_state;
  __IO cdc_configuration_state     Configuration_state;    
  __IO uint8_t                     select;
  __IO uint8_t                     lock;
} cdc_demo_state_machine;

typedef struct _DemoSettings
{
  __IO uint8_t                     BaudRateIdx;
  __IO uint8_t                     DataBitsIdx;
  __IO uint8_t                     ParityIdx;
  __IO uint8_t                     StopBitsIdx;
  
}CDC_DEMO_Settings;

typedef struct _DemoSettingStateMachine
{
  
  CDC_DEMO_Settings                settings;
  __IO uint8_t                     select;
  __IO uint8_t                     lock;
  
}cdc_demo_setting_state_machine;

typedef enum 
{
  CDC_SELECT_MENU   = 0,
  CDC_SELECT_CONFIG,    
}cdc_demo_select_mode;

extern usbh_user_cb usr_cb;
extern uint8_t USBH_USR_ApplicationState;

/* function declarations */
/* user operation for host-mode initialization */
void usbh_user_init(void);
/* deinit user state and associated variables */
void usbh_user_deinit(void);
/* user operation for device attached */
void usbh_user_device_connected(void);
/* user operation when unrecovered error happens */
void usbh_user_unrecovered_error(void);
/* user operation for device disconnect event */
void usbh_user_device_disconnected(void);
/* user operation for reset USB Device */
void usbh_user_device_reset(void);
/* user operation for detecting device speed */
void usbh_user_device_speed_detected(uint32_t DeviceSpeed);
/* user operation when device descriptor is available */
void usbh_user_device_desc_available(void *device_desc);
/* USB device is successfully assigned the Address */
void usbh_user_device_address_assigned(void);
/* user operation when configuration descriptor is available */
void usbh_user_configuration_descavailable(usb_desc_config *cfgDesc,
                                           usb_desc_itf *itfDesc,
                                           usb_desc_ep *epDesc);
/* user operation when manufacturer string exists */
void usbh_user_manufacturer_string(void *manufacturer_string);
/* user operation when product string exists */
void usbh_user_product_string(void *product_string);
/* user operation when serialNum string exists */
void usbh_user_serialnum_string(void *serial_num_string);
/* user response request is displayed to ask for application jump to class */
void usbh_user_enumeration_finish(void);
/* user operation when device is not supported */
void usbh_user_device_not_supported(void);
/* user action for application state entry */
usbh_user_status usbh_user_userinput(void);
/* user operation for device overcurrent detection event */
void usbh_user_over_current_detected(void);
/* demo application for mass storage */
int usbh_user_application(void);

#endif /*__USBH_USR_H */
