/*!
    \file    rtc.h
    \brief   headfile of RTC
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#ifndef RTC_H
#define RTC_H

#include "gd32c10x.h"
#include <stdio.h>

#define DIGIT_TO_CHAR_OFFSET    (48)
#define MIN_YEAR                (1951)
#define YEAR_SPAN               (100)
#define MAX_YEAR                (MIN_YEAR + YEAR_SPAN - 1)
#define IS_LEAP_YEAR(year)      ((year % 4 == 0) && ( year % 100 != 0) || (year % 400 == 0))

void nvic_configuration(void);
void rtc_configuration(void);
uint32_t time_regulate(void);
void time_adjust(void);
uint32_t usart_scanf(uint32_t max_value);
void print_calendar_on_lcd(uint32_t rtc_counter);
uint32_t calc_days_num_by_date(uint32_t year, uint32_t month, uint32_t day);

#endif /* RTC_H */
