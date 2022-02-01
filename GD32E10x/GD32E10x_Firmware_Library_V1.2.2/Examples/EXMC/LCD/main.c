/*!
    \file    main.c
    \brief   LCD test demo
    
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
    \version 2020-09-30, V1.1.0, firmware for GD32E10x
    \version 2020-12-31, V1.2.0, firmware for GD32E10x
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

#include "gd32e10x.h"
#include "picture.h"
#include "gd32e10x_lcd_eval.h"

void draw_picture(void);

uint8_t gd_website_string[]={"www.gigadevice.com"} ;
char_format_struct char_format;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LCD of GD EVAL board */
    gd_eval_lcd_init();
    /* draw picture on LCD */
    draw_picture();
    
    while(1){
    }
}

/*!
    \brief      draw picture on LCD
    \param[in]  none
    \param[out] none
    \retval     none
*/
void draw_picture(void)
{  
    uint16_t i;
  
    /* clear the LCD screen */
    lcd_clear(WHITE);    
    /* draw the picture of Gigadevice logo */
    lcd_picture_draw(40,100,40+160-1,100+87-1,(uint16_t *)(picture + BMP_HEADSIZE));
    /* draw a rectangle */
    lcd_rectangle_draw(10,10,230,310,BLUE);

    /* configure char format */
    char_format.char_color = BLUE;
    char_format.bk_color = WHITE;
    char_format.direction = CHAR_DIRECTION_VERTICAL;
    char_format.font = CHAR_FONT_8_16;
    
    /* draw character on LCD screen */
    for (i = 0; i < sizeof(gd_website_string)-1; i++){
        lcd_char_display((50+8*i), 180+20, *(gd_website_string+i),char_format);
    }
}  
