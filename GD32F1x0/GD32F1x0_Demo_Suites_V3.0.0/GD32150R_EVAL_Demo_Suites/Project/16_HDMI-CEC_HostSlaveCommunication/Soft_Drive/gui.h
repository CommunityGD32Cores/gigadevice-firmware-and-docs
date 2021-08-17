/*!
    \file  gui.h
    \brief the header file of gui

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

#ifndef GUI_H
#define GUI_H

#include "gd32f1x0.h"
#include <stdlib.h>

/* bgr to rgb format conversion */
uint16_t lcd_bgr2rgb(uint16_t c);
/* gui circle */
void gui_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t fc);
/* gui draw line */
void gui_draw_line(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t color);
/* gui box */
void gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc);
/* gui box2 */
void gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode);
/* gui rect */
void gui_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t fc);
/* display button down */
void display_button_down(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/* display button up */
void display_button_up(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/* gui draw font to gbk16 */
void gui_draw_font_gbk16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s);
/* gui draw font to gbk24 */
void gui_draw_font_gbk24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s);
/* gui draw font to num32 */
void gui_draw_font_num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num);

#endif /* GUI_H */
