/*!
    \file    gd32f3x0_lcd_eval.h
    \brief   LCD driver header file

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
    \version 2022-01-06, V2.2.0, firmware for GD32F3x0
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#ifndef GD32F3X0_LCD_EVAL_H
#define GD32F3X0_LCD_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f350r_eval.h"

/* Colors */
#define RED             0xf800
#define GREEN           0x07e0
#define BLUE            0x001f
#define WHITE           0xffff
#define BLACK           0x0000
#define YELLOW          0xFFE0

/* GRAYs */
#define GRAY0           0xEF7D
#define GRAY1           0x8410
#define GRAY2           0x4208

/* PF5 tft cs */
#define LCD_CS_SET      ((uint32_t)(GPIO_BOP(GPIOF) = GPIO_PIN_5))
#define LCD_CS_CLR      ((uint32_t)(GPIO_BC(GPIOF) = GPIO_PIN_5))

/* PC3 tft rs/dc */
#define LCD_RS_SET      ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_3))
#define LCD_RS_CLR      ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_3))

/* PC4 tft rst */
#define LCD_RST_SET     ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_4))
#define LCD_RST_CLR     ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_4))

/* initialize the lcd */
void lcd_init(void);
/* set lcd display region */
void lcd_region_set(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
/* set the start display point of lcd */
void lcd_set_xy(uint16_t x, uint16_t y);
/* draw a point on the lcd */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t data);
/* clear the lcd */
void lcd_clear(uint16_t color);
/* convert bgr format to rgb format */
uint16_t lcd_bgr2rgb(uint16_t c);
/* draw a circle on the lcd */
void lcd_circle_draw(uint16_t x, uint16_t y, uint16_t r, uint16_t fc);
/* draw a line on the lcd */
void lcd_line_draw(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
/* draw a rectangle on the lcd */
void lcd_rect_draw(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t line_color);
/* draw a box2 on the lcd */
void lcd_box2(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t mode);
/* draw a rectangle with color on the lcd */
void lcd_rect_color_draw(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t fc);
/* display button down */
void display_button_down(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
/* display button up */
void display_button_up(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
/* draw gbk16 font on the lcd */
void lcd_draw_font_gbk16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s);
/* draw gbk24 font on the lcd */
void lcd_draw_font_gbk24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s);
/* draw num32 font on the lcd */
void lcd_draw_font_num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num);

#ifdef __cplusplus
}
#endif

#endif /* GD32F3X0_LCD_EVAL_H */
