/*!
    \file  usbd_msc_core.h
    \brief the header file of USB MSC device class core functions
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-10-19, V1.0.0, demo for GD32F403
*/

#ifndef USBD_MSC_CORE_H
#define USBD_MSC_CORE_H

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
    usb_descriptor_configuration_struct        config;

    usb_descriptor_interface_struct            msc_interface;
    usb_descriptor_endpoint_struct             msc_in_endpoint;
    usb_descriptor_endpoint_struct             msc_out_endpoint;
} usb_descriptor_configuration_set_struct;

extern uint8_t* usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descripter;
extern const usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the MSC device */
uint8_t msc_init (void *pudev, uint8_t config_index);
/* de-initialize the MSC device */
uint8_t msc_deinit (void *pudev, uint8_t config_index);
/* handle the MSC class-specific and standard requests */
uint8_t msc_req_handler (void *pudev, usb_device_req_struct *req);
/* handle data stage */
uint8_t msc_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_id);

#endif  /* USBD_MSC_CORE_H */
