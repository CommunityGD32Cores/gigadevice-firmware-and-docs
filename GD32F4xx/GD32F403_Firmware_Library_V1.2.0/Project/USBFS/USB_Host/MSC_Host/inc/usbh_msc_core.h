/*!
    \file  usbh_msc_core.h
    \brief this file contains all the prototypes for the usbh_msc_core.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBH_MSC_CORE_H
#define USBH_MSC_CORE_H

#include "usbh_core.h"
#include "usbh_ctrl.h"
#include "usbh_hcs.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

/* structure for MSC process */
typedef struct
{
    uint8_t              hc_num_in; 
    uint8_t              hc_num_out; 
    uint8_t              msc_bulk_out_ep;
    uint8_t              msc_bulk_in_ep;
    uint16_t             msc_bulk_in_ep_size;
    uint16_t             msc_bulk_out_ep_size;
    uint8_t              buff[USBH_MSC_MPS_SIZE];
    uint8_t              maxLun;
}msc_machine_struct; 

typedef struct
{
    usbh_status_enum (*class_req_polling)  (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate);
    usbh_status_enum (*class_polling)      (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate);
}class_polling_fun_cb_struct;

#define USB_REQ_BOT_RESET           0xFFU
#define USB_REQ_GET_MAX_LUN         0xFEU

extern msc_machine_struct           msc_machine;
extern uint8_t                      msc_error_count;

/* function declarations */
/* de-initialize interface by freeing host channels allocated to interface */
extern void usbh_msc_interface_deinit (usb_core_handle_struct *pudev, void *puhost);
/* interface initialization for MSC class */
extern usbh_status_enum usbh_msc_interface_init (usb_core_handle_struct *pudev, void *puhost);
/* clear or disable a specific feature. */
extern usbh_status_enum usbh_clear_feature (usb_core_handle_struct *pudev,
                                            usbh_host_struct *puhost,
                                            uint8_t ep_addr, 
                                            uint8_t hc_num);
#endif  /* USBH_MSC_CORE_H */
