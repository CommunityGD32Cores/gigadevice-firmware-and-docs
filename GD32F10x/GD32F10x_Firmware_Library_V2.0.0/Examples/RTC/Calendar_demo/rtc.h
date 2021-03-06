/*!
    \file  rtc.h
    \brief headfile of RTC check and config,time_show and time_adjust function
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef __RTC_H
#define __RTC_H

#include "gd32f10x.h"
#include <stdio.h>

void nvic_configuration(void);
void rtc_configuration(void);
uint32_t time_regulate(void);
void time_adjust(void);
void time_display(uint32_t timevar);
void time_show(void);
uint8_t usart_scanf(uint32_t value);

#endif /* __RTC_H */

