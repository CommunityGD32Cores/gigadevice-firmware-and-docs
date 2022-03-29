/*!
    \file    lcd_log.c
    \brief   LCD log driver

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

#include "lcd_log.h"

/*!
    \brief      initialize the lcd log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init(void)
{
    lcd_clear(BLACK);
}

/*!
    \brief      deinitialize the lcd log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit(void)
{
    /* no operation */
}

/*!
    \brief      display the application header (title) on the lcd screen
    \param[in]  p_title: pointer to the string to be displayed
    \param[in]  start_x: the x position of the start point
    \param[out] none
    \retval     none
*/
void lcd_log_header_set(uint8_t *p_title, uint16_t start_x)
{
    lcd_rect_color_draw(0, 0, 240, 40, BLUE);

    lcd_draw_font_gbk16(start_x, 10, RED, BLUE, (char *)p_title);

}

/*!
    \brief      display the application footer (status) on the LCD screen
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the x position of the start point
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set(uint8_t *p_status, uint16_t start_x)
{
    lcd_rect_color_draw(0, 280, 240, 320, BLUE);

    lcd_draw_font_gbk16(start_x, 290, RED, BLUE, (char *)p_status);

}

/*!
    \brief      redirect the printf to the lcd
    \param[in]  p_str: pointer to string to be displayed
    \param[in]  len: the length of string
    \param[in]  charcolor: the color of string
    \param[in]  backcolor: the color of background
    \param[out] none
    \retval     none
*/
void lcd_log_print(uint8_t *p_str,
                   uint16_t *len,
                   uint16_t charcolor,
                   uint16_t backcolor)
{
    if(*len >= 280) {
        *len = 40;

        lcd_rect_color_draw(0, 40, 240, 280, BLACK);
    }

    lcd_draw_font_gbk16(2, *len, charcolor, backcolor, (char *)p_str);

    *len += 20;
}

/*!
    \brief      clear the text zone
    \param[in]  start_x: the x position of the start point
    \param[in]  start_y: the y position of the start point
    \param[in]  end_x: the x position of the end point
    \param[in]  end_y: the y position of the end point
    \param[out] none
    \retval     none
*/
void lcd_log_text_zone_clear(uint16_t start_x,
                             uint16_t start_y,
                             uint16_t end_x,
                             uint16_t end_y)
{
    lcd_rect_color_draw(start_x, start_y, end_x, end_y, BLACK);
}
