/*!
    \file    gd32e50x_lcd_eval.h
    \brief   LCD driver header file

    \version 2020-06-30, V1.0.0, demo for GD32E50x
    \version 2020-08-26, V1.1.0, demo for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef GD32E50X_LCD_EVAL_H
#define GD32E50X_LCD_EVAL_H

#include "gd32e50x.h"
#include "lcd_font.h"

extern font_struct font16x24;
extern font_struct font12x12;
extern font_struct font8x16;
extern font_struct font8x12;
extern font_struct font8x8;

#define LINE(x) ((x) * (((font_struct *)lcd_font_get())->height))

#define BANK0_LCD_D         ((uint32_t)0x65000000)    /*!< LCD data address */
#define BANK0_LCD_C         ((uint32_t)0x64000000)    /*!< LCD register address */

/* LCD registers */
#define R0                  0x00U
#define R1                  0x01U
#define R2                  0x02U
#define R3                  0x03U
#define R4                  0x04U
#define R5                  0x05U
#define R6                  0x06U
#define R7                  0x07U
#define R8                  0x08U
#define R9                  0x09U
#define R10                 0x0AU
#define R11                 0x0BU
#define R12                 0x0CU
#define R13                 0x0DU
#define R14                 0x0EU
#define R15                 0x0FU
#define R16                 0x10U
#define R17                 0x11U
#define R18                 0x12U
#define R19                 0x13U
#define R20                 0x14U
#define R21                 0x15U
#define R22                 0x16U
#define R23                 0x17U
#define R24                 0x18U
#define R25                 0x19U
#define R26                 0x1AU
#define R27                 0x1BU
#define R28                 0x1CU
#define R29                 0x1DU
#define R30                 0x1EU
#define R31                 0x1FU
#define R32                 0x20U
#define R33                 0x21U
#define R34                 0x22U
#define R35                 0x23U
#define R36                 0x24U
#define R37                 0x25U
#define R40                 0x28U
#define R41                 0x29U
#define R43                 0x2BU
#define R45                 0x2DU
#define R48                 0x30U
#define R49                 0x31U
#define R50                 0x32U
#define R51                 0x33U
#define R52                 0x34U
#define R53                 0x35U
#define R54                 0x36U
#define R55                 0x37U
#define R56                 0x38U
#define R57                 0x39U
#define R58                 0x3AU
#define R59                 0x3BU
#define R60                 0x3CU
#define R61                 0x3DU
#define R62                 0x3EU
#define R63                 0x3FU
#define R64                 0x40U
#define R65                 0x41U
#define R66                 0x42U
#define R67                 0x43U
#define R68                 0x44U
#define R69                 0x45U
#define R70                 0x46U
#define R71                 0x47U
#define R72                 0x48U
#define R73                 0x49U
#define R74                 0x4AU
#define R75                 0x4BU
#define R76                 0x4CU
#define R77                 0x4DU
#define R78                 0x4EU
#define R79                 0x4FU
#define R80                 0x50U
#define R81                 0x51U
#define R82                 0x52U
#define R83                 0x53U
#define R96                 0x60U
#define R97                 0x61U
#define R106                0x6AU
#define R118                0x76U
#define R128                0x80U
#define R129                0x81U
#define R130                0x82U
#define R131                0x83U
#define R132                0x84U
#define R133                0x85U
#define R134                0x86U
#define R135                0x87U
#define R136                0x88U
#define R137                0x89U
#define R139                0x8BU
#define R140                0x8CU
#define R141                0x8DU
#define R143                0x8FU
#define R144                0x90U
#define R145                0x91U
#define R146                0x92U
#define R147                0x93U
#define R148                0x94U
#define R149                0x95U
#define R150                0x96U
#define R151                0x97U
#define R152                0x98U
#define R153                0x99U
#define R154                0x9AU
#define R157                0x9DU
#define R192                0xC0U
#define R193                0xC1U
#define R229                0xE5U

/* LCD color */
#define LCD_COLOR_WHITE     0xFFFFU
#define LCD_COLOR_BLACK     0x0000U
#define LCD_COLOR_GREY      0xF7DEU
#define LCD_COLOR_BLUE      0x001FU
#define LCD_COLOR_BLUE2     0x051FU
#define LCD_COLOR_RED       0xF800U
#define LCD_COLOR_MAGENTA   0xF81FU
#define LCD_COLOR_GREEN     0x07E0U
#define LCD_COLOR_CYAN      0x7FFFU
#define LCD_COLOR_YELLOW    0xFFE0U

/* definitions of LCD lines */
#define LCD_LINE_0          0U
#define LCD_LINE_1          24U
#define LCD_LINE_2          48U
#define LCD_LINE_3          72U
#define LCD_LINE_4          96U
#define LCD_LINE_5          120U
#define LCD_LINE_6          144U
#define LCD_LINE_7          168U
#define LCD_LINE_8          192U
#define LCD_LINE_9          216U

#define CHAR_FONT_8_16                    ((uint16_t)0x0000U) /*!< the font of char is 8X16 */
#define CHAR_FONT_16_24                   ((uint16_t)0x0001U) /*!< the font of char is 16X24 */

#define CHAR_DIRECTION_HORIZONTAL         ((uint16_t)0x0000U) /*!< character display direction is horizontal */
#define CHAR_DIRECTION_VERTICAL           ((uint16_t)0x0001U) /*!< character display direction is vertical */

#define LCD_PIXEL_WIDTH                   ((uint16_t)240U)
#define LCD_PIXEL_HEIGHT                  ((uint16_t)320U)

/* char format struct definitions */
typedef struct
{   
    uint16_t font;                                            /*!< the type of font */
    uint16_t direction;                                       /*!< the direction of char */
    uint16_t char_color;                                      /*!< the color of char */
    uint16_t bk_color;                                        /*!< the color of backgroud */
}char_format_struct;

/* function declarations */
/* initializes the LCD of GD EVAL board */
void gd_eval_lcd_init(void);
/* LCD peripheral initialize */
void exmc_lcd_init(void);
/* initializes the LCD */
void lcd_init(void);
/* write data to the selected LCD register */
void lcd_register_write(uint16_t register_id,uint16_t value);
/* read the value of LCD register */
uint16_t lcd_register_read(uint8_t register_id);
/* write command to LCD register */
void lcd_command_write (uint16_t value);
/* prepare to write to the LCD GRAM */
void lcd_gram_write_prepare(void);
/* write RGB code to the LCD GRAM register */
void lcd_gram_write(uint16_t rgb_code);
/* read data from GRAM */
uint16_t lcd_gram_read(void);
/* clear the LCD with specified color */
void lcd_clear(uint16_t color);
/* set the text font */
void lcd_font_set(font_struct *fonts);
/* get the text font */
font_struct *lcd_font_get(void);
/* set the text color */
void lcd_text_color_set(__IO uint16_t color);
/* get the current text color */
uint16_t lcd_text_color_get(void);
/* set the background color */
void lcd_background_color_set(__IO uint16_t color);
/* get the current background color */
uint16_t lcd_background_color_get(void);
/* set the point according to the specified position and color */
void lcd_point_set(uint16_t x,uint16_t y,uint16_t point);
/* get the point according to the specified position */
uint16_t lcd_point_get(uint16_t x,uint16_t y);
/* draw a horizontal line on LCD screen */
void lcd_hline_draw(uint16_t x,uint16_t start_y,uint16_t end_y,uint16_t color,uint16_t width);
/* draw a rectangle according to the specified position and color */
void lcd_rectangle_draw(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,uint16_t point);
/* fill the specified color to a rectangle */
void lcd_rectangle_fill(uint16_t start_x,uint16_t start_y,uint16_t width,uint16_t height);
/* display a char on LCD screen according to the specified position */
void lcd_char_display(uint16_t x,uint16_t y,uint8_t c);
/* display the vertical character on LCD */
void lcd_vertical_char_display(uint16_t line, uint16_t column, uint8_t ascii);
/* display the vertical string on LCD */
void lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr);

#endif /* GD32E50X_LCD_EVAL_H */
