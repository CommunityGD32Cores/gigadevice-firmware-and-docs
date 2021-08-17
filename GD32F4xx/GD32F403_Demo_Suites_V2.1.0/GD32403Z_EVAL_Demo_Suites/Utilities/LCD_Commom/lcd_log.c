/*!
    \file  lcd_log.c
    \brief this file provides all the LCD Log firmware functions

    \version 2017-02-10, V1.0.0, firmware for GD32F403
    \version 2018-10-10, V1.1.0, firmware for GD32F403
    \version 2018-12-25, V2.0.0, firmware for GD32F403
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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
#include "usb_conf.h"
#include "usb_delay.h"

extern uint16_t LINE;

#define LCD_ILI9320       0x8989
#define LCD_ILI9325       0x9325

/*!
    \brief      initializes the LCD of GD EVAL board
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gd_eval_lcd_init(void)
{
    __IO uint32_t lcd_id = 0;

    exmc_lcd_init();

    delay_ms(50);

    /* Read the LCD ID */
    lcd_id = lcd_register_read(R0);

    /* check if the LCD is ILI9320 controller */
    if (LCD_ILI9320 == lcd_id) {
        lcd_init();
    }
}

/*!
    \brief      initialize the LCD Log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(BLACK);
}

/*!
    \brief      de-initialize the LCD Log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
    /* no operation */
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  ptitle: pointer to the string to be displayed
    \param[in]  start_x: x location
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *ptitle, uint16_t start_x)
{
    int i = 0;
    int len = strlen((const char *)ptitle);

    char_format_struct chara_format =
    {
        .font = CHAR_FONT_8_16,
        .direction = CHAR_DIRECTION_HORIZONTAL,
        .char_color = RED,
        .bk_color = BLUE
    };

    lcd_rectangle_fill(210, 0, 240, 320, BLUE);

    for(i = 0; i < len; i++) {
        lcd_char_display(230, (start_x + 8 * i), ptitle[i], chara_format);
    }
}

/*!
    \brief      display the application footer (status) on the LCD screen
    \param[in]  pstatus: pointer to the string to be displayed
    \param[in]  start_x: x location
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *pstatus, uint16_t start_x)
{
    int i = 0;
    int Len = strlen((const char *)pstatus);

    char_format_struct chara_format =
    {
        .font = CHAR_FONT_8_16,
        .direction = CHAR_DIRECTION_HORIZONTAL,
        .char_color = RED,
        .bk_color = BLUE
    };

    lcd_rectangle_fill(0, 0, 30, 320, BLUE);

    for (i = 0; i < Len; i++) {
        lcd_char_display(20, (start_x + 8 * i), pstatus[i], chara_format);
    }
}

/*!
    \brief      clear the text zone
    \param[in]  start_x, start_y, end_x, end_y: zone location
    \param[out] none
    \retval     none
*/
void lcd_log_textzone_clear (uint16_t start_x,
                             uint16_t start_y,
                             uint16_t end_x,
                             uint16_t end_y)
{
    lcd_rectangle_fill(start_x, start_y, end_x, end_y, BLACK);
}

/*!
    \brief      redirect the printf to the LCD
    \param[in]  pstr: pointer to string to be displayed
    \param[in]  len: string length
    \param[in]  char_color: string color
    \param[in]  back_color: back color
    \param[out] none
    \retval     none
*/
void lcd_log_print (uint8_t *pstr, 
                    uint16_t len, 
                    uint16_t char_color, 
                    uint16_t back_color)
{
    uint16_t i;

    char_format_struct chara_format =
    {
        .font = CHAR_FONT_8_16,
        .direction = CHAR_DIRECTION_HORIZONTAL,
        .char_color = char_color,
        .bk_color = back_color
    };

#if defined(USE_HOST_MODE) && defined(USE_DEVICE_MODE)
    if (LINE <= 60) {
        LINE = 190;
        lcd_rectangle_fill(60, 0, 210, 320, BLACK);
    }
#else
    #if defined USBH_CDC
        if (LINE <= 60) {
            LINE = 190;
            lcd_rectangle_fill(60, 0, 210, 320, BLACK);
        }
    #elif defined USBD_CDC
        if (LINE <= 40) {
            LINE = 190;
            lcd_rectangle_fill(40, 0, 210, 320, BLACK);
        }
    #else
        if (LINE <= 30) {
            LINE = 190;
            lcd_rectangle_fill(30, 0, 210, 320, BLACK);
        }
    #endif
#endif

    for (i = 0; i < len; i ++) {
        lcd_char_display(LINE, (10 + 8 * i), *pstr++, chara_format);
    }

    LINE -= 20;
}
