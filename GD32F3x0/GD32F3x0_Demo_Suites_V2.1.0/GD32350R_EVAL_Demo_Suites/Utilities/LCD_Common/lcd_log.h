/*!
    \file    lcd_log.h
    \brief   the header file of LCD log

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2021-05-31, V2.1.0, firmware for GD32F3x0
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

#ifndef LCD_LOG_H
#define LCD_LOG_H

#include "gd32f3x0_lcd_eval.h"

/* initialize the LCD log module */
void lcd_log_init (void);
/* de-initialize the LCD log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen */
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x);
/* redirect the printf to the lcd */
void lcd_log_print (uint8_t *pStr, uint16_t *pLine, uint16_t CharColor, uint16_t BackColor);
/* clear the text zone */
void lcd_log_text_zone_clear (uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY);

#endif /* LCD_LOG_H */

