/*!
    \file  usb_conf.h
    \brief general low level driver configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef USB_CONF_H
#define USB_CONF_H

#include "gd32f20x.h"
#include "gd32f207i_eval.h"

#ifdef USE_USBFS
    #define USBFS_CORE
#endif

#ifdef USBFS_CORE
    #define USBFS_RX_FIFO_SIZE                            128U
    #define USBFS_HTX_NPFIFO_SIZE                         96U
    #define USBFS_HTX_PFIFO_SIZE                          96U

//  #define USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
//  #define USB_OTG_FS_SOF_OUTPUT_ENABLED
#endif

#define USE_HOST_MODE
//#define USE_DEVICE_MODE
//#define USE_OTG_MODE

#ifndef USBFS_CORE
    #ifndef USBHS_CORE
        #error "USBHS_CORE or USBFS_CORE should be defined"
    #endif
#endif

#ifndef USE_DEVICE_MODE
    #ifndef USE_HOST_MODE
        #error "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
    #endif
#endif

#ifndef USE_USBHS
    #ifndef USE_USBFS
        #error "USE_USBHS or USE_USBFS should be defined"
    #endif
#endif

/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */
#ifdef USBHS_INTERNAL_DMA_ENABLED
    #if defined   (__GNUC__)            /* GNU Compiler */
        #define __ALIGN_END __attribute__ ((aligned(4)))
        #define __ALIGN_BEGIN
    #else
        #define __ALIGN_END
        #if defined   (__CC_ARM)        /* ARM Compiler */
            #define __ALIGN_BEGIN __align(4)
        #elif defined (__ICCARM__)      /* IAR Compiler */
            #define __ALIGN_BEGIN
        #elif defined  (__TASKING__)    /* TASKING Compiler */
            #define __ALIGN_BEGIN __align(4)
        #endif                          /* __CC_ARM */
    #endif                              /* __GNUC__ */
#else
    #define __ALIGN_BEGIN
    #define __ALIGN_END   
#endif /* USBHS_INTERNAL_DMA_ENABLED */

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
    #define __packed    __packed
#elif defined (__ICCARM__)     /* IAR Compiler */
    #define __packed    __packed
#elif defined   ( __GNUC__ )   /* GNU Compiler */
    #define __packed    __attribute__ ((__packed__))
#elif defined   (__TASKING__)  /* TASKING Compiler */
    #define __packed    __unaligned
#endif /* __CC_ARM */

#endif /* USB_CONF_H */
