/*!
    \file  tft_test.c
    \brief tft lcd test functions
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
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

#include "gd32e230c_lcd_eval.h"
#include "tft_test.h"
#include "systick.h"
unsigned char num[10] = {0,1,2,3,4,5,6,7,8,9};

/*!
    \brief      number display test
    \param[in]  none
    \param[out] none
    \retval     none
*/
void num_test(void)
{
    uint8_t i = 0;
    lcd_clear(BLUE);
    lcd_draw_font_gbk24(16,20,YELLOW,BLUE,"Num Display Test");

    for(i = 0;i < 10;i ++){
        lcd_draw_font_num32(50 + (i % 3) * 40,32 * (i / 3) + 50,WHITE,BLUE,num[i + 1]);
        delay_ms(100);
    }

    delay_ms(1000);
}

/*!
    \brief      font display test
    \param[in]  none
    \param[out] none
    \retval     none
*/
void font_test(void)
{
    __O  uint8_t x_offset;

#ifdef H_VIEW
    x_offset = 40;
#else
    x_offset = 0;
#endif

    lcd_clear(BLUE);

    /* lcd display mcu informations */
    lcd_draw_font_gbk16(2,10,WHITE,BLUE," GigaDevice Semiconductor Inc.");
    lcd_draw_font_gbk16(2,30,WHITE,BLUE,"  -- GD32E230 Series MCU --   ");
    
    lcd_draw_font_gbk16(2,50,WHITE,BLUE,"     GD32E230C_EAVL_V1.0  ");

    lcd_draw_font_gbk16(x_offset,60 + 16,WHITE,BLUE,"     ---- GD32E230 ----      ");

    lcd_draw_font_gbk16(x_offset,60 + 32, WHITE,BLUE,"         GD32E230CxT6      ");
    lcd_draw_font_gbk16(x_offset,60 + 48, WHITE,BLUE,"         GD32E230KxT6      ");
    lcd_draw_font_gbk16(x_offset,60 + 64, WHITE,BLUE,"         GD32E230KxU6      ");
    lcd_draw_font_gbk16(x_offset,60 + 80, WHITE,BLUE,"         GD32E230GxU6      ");
    lcd_draw_font_gbk16(x_offset,60 + 96, WHITE,BLUE,"         GD32E230FxP6      ");
    lcd_draw_font_gbk16(x_offset,60 + 112,WHITE,BLUE,"         GD32E230FxH6      ");

    lcd_draw_font_gbk24(x_offset,80 + 200,WHITE,BLUE,"  Smarter! Faster! Cheaper!  ");

    delay_ms(1000);

    /* lcd display Chinese characters */
    lcd_clear(BLUE);
    lcd_draw_font_gbk16(16,10,WHITE,BLUE,"        ²âÊÔ   ");
    lcd_draw_font_gbk24(16,30,WHITE,BLUE,"       ²âÊÔ   "); 

    lcd_draw_font_gbk16(16,50,WHITE,BLUE,"        ²âÊÔ   ");
    lcd_draw_font_gbk24(16,70,WHITE,BLUE,"       ²âÊÔ   "); 

    delay_ms(3000);
}

/*!
    \brief      color display test
    \param[in]  none
    \param[out] none
    \retval     none
*/
void color_test(void)
{
    __O  uint8_t i=2;

    while(i--){
        /* lcd display color test */
        lcd_clear(WHITE);
        lcd_draw_font_gbk16(20,50,BLACK,WHITE,"Color Display Test");
        lcd_draw_font_gbk16(100,100,BLACK,WHITE,"White");
        delay_ms(1000);
        
        /* lcd display color test */
        lcd_clear(BLACK);
        lcd_draw_font_gbk16(20,50,WHITE,BLACK,"Color Display Test");
        lcd_draw_font_gbk16(100,100,WHITE, BLACK,"Black");
        delay_ms(1000);
       
        /* lcd display color test */
        lcd_clear(RED);
        lcd_draw_font_gbk16(20,50,WHITE,RED,"Color Display Test");
        lcd_draw_font_gbk16(100,100,WHITE,RED,"Red");
        delay_ms(1000);

        /* lcd display color test */
        lcd_clear(GREEN);
        lcd_draw_font_gbk16(20,50,BLUE,GREEN,"Color Display Test");
        lcd_draw_font_gbk16(100,100,BLUE,GREEN,"Green");
        delay_ms(1000);

        /* lcd display color test */
        lcd_clear(BLUE);
        lcd_draw_font_gbk16(20,50,YELLOW,BLUE,"Color Display Test");
        lcd_draw_font_gbk16(100,100,YELLOW,BLUE,"Blue");
        delay_ms(1000);

        /* lcd display color test */
        lcd_clear(YELLOW);
        lcd_draw_font_gbk16(20,50,BLUE,YELLOW,"Color Display Test");
        lcd_draw_font_gbk16(100,100,BLUE,YELLOW,"Yellow");
        delay_ms(1000);
    }
}

/*!
    \brief      gui draw test
    \param[in]  none
    \param[out] none
    \retval     none
*/
void draw_test(void)
{
    __O  uint8_t i=0;

    lcd_clear(BLUE);
    lcd_draw_font_gbk16(2,20,WHITE, BLUE,"  GUI Draw Test ");
    /* gui box */
    lcd_box(60,90,160,160,GREEN);
    /* gui box2 */
    lcd_box2(20,160,200,120,0);
    lcd_box2(25,165,205,125,0);
    lcd_box2(30,170,210,130,0);
    /* gui rect */
    lcd_rect_color_draw(10,50,110,70,YELLOW);
    /* gui circle */
    lcd_circle_draw(120,160,80,RED);
   
    delay_ms(1000);
}
