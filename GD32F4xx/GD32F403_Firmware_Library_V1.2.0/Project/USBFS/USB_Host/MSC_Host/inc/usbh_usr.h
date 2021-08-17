/*!
    \file  usbh_usr.h
    \brief user application layer header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBH_USR_H
#define USBH_USR_H

#include "ff.h"
#include "usbh_core.h"
#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "usb_std.h"

#include <stdio.h>

extern  usbh_user_callback_struct usr_cb;

/* state machine for the usbh_usr_application_state */
#define USBH_USR_FS_INIT             0
#define USBH_USR_FS_READLIST         1
#define USBH_USR_FS_WRITEFILE        2
#define USBH_USR_FS_DEMOEND          3

extern  uint8_t usbh_usr_application_state;

/* function declarations */
/* user operation for host-mode initialization */
void usbh_user_init (void);
/* de-int user state and associated variables */
void usbh_user_deinit (void);
/* user operation for device attached */
void usbh_user_device_connected (void);
/* user operation for reset USB Device */
void usbh_user_device_reset (void);
/* user operation for device disconnect event */
void usbh_user_device_disconnected (void);
/* user operation for device overcurrent detection event */
void usbh_user_over_current_detected (void);
/* user operation for detectting device speed */
void usbh_user_device_speed_detected (uint8_t device_speed); 
/* user operation when device descriptor is available */
void usbh_user_device_desc_available (void *dev_desc);
/* USB device is successfully assigned the address */
void usbh_user_device_address_assigned (void);
/* user operation when configuration descriptor is available */
void usbh_user_configuration_descavailable (usb_descriptor_configuration_struct *cfg_desc,
                                            usb_descriptor_interface_struct *itf_desc,
                                            usb_descriptor_endpoint_struct *ep_desc);
/* user operation when manufacturer string exists */
void usbh_user_manufacturer_string (void *mfc_string);
/* user operation when product string exists */
void usbh_user_product_string (void *prod_string);
/* user operatin when serialNum string exists */
void usbh_user_serialnum_string (void *serialnum_string);
/* user response request is displayed to ask for application jump to class */
void usbh_user_enumeration_finish (void);
/* user action for application state entry */
usbh_user_status_enum usbh_user_userinput (void);
/* user operation when device is not supported */
void usbh_user_device_not_supported (void);
/* user operation when unrecoveredError happens */
void usbh_user_unrecovered_error (void);
/* demo application for mass storage */
int usbh_usr_msc_application (usb_core_handle_struct *pudev, uint8_t id);

#endif /* USBH_USR_H */
