/*!
    \file  printer_core.h
    \brief the header file of USB printer device class core functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef PRINTER_CORE_H
#define PRINTER_CORE_H

#include "usbd_std.h"

#define DEVICE_ID_LEN                   103

#define USB_PRINTER_CONFIG_DESC_SIZE    32

#define GET_DEVICE_ID                   0x00
#define GET_PORT_STATUS                 0x01
#define SOFT_RESET                      0x02

/* USB configuration descriptor struct */
typedef struct
{
    usb_descriptor_configuration_struct        Config;
    usb_descriptor_interface_struct            Printer_Interface;
    usb_descriptor_endpoint_struct             Printer_IN_Endpoint;
    usb_descriptor_endpoint_struct             Printer_OUT_Endpoint;
} usb_descriptor_configuration_set_struct;

extern uint8_t* usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descripter;
extern const usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the printer device */
uint8_t printer_init         (void *pudev, uint8_t config_index);
/* de-initialize the printer device */
uint8_t printer_deinit       (void *pudev, uint8_t config_index);
/* handle the printer class-specific requests */
uint8_t printer_req_handler  (void *pudev, usb_device_req_struct *req);
/* handle data Stage */
uint8_t printer_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_id);

#endif  /* PRINTER_CORE_H */
