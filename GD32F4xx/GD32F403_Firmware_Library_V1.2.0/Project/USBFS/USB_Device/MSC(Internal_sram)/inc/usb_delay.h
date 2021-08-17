/*!
    \file  usb_delay.h
    \brief usb delay driver header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USB_DELAY_H
#define USB_DELAY_H

#include "usb_core.h"

#define TIM_MSEC_DELAY                          0x01
#define TIM_USEC_DELAY                          0x02

/* function declarations */
void timer_nvic_init(void);
void delay_us(uint32_t time_us);
void delay_ms(uint32_t time_ms);
void timer_delay_irq (void);

#endif /* USB_DELAY_H */
