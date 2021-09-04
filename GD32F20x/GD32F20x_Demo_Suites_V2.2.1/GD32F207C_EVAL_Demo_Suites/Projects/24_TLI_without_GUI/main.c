/*!
    \file  main.c
    \brief TLI display demo without GUI

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2018-10-31, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
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

#include "gd32f20x.h"
#include "systick.h"
#include <stdio.h>
#include "picture.h"
#include "gd32f207c_eval.h"
#include "gd32f207c_lcd_eval.h"

void lcd_picture_display(void);
static void lcd_config(void);

extern tli_layer_parameter_struct         tli_layer_initstruct;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure Systick and LCD */
    systick_config();
    lcd_config();
    
    tli_layer_enable(LAYER0);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable(); 

    while(1){
        /* display different pictures */
        lcd_picture_display();
    }
}

/*!
    \brief      configure LCD
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_config(void)
{
    /* configure LCD control lines and SPI */
    lcd_control_lines_config();
    lcd_chip_select(DISABLE);  
    lcd_chip_select(ENABLE);
    lcd_spi_config();
    
    /* power on the LCD */
    lcd_power_on1();
    /* configure the LCD and layers */
    lcd_init();
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    lcd_layer_init(LCD_LAYER_BACKGROUND, 320, 162);
}

/*!
    \brief      display different pictures on LCD
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_picture_display(void)
{
    /* display image 0 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_0;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 1 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_1;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 2 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_2;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 3 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_3;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 4 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_4;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 5 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_5;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 6 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_6;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
    
    /* display image 7 */
    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_7;
    lcd_layer_init(LCD_LAYER_FOREGROUND, 320, 60);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    
    delay_1ms(0x6F);
}
