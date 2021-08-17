/*!
    \file    lcd_log.c
    \brief   LCD log driver

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
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

#include "lcd_log.h"
#include "gd32f20x_lcd_eval.h"

uint16_t LINE;

/*!
    \brief      initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(LCD_COLOR_WHITE);
    
    lcd_font_set(&Font8x16);
    
    lcd_text_color_set(LCD_COLOR_RED);
    
    lcd_background_color_set(LCD_COLOR_WHITE);
}

/*!
    \brief      de-initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  p_title: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x)
{
    lcd_text_color_set(LCD_COLOR_BLUE2);

    lcd_rectangle_fill(0, 0, 30, 272);

    lcd_text_color_set(LCD_COLOR_RED);

    lcd_vertical_string_display(8, start_x, p_title);
    LINE = 30;
}

/*!
    \brief      display the application footer (status) on the LCD screen 
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x)
{
    lcd_text_color_set(LCD_COLOR_BLUE2);

    lcd_rectangle_fill(450, 0, 480, 272);

    lcd_text_color_set(LCD_COLOR_RED);

    lcd_vertical_string_display(458, start_x, p_status);
}

/*!
    \brief      clear the text zone 
    \param[in]  start_x: the start x position
    \param[in]  start_y: the start y position
    \param[in]  width: the width to clear text zone
    \param[in]  height: the heitht to clear text zone
    \param[out] none
    \retval     none
*/
void lcd_log_text_zone_clear(uint16_t start_x,
                             uint16_t start_y,
                             uint16_t width,
                             uint16_t height)
{
    lcd_rectangle_fill(start_x, start_y, width, height);
    LINE = 30;
}

/*!
    \brief      redirect the printf to the lcd 
    \param[in]  p_str: pointer to string to be displayed
    \param[in]  offset: the offset to set
    \param[in]  char_color: the clar color to set
    \param[out] none
    \retval     none
*/
void lcd_log_print (uint8_t *p_str, uint16_t offset, uint16_t char_color)
{
#if defined(USE_HOST_MODE) && defined(USE_DEVICE_MODE)
    if(LINE >= 400)
    {
        LINE = 40;

        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_rectangle_fill(30, 0, 390, 272);
    }
#else
    if(LINE >= 440)
    {
        LINE = 40;

        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_rectangle_fill(30, 0, 420, 272);
    }
#endif

    lcd_text_color_set(char_color);

    lcd_vertical_string_display(LINE, offset, p_str);

    LINE += 20;
}
