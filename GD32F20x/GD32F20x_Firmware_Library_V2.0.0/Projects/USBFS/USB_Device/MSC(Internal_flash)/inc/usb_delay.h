/*!
    \file  usb_delay.h
    \brief usb delay driver header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
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
