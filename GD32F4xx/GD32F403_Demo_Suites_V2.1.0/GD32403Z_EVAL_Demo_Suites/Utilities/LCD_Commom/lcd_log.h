/*!
    \file  lcd_log.h
    \brief header for the lcd_log.c file

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

#ifndef  __LCD_LOG_H
#define  __LCD_LOG_H

#include "gd32f403Z_lcd_eval.h"
#include <string.h>

/* initializes the LCD of GD EVAL board */
void gd_eval_lcd_init(void);
/* initialize the LCD Log module */
void lcd_log_init (void);
/* de-initialize the LCD Log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen  */
void lcd_log_header_set (uint8_t *ptitle, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *pstatus, uint16_t start_x);
/* redirect the printf to the LCD */
void lcd_log_print (uint8_t *pstr, 
                    uint16_t len, 
                    uint16_t char_color, 
                    uint16_t back_color);
/* clear the text zone */
void lcd_log_textzone_clear (uint16_t start_x, 
                             uint16_t start_y, 
                             uint16_t end_x, 
                             uint16_t end_y);


#endif /* __LCD_LOG_H */ 

