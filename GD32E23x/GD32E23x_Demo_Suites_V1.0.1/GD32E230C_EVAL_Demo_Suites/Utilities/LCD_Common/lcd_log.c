/*!
    \file  lcd_log.c
    \brief LCD log driver
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

/*!
    \brief      initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(BLACK);
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
void lcd_log_header_set (uint8_t *pTitle, uint16_t StartX)
{
    lcd_rect_color_draw(0, 0, 240, 40, BLUE);

    lcd_draw_font_gbk16(StartX, 10, RED, BLUE, (char *)pTitle);

}

/*!
    \brief      display the application footer (status) on the LCD screen 
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *pStatus, uint16_t StartX)
{
    lcd_rect_color_draw(0, 280, 240, 320, BLUE);

    lcd_draw_font_gbk16(StartX, 290, RED, BLUE, (char *)pStatus);

}

/**
  * @brief  Clear the Text Zone 
  * @param  None 
  * @retval None
  */
void lcd_log_text_zone_clear(uint16_t StartX,
                             uint16_t StartY,
                             uint16_t EndX,
                             uint16_t EndY)
{
    lcd_rect_color_draw(StartX, StartY, EndX, EndY, BLACK);
}

/**
  * @brief  Redirect the printf to the lcd 
  * @param  pStr: pointer to string to be displayed
  * @param  Len: string length
  * @retval None
  */
void lcd_log_print (uint8_t *pStr, 
                    uint16_t *pLine, 
                    uint16_t CharColor, 
                    uint16_t BackColor)
{
    if(*pLine >= 280)
    {
        *pLine = 40;

        lcd_rect_color_draw(0, 40, 240, 280, BLACK);
    }

    lcd_draw_font_gbk16(2, *pLine, CharColor, BackColor, (char *)pStr);

    *pLine += 20;
}

