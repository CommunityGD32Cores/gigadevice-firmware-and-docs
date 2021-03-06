/*!
    \file  usbd_hid_core.h
    \brief the header file of USB DFU device class core functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef DFU_CORE_H
#define DFU_CORE_H

#include "usbd_std.h"

/* manifestation State */
#define MANIFEST_COMPLETE       0x00
#define MANIFEST_IN_PROGRESS    0x01

#define USB_DFU_DESC_SIZE       9

/* special commands with download request */
#define GET_COMMANDS            0x00
#define SET_ADDRESS_POINTER     0x21
#define ERASE                   0x41

/* memory operation command */
#define CMD_ERASE               0
#define CMD_WRITE               1

#define FLASH_ERASE_TIMEOUT     60
#define FLASH_WRITE_TIMEOUT     80

/* bit detach capable = bit 3 in bmAttributes field */
#define DFU_DETACH_MASK               (uint8_t)(0x10)

#define USB_SERIAL_STRING_SIZE        0x06
#define DEVICE_ID                     (0x40022100)

#define USB_DFU_CONFIG_DESC_SIZE      (18 + (9 * USBD_ITF_MAX_NUM))
#define DFU_DESC_TYPE                 0x21


/* DFU device state defines */
typedef enum
{
    STATE_appIDLE = 0x00,
    STATE_appDETACH,
    STATE_dfuIDLE,
    STATE_dfuDNLOAD_SYNC,
    STATE_dfuDNBUSY,
    STATE_dfuDNLOAD_IDLE,
    STATE_dfuMANIFEST_SYNC,
    STATE_dfuMANIFEST,
    STATE_dfuMANIFEST_WAIT_RESET,
    STATE_dfuUPLOAD_IDLE,
    STATE_dfuERROR
}dfu_state_enum;

/* DFU device status defines */
typedef enum
{
    STATUS_OK = 0x00,
    STATUS_errTARGET,
    STATUS_errFILE,
    STATUS_errWRITE,
    STATUS_errERASE,
    STATUS_errCHECK_ERASED,
    STATUS_errPROG,
    STATUS_errVERIFY,
    STATUS_errADDRESS,
    STATUS_errNOTDONE,
    STATUS_errFIRMWARE,
    STATUS_errVENDOR,
    STATUS_errUSBR,
    STATUS_errPOR,
    STATUS_errUNKNOWN,
    STATUS_errSTALLEDPKT
}dfu_status_enum;

/* DFU class-specific requests */
typedef enum
{
    DFU_DETACH = 0,
    DFU_DNLOAD,
    DFU_UPLOAD,
    DFU_GETSTATUS,
    DFU_CLRSTATUS,
    DFU_GETSTATE,
    DFU_ABORT,
    DFU_REQ_MAX
}dfu_requests_enum;

#pragma pack(1)

/* USB dfu function descriptor struct */
typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */
    uint8_t bmAttributes;                 /*!< DFU attributes */
    uint16_t wDetachTimeOut;              /*!< time, in milliseconds, that the device will wait after receipt of the DFU_DETACH request. If */ 
    uint16_t wTransferSize;               /*!< maximum number of bytes that the device can accept per control-write transaction */
    uint16_t bcdDFUVersion;               /*!< numeric expression identifying the version of the DFU Specification release. */
} usb_dfu_function_descriptor_struct;

#pragma pack()

/* USB configuration descriptor struct */
typedef struct
{
    usb_descriptor_configuration_struct        Config;
    usb_descriptor_interface_struct            DFU_Interface;
    usb_dfu_function_descriptor_struct         DFU_Function_Desc;
} usb_descriptor_configuration_set_struct;

typedef  void  (*pAppFunction) (void);

extern uint8_t* usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descripter;
extern const usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the MSC device */
uint8_t dfu_init (void *pudev, uint8_t config_index);
/* de-initialize the MSC device */
uint8_t dfu_deinit (void *pudev, uint8_t config_index);
/* handle the MSC class-specific requests */
uint8_t dfu_req_handler (void *pudev, usb_device_req_struct *req);
/* handle data Stage */
uint8_t dfu_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_num);

#endif  /* DFU_CORE_H */
