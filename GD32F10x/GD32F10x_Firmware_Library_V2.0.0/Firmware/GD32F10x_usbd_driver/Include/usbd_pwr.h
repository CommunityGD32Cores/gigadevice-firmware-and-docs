/*!
    \file  usbd_pwr.h
    \brief USB device power management functions prototype
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef USBD_PWR_H
#define USBD_PWR_H

#include "usbd_core.h"

/* function declarations */
/* USB wakeup first operation is to wakeup mcu */
void  resume_mcu (void);
/* set USB device to suspend mode */
void  usbd_suspend (void);
/* start to remote wakeup */
void  usbd_remote_wakeup_active (void);

#endif /* USBD_PWR_H */
