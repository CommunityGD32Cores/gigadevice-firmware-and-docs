/*!
    \file  usbh_msc_scsi.h
    \brief header file for usbh_msc_scsi.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBH_MSC_SCSI_H
#define USBH_MSC_SCSI_H

#include <stdint.h>
#include "usb_core.h"
#include "usbh_core.h"

typedef enum
{
    USBH_MSC_OK = 0U,
    USBH_MSC_FAIL,
    USBH_MSC_PHASE_ERROR,
    USBH_MSC_BUSY
}usbh_msc_status_enum;

typedef enum
{
    CMD_UNINITIALIZED_STATE = 0U,
    CMD_SEND_STATE,
    CMD_WAIT_STATUS
}cmd_states_enum;

typedef struct
{
    uint32_t msc_capacity;
    uint32_t msc_sense_key;
    uint16_t msc_page_length;
    uint8_t msc_bulk_out_ep;
    uint8_t msc_bulk_in_ep;
    uint8_t msc_write_protect;
}msc_param_struct;

#define OPCODE_TEST_UNIT_READY            0x00U
#define OPCODE_READ_CAPACITY10            0x25U
#define OPCODE_MODE_SENSE6                0x1AU
#define OPCODE_READ10                     0x28U
#define OPCODE_WRITE10                    0x2AU
#define OPCODE_REQUEST_SENSE              0x03U

#define DESC_REQUEST_SENSE                0x00U
#define ALLOCATION_LENGTH_REQUEST_SENSE   63U
#define XFER_LEN_READ_CAPACITY10          8U
#define XFER_LEN_MODE_SENSE6              63U

#define MASK_MODE_SENSE_WRITE_PROTECT     0x80U
#define MODE_SENSE_PAGE_CONTROL_FIELD     0x00U
#define MODE_SENSE_PAGE_CODE              0x3FU
#define DISK_WRITE_PROTECTED              0x01U

extern msc_param_struct usbh_msc_param;

/* function definition */
/* send'test unit ready' command to the device */
uint8_t usbh_msc_test_unit_ready (usb_core_handle_struct *pudev);
/* send the read capacity command to the device */
uint8_t usbh_msc_read_capacity10 (usb_core_handle_struct *pudev);
/* send the mode sense6 command to the device */
uint8_t usbh_msc_mode_sense6 (usb_core_handle_struct *pudev);
/* send the request sense command to the device */
uint8_t usbh_msc_request_sense (usb_core_handle_struct *pudev);
/* send the write command to the device */
uint8_t usbh_msc_write10 (usb_core_handle_struct *pudev,
                          uint8_t *data_buffer,
                          uint32_t address,
                          uint32_t nb_of_bytes);
/* send the read command to the device */
uint8_t usbh_msc_read10 (usb_core_handle_struct *pudev,
                         uint8_t *data_buffer,
                         uint32_t address,
                         uint32_t nb_of_bytes);

#endif  /* USBH_MSC_SCSI_H */

