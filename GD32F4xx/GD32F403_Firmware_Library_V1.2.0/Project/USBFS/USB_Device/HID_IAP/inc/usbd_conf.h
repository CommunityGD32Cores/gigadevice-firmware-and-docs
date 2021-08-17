/*!
    \file  usbd_conf.h
    \brief the header file of USBHS device-mode configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM                   1
#define USBD_ITF_MAX_NUM                   1
#define USB_STR_DESC_MAX_SIZE              64

/* USB feature -- Self Powered */
#define USBD_SELF_POWERED

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

#define USB_STRING_COUNT                   4

#define IAP_IN_EP                          EP1_IN
#define IAP_OUT_EP                         EP1_OUT

#define IAP_IN_PACKET                      24

#ifdef USBHS_CORE
    #ifdef USE_ULPI_PHY
        #define IAP_OUT_PACKET             1024
        #define TRANSFER_SIZE              1022  /* IAP maximum data packet size */
    #elif defined(USE_EMBEDDED_PHY)
        #define IAP_OUT_PACKET             64
        #define TRANSFER_SIZE              62
    #endif
#elif defined(USBFS_CORE)
    #define IAP_OUT_PACKET                 64
    #define TRANSFER_SIZE                  62
#endif

#define PAGE_SIZE                          2048

/* maximum number of supported memory media (Flash, RAM or EEPROM and so on) */
#define MAX_USED_MEMORY_MEDIA              1

/* memory address from where user application will be loaded, which represents 
   the DFU code protected against write and erase operations.*/
#define APP_LOADED_ADDR                    0x08004000

/* make sure the corresponding memory where the DFU code should not be loaded
   cannot be erased or overwritten by DFU application. */
#define IS_PROTECTED_AREA(addr)            (uint8_t)(((addr >= 0x08000000) && \
                                           (addr < (APP_LOADED_ADDR)))? 1 : 0)

/* base address of the FMC sectors */
#define ADDR_FMC_SECTOR_0       ((uint32_t)0x08000000) /*!< base address of sector 0,  16 kbytes */
#define ADDR_FMC_SECTOR_1       ((uint32_t)0x08004000) /*!< base address of sector 1,  16 kbytes */
#define ADDR_FMC_SECTOR_2       ((uint32_t)0x08008000) /*!< base address of sector 2,  16 kbytes */
#define ADDR_FMC_SECTOR_3       ((uint32_t)0x0800C000) /*!< base address of sector 3,  16 kbytes */
#define ADDR_FMC_SECTOR_4       ((uint32_t)0x08010000) /*!< base address of sector 4,  64 kbytes */
#define ADDR_FMC_SECTOR_5       ((uint32_t)0x08020000) /*!< base address of sector 5, 128 kbytes */
#define ADDR_FMC_SECTOR_6       ((uint32_t)0x08040000) /*!< base address of sector 6, 128 kbytes */
#define ADDR_FMC_SECTOR_7       ((uint32_t)0x08060000) /*!< base address of sector 7, 128 kbytes */
#define ADDR_FMC_SECTOR_8       ((uint32_t)0x08080000) /*!< base address of sector 8, 128 kbytes */
#define ADDR_FMC_SECTOR_9       ((uint32_t)0x080A0000) /*!< base address of sector 9, 128 kbytes */
#define ADDR_FMC_SECTOR_10      ((uint32_t)0x080C0000) /*!< base address of sector 10,128 kbytes */
#define ADDR_FMC_SECTOR_11      ((uint32_t)0x080E0000) /*!< base address of sector 11,128 kbytes */
#define ADDR_FMC_SECTOR_12      ((uint32_t)0x08100000) /*!< base address of sector 12, 16 kbytes */
#define ADDR_FMC_SECTOR_13      ((uint32_t)0x08104000) /*!< base address of sector 13, 16 kbytes */
#define ADDR_FMC_SECTOR_14      ((uint32_t)0x08108000) /*!< base address of sector 14, 16 kbytes */
#define ADDR_FMC_SECTOR_15      ((uint32_t)0x0810C000) /*!< base address of sector 15, 16 kbytes */
#define ADDR_FMC_SECTOR_16      ((uint32_t)0x08110000) /*!< base address of sector 16, 64 kbytes */
#define ADDR_FMC_SECTOR_17      ((uint32_t)0x08120000) /*!< base address of sector 17,128 kbytes */
#define ADDR_FMC_SECTOR_18      ((uint32_t)0x08140000) /*!< base address of sector 18,128 kbytes */
#define ADDR_FMC_SECTOR_19      ((uint32_t)0x08160000) /*!< base address of sector 19,128 kbytes */
#define ADDR_FMC_SECTOR_20      ((uint32_t)0x08180000) /*!< base address of sector 20,128 kbytes */
#define ADDR_FMC_SECTOR_21      ((uint32_t)0x081A0000) /*!< base address of sector 21,128 kbytes */
#define ADDR_FMC_SECTOR_22      ((uint32_t)0x081C0000) /*!< base address of sector 22,128 kbytes */
#define ADDR_FMC_SECTOR_23      ((uint32_t)0x081E0000) /*!< base address of sector 23,128 kbytes */
#define ADDR_FMC_SECTOR_24      ((uint32_t)0x08200000) /*!< base address of sector 24,256 kbytes */
#define ADDR_FMC_SECTOR_25      ((uint32_t)0x08240000) /*!< base address of sector 25,256 kbytes */
#define ADDR_FMC_SECTOR_26      ((uint32_t)0x08280000) /*!< base address of sector 26,256 kbytes */
#define ADDR_FMC_SECTOR_27      ((uint32_t)0x082C0000) /*!< base address of sector 27,256 kbytes */


#endif /* USBD_CONF_H */
