/*!
    \file    usb_conf.h
    \brief   general low level driver configuration

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#ifndef __USB_CONF_H
#define __USB_CONF_H

#include <stdlib.h>
#include "gd32e50x.h"
#include "gd32e507z_eval.h"
#include "gd32e50x_lcd_eval.h"
#include "lcd_log.h"
#include "usb_lcd_conf.h"

#define USBH_KEEP_CFG_DESCRIPTOR           0U

/****************** USB OTG FS PHY CONFIGURATION *******************************
 *  The USB OTG FS Core supports one on-chip Full Speed PHY.
 *  The USE_EMBEDDED_PHY symbol is defined in the project compiler preprocessor 
 *  when FS core is used.
*******************************************************************************/

/* on-chip full-speed USB PHY */
#ifdef USE_USB_FS
    #define OC_FS_PHY
#endif

/* on-chip high-speed USB PHY */
#ifdef USE_USB_HS
    #define OC_HS_PHY
#endif /* USE_USB_HS */

/*******************************************************************************
 *                 FIFO Size Configuration in Host mode
 *
 *  (i) Receive data FIFO size = (Largest Packet Size / 4) + 1 or 
 *                               2x (Largest Packet Size / 4) + 1,  If a 
 *                               high-bandwidth channel or multiple isochronous
 *                               channels are enabled.
 *
 *  (ii) For the host nonperiodic Transmit FIFO is the largest maximum packet size 
 *       for all supported nonperiodic OUT channels. Typically, a space 
 *       corresponding to two Largest Packet Size is recommended.
 *
 *  (iii) The minimum amount of RAM required for Host periodic Transmit FIFO is 
 *        the largest maximum packet size for all supported periodic OUT channels.
 *        If there is at least one High Bandwidth Isochronous OUT endpoint, 
 *        then the space must be at least two times the maximum packet size for 
 *        that channel.
*******************************************************************************/

/****************** USB OTG FS CONFIGURATION **********************************/

#define USB_RX_FIFO_SIZE                      512U
#define USB_HTX_NPFIFO_SIZE                   256U
#define USB_HTX_PFIFO_SIZE                    256U

#ifdef OC_FS_PHY
     #define USB_EMBEDDED_FS_PHY_ENABLED
#elif defined(OC_HS_PHY)
     #define USB_EMBEDDED_HS_PHY_ENABLED
#else
     #error "PHY is not selected"
#endif

#define USB_INTERNAL_DMA_ENABLED

#define USB_SOF_OUTPUT                          0U
#define USB_LOW_POWER                           0U

/* if uncomment it, need jump to USB JP */
//#define VBUS_SENSING_ENABLED

/****************** USB MODE CONFIGURATION ********************************/
#define USE_HOST_MODE
//#define USE_DEVICE_MODE
//#define USE_OTG_MODE

#ifndef OC_FS_PHY
    #ifndef OC_HS_PHY
        #error  "OC_FS_PHY or OC_HS_PHY should be defined!"
    #endif
#endif

#ifndef USE_DEVICE_MODE
    #ifndef USE_HOST_MODE
        #error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
    #endif
#endif

#ifndef USE_USB_HS
    #ifndef USE_USB_FS
        #error  "USE_USB_HS or USE_USB_FS should be defined"
    #endif
#endif

/****************** C Compilers dependant keywords ****************************/
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */

#ifdef USB_INTERNAL_DMA_ENABLED
    #if defined   (__GNUC__)            /* GNU Compiler */
        #define __ALIGN_END __attribute__ ((aligned (4)))
        #define __ALIGN_BEGIN
    #else
        #define __ALIGN_END
        #if defined   (__CC_ARM)        /* ARM Compiler */
            #define __ALIGN_BEGIN __align(4)
        #elif defined (__ICCARM__)      /* IAR Compiler */
            #define __ALIGN_BEGIN
        #elif defined  (__TASKING__)    /* TASKING Compiler */
            #define __ALIGN_BEGIN __align(4) 
        #endif /* __CC_ARM */
    #endif /* __GNUC__ */
#else
    #define __ALIGN_BEGIN
    #define __ALIGN_END   
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined   ( __GNUC__ )   /* GNU Compiler */
    #ifndef __packed
        #define __packed __attribute__ ((__packed__))
    #endif
#elif defined   (__TASKING__)  /* TASKING Compiler */
    #define __packed __unaligned
#endif /* __GNUC__ */

#endif /* __USB_CONF_H */
