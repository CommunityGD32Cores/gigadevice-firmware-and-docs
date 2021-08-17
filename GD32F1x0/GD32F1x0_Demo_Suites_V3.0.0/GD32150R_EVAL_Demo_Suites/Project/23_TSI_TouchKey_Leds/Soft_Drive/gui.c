/*!
    \file  gui.c
    \brief gui display functions

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

#include "gd32f1x0.h"
#include "lcd_driver.h"
#include "gui.h"
#include "systick.h"
#include "font.h"

/*!
    \brief      bgr to rgb format conversion
    \param[in]  c: bgr color value
    \param[out] none
    \retval     rgb color value
*/
uint16_t lcd_bgr2rgb(uint16_t c)
{
    uint16_t r,g,b,rgb;
    b = (c>>0) & 0x1f;
    g = (c>>5) & 0x3f;
    r = (c>>11) & 0x1f;
    rgb = (b<<11) + (g<<5) + (r<<0);
    return(rgb);
}

/*!
    \brief      gui circle
    \param[in]  x: the x position of the start point 
    \param[in]  y: the y position of the start point
    \param[in]  r: the radius of circle
    \param[in]  fc: lcd display color
    \param[out] none
    \retval     none
*/
void gui_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t fc)
{
    unsigned short a,b;
    int c;
    a = 0;
    b = r;
    c = 3 - 2*r;
    LCD_CS_CLR;

    while(a<b){
        /* draw points on the lcd */
        gui_draw_point(x+a, y+b, fc);
        gui_draw_point(x-a, y+b, fc);
        gui_draw_point(x+a, y-b, fc);
        gui_draw_point(x-a, y-b, fc);
        gui_draw_point(x+b, y+a, fc);
        gui_draw_point(x-b, y+a, fc);
        gui_draw_point(x+b, y-a, fc);
        gui_draw_point(x-b, y-a, fc);

        if(c < 0)
            c = c+4*a+6;
        else{ 
            c = c+4*(a-b)+10;
            b -= 1; 
        } 
        a += 1;
    } 
    if(a == b){
        /* draw points on the lcd */
        gui_draw_point(x+a, y+b, fc);
        gui_draw_point(x+a, y+b, fc);
        gui_draw_point(x+a, y-b, fc);
        gui_draw_point(x-a, y-b, fc);
        gui_draw_point(x+b, y+a, fc);
        gui_draw_point(x-b, y+a, fc);
        gui_draw_point(x+b, y-a, fc);
        gui_draw_point(x-b, y-a, fc);
    } 
    LCD_CS_SET;
}

/*!
    \brief      gui draw line
    \param[in]  x0: the x position of the start point
    \param[in]  y0: the y position of the start point
    \param[in]  x1: the x position of the end point
    \param[in]  y1: the y position of the end point
    \param[in]  color: lcd display color
    \param[out] none
    \retval     none
*/
void gui_draw_line(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t color)
{
    /* - difference in x's
       - difference in y's
       - dx,dy * 2
       - amount in pixel space to move during drawing
       - amount in pixel space to move during drawing
       - the discriminant i.e. error i.e. decision variable
       - used for looping */
    int dx,dy,dx2,dy2,x_inc,y_inc,error,index;

    LCD_CS_CLR;

    lcd_set_xy(x0,y0);
    /* calculate x distance */
    dx = x1-x0;
    /* calculate y distance */
    dy = y1-y0;

    if(dx>=0){
        x_inc = 1;
    }else{
        x_inc = -1;
        dx    = -dx;
    } 

    if(dy>=0){
        y_inc = 1;
    }else{
        y_inc = -1;
        dy    = -dy; 
    } 

    dx2 = dx << 1;
    dy2 = dy << 1;

    if(dx > dy){
        /* initialize error */
        error = dy2 - dx;
        /* draw the line */
        for(index=0; index <= dx; index++){
            gui_draw_point(x0,y0,color);
            /* test if error has overflowed */
            if(error >= 0){
                error-=dx2;
                /* move to next line */
                y0+=y_inc;
             }
            /* adjust the error term */
            error+=dy2;
            /* move to the next pixel */
            x0+=x_inc;
         }
    }else{
        /* initialize error term */
        error = dx2 - dy;
        /* draw the linedraw the line*/
        for(index=0; index <= dy; index++){
            /* set the pixel */
            gui_draw_point(x0,y0,color);

            /* test if error overflowed */
            if(error >= 0){
                error-=dy2;
                /* move to next line */
                x0+=x_inc;
            }
            /* adjust the error term */
            error+=dx2;

            /* move to the next pixel */
            y0+=y_inc;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui box
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  bc: lcd display color
    \param[out] none
    \retval     none
*/
void gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc)
{
    LCD_CS_CLR;

    /* gui draw line*/
    gui_draw_line(x,y, x+w, y, 0xEF7D);
    gui_draw_line(x+w-1, y+1, x+w-1, y+1+h, 0x2965);
    gui_draw_line(x, y+h, x+w, y+h, 0x2965);
    gui_draw_line(x, y, x, y+h, 0xEF7D);
    gui_draw_line(x+1, y+1, x+1+w-2, y+1+h-2, bc);

    LCD_CS_SET;
}

/*!
    \brief      gui box2
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  mode: display color combination mode 
    \param[out] none
    \retval     none
*/
void gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode)
{
    LCD_CS_CLR;
    /* gui box2 display mode0 */
    if(mode == 0){
        gui_draw_line(x, y, x+w, y, 0xEF7D);
        gui_draw_line(x+w-1, y+1, x+w-1, y+1+h, 0x2965);
        gui_draw_line(x, y+h, x+w, y+h, 0x2965);
        gui_draw_line(x, y, x, y+h, 0xEF7D);
    }
    /* gui box2 display mode1 */
    if(mode == 1){
        gui_draw_line(x, y, x+w, y, 0x2965);
        gui_draw_line(x+w-1, y+1, x+w-1, y+1+h, 0xEF7D);
        gui_draw_line(x, y+h, x+w, y+h, 0xEF7D);
        gui_draw_line(x, y, x, y+h, 0x2965);
    }
    /* gui box2 display mode2 */
    if(mode == 2){
        gui_draw_line(x, y, x+w, y, 0xffff);
        gui_draw_line(x+w-1, y+1, x+w-1, y+1+h, 0xffff);
        gui_draw_line(x, y+h, x+w, y+h, 0xffff);
        gui_draw_line(x, y, x, y+h, 0xffff);
    }
    LCD_CS_SET;
}

/*!
    \brief      gui rect
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[in]  fc: lcd display color
    \param[out] none
    \retval     none
*/
void gui_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t fc)
{
    int ix,iy;
    LCD_CS_CLR;
    for(ix = x1; ix < x2; ix++){
        for(iy = y1; iy < y2; iy++)
            /* set the pixel */
            gui_draw_point(ix, iy,fc);
    }

    LCD_CS_SET;
}

/*!
    \brief      display button down
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[out] none
    \retval     none
*/
void display_button_down(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_CS_CLR;

    /* gui draw line with gray color*/
    gui_draw_line(x1, y1, x2,y1, GRAY2);
    gui_draw_line(x1+1, y1+1, x2, y1+1, GRAY1);
    gui_draw_line(x1, y1, x1, y2, GRAY2);
    gui_draw_line(x1+1, y1+1, x1+1, y2, GRAY1);

    /* gui draw line with white color*/
    gui_draw_line(x1, y2, x2, y2, WHITE);
    gui_draw_line(x2, y1, x2, y2, WHITE);
    LCD_CS_SET;
}

/*!
    \brief      display button up
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[out] none
    \retval     none
*/
void display_button_up(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_CS_CLR;

    /* gui draw line with white color*/
    gui_draw_line(x1, y1, x2, y1, WHITE);
    gui_draw_line(x1, y1, x1, y2, WHITE);

    /* gui draw line with gray color*/
    gui_draw_line(x1+1, y2-1, x2, y2-1, GRAY1);
    gui_draw_line(x1, y2, x2,y2, GRAY2);
    gui_draw_line(x2-1, y1+1, x2-1, y2, GRAY1);
    gui_draw_line(x2, y1, x2, y2, GRAY2);
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to gbk16
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: lcd display color
    \param[in]  bc: display color of font
    \param[in]  *s: display char
    \param[out] none
    \retval     none
*/
void gui_draw_font_gbk16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s)
{
    unsigned char i,j;
    unsigned short k,x0;
    x0 = x;

    LCD_CS_CLR;
    while(*s){
        /* ASCII character table from 32 to 128 */
        if(((uint8_t)(*s)) < 128){
            k = *s;
            if(k == 13){
                x = x0;
                y += 16;
            }else{
                if(k > 32)
                    k -= 32;
                else 
                    k = 0;
                for(i=0;i<16;i++)
                    for(j=0; j<8; j++){
                        if(asc16[k*16+i] & (0x80 >> j))
                            gui_draw_point(x+j, y+i ,fc);
                        else{
                            if(fc != bc)
                                gui_draw_point(x+j, y+i, bc);
                        }
                    }
                x += 8;
            }
            s++;
        }else{
            for(k=0; k<hz16_num; k++){
                if((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s+1))){ 
                    for(i=0;i<16;i++){
                        for(j=0; j<8; j++){
                            if(hz16[k].Msk[i*2] & (0x80 >> j))
                                gui_draw_point(x+j, y+i, fc);
                            else{
                                if(fc != bc)
                                    gui_draw_point(x+j, y+i, bc);
                            }
                        }
                        for(j=0; j<8; j++){
                            if(hz16[k].Msk[i*2+1] & (0x80 >> j))
                                gui_draw_point(x+j+8, y+i, fc);
                            else{
                                if(fc != bc)
                                    gui_draw_point(x+j+8, y+i, bc);
                            }
                        }
                    }
                }
            }
            s += 2;
            x += 16;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to gbk24
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: lcd display color
    \param[in]  bc: display color of font
    \param[in]  *s: display char
    \param[out] none
    \retval     none
*/
void gui_draw_font_gbk24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s)
{
    unsigned char i,j;
    unsigned short k;

    LCD_CS_CLR;
    while(*s){
        if(((uint8_t)(*s)) < 0x80){
            k = *s;
            if(k > 32)
                k -= 32;
            else
                k = 0;

            for(i=0; i<16; i++)
                for(j=0; j<8; j++){
                    if(asc16[k*16+i] & (0x80 >> j))
                        gui_draw_point(x+j, y+i, fc);
                    else{
                        if(fc != bc)
                            gui_draw_point(x+j, y+i, bc);
                    }
                }
            s++;
            x += 8;
        }else{
            for(k=0; k<hz24_num; k++){
                if((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s+1))){ 
                    for(i=0; i<24; i++){
                        for(j=0; j<8; j++){
                            if(hz24[k].Msk[i*3] & (0x80 >> j))
                                gui_draw_point(x+j, y+i, fc);
                            else{
                                if(fc != bc)
                                    gui_draw_point(x+j, y+i, bc);
                            }
                        }
                        for(j=0; j<8; j++){
                            if(hz24[k].Msk[i*3+1] & (0x80 >> j))
                                gui_draw_point(x+j+8, y+i, fc);
                            else{
                                if(fc != bc)
                                gui_draw_point(x+j+8, y+i, bc);
                            }
                        }
                        for(j=0; j<8; j++){
                            if(hz24[k].Msk[i*3+2] & (0x80 >> j))
                                gui_draw_point(x+j+16, y+i, fc);
                            else{
                                if(fc != bc)
                                    gui_draw_point(x+j+16, y+i, bc);
                            }
                        }
                    }
                }
            }
        s += 2;
        x += 24;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to num32
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: lcd display color
    \param[in]  bc: display color of font
    \param[in]  num: display num
    \param[out] none
    \retval     none
*/
void gui_draw_font_num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num)
{
    unsigned char i,j,k,c;

    LCD_CS_CLR;
    for(i=0; i<32; i++){
        for(j=0; j<4; j++){
            c = *(sz32 + num*32*4+i*4+j);
            for(k=0; k<8; k++){
                if(c & (0x80 >> k))
                    gui_draw_point(x+j*8+k, y+i, fc);
                else{
                    if(fc != bc)
                        gui_draw_point(x+j*8+k, y+i, bc);
                }
            }
        }
    }
    LCD_CS_SET;
}
