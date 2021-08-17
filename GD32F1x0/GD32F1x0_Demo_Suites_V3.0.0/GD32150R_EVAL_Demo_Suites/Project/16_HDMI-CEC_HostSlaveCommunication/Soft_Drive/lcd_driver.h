/*!
    \file  lcd_driver.h
    \brief the header file of lcd driver

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdlib.h>
#include "gd32f1x0.h"

#define RED             (uint16_t)0xF800
#define GREEN           (uint16_t)0x07E0
#define BLUE            (uint16_t)0x001F
#define WHITE           (uint16_t)0xFFFF
#define BLACK           (uint16_t)0x0000
#define YELLOW          (uint16_t)0xFFE0

/* grays */
#define GRAY0           (uint16_t)0xEF7D
#define GRAY1           (uint16_t)0x8410
#define GRAY2           (uint16_t)0x4208

/* PF5 tft cs */
#define LCD_CS_SET      ((uint32_t)(GPIO_BOP(GPIOF) = GPIO_PIN_5))
#define LCD_CS_CLR      ((uint32_t)(GPIO_BC(GPIOF) = GPIO_PIN_5))

/* PC3 tft rs/dc */
#define LCD_RS_SET      ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_3))
#define LCD_RS_CLR      ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_3))

/* PC4 tft rst */
#define LCD_RST_SET     ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_4))
#define LCD_RST_CLR     ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_4))

/* lcd init */
void lcd_init(void);
/* clear the lcd */
void lcd_clear(uint16_t color);
/* set the start display point of lcd */
void lcd_set_xy(uint16_t x,uint16_t y);
/* draw a point on the lcd */
void gui_draw_point(uint16_t x,uint16_t y,uint16_t data);

#endif /* LCD_DRIVER_H */
