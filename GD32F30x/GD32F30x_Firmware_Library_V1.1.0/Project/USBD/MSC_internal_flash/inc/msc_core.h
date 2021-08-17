/*!
    \file  usbd_hid_core.h
    \brief the header file of USB MSC device class core functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#ifndef MSC_CORE_H
#define MSC_CORE_H

#include "usbd_core.h"
#include "usbd_std.h"

#define STORAGE_LUN_NUM                   1

#define MSC_EPIN_SIZE                     MSC_DATA_PACKET_SIZE
#define MSC_EPOUT_SIZE                    MSC_DATA_PACKET_SIZE

#define BBB_GET_MAX_LUN                   0xFE
#define BBB_RESET                         0xFF

#define USB_LANGID_STRING_SIZE            0x04
#define USB_MSC_CONFIG_DESC_SIZE          32
#define USB_STR_DESC_MAX_SIZE             64

/* USB configuration descriptor struct */
typedef struct
{
    usb_descriptor_configuration_struct        Config;

    usb_descriptor_interface_struct            MSC_Interface;
    usb_descriptor_endpoint_struct             MSC_INEndpoint;
    usb_descriptor_endpoint_struct             MSC_OUTEndpoint;
} usb_descriptor_configuration_set_struct;

extern void* const usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descripter;
extern usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the MSC device */
usbd_status_enum msc_init (void *pudev, uint8_t config_index);
/* de-initialize the MSC device */
usbd_status_enum msc_deinit (void *pudev, uint8_t config_index);
/* handle the MSC class-specific and standard requests */
usbd_status_enum msc_req_handler (void *pudev, usb_device_req_struct *req);
/* handle data stage */
usbd_status_enum msc_data_handler (void *pudev, usbd_dir_enum rx_tx, uint8_t ep_id);

#endif  /* MSC_CORE_H */
