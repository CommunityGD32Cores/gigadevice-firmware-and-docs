/*!
    \file  gd32f450z_lcd_eval.h
    \brief definitions for GD32F450Z_EVAL's LCD

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#ifndef GD32F450Z_LCD_EVAL_H
#define GD32F450Z_LCD_EVAL_H

#include "gd32f450z_eval.h"
   
#define LCD_CS_PIN                   GPIO_PIN_11
#define LCD_CS_GPIO_PORT             GPIOD
#define LCD_CS_GPIO_CLK              RCU_GPIOD 

#define LCD_RS_PIN                   GPIO_PIN_3
#define LCD_RS_GPIO_PORT             GPIOE
#define LCD_RS_GPIO_CLK              RCU_GPIOE

#define LCD_SPI_SCK_PIN              GPIO_PIN_13
#define LCD_SPI_SCK_GPIO_PORT        GPIOG
#define LCD_SPI_SCK_GPIO_CLK         RCU_GPIOG

#define LCD_SPI_MOSI_PIN             GPIO_PIN_14
#define LCD_SPI_MOSI_GPIO_PORT       GPIOG
#define LCD_SPI_MOSI_GPIO_CLK        RCU_GPIOG 

#define LCD_SPI                      SPI5
#define LCD_SPI_CLK                  RCU_SPI5

/* enable the LCD */
void lcd_enable(void);
/* disable the LCD */
void lcd_disable(void);
/* configure the LCD control line */
void lcd_ctrl_line_config(void);
/* set the LCD control line */
void lcd_ctrl_line_set(uint32_t gpiox, uint16_t gpiopin);
/* reset the LCD control line */
void lcd_ctrl_line_reset(uint32_t gpiox, uint16_t gpiopin);
/* configure the LCD SPI and it's GPIOs */
void lcd_spi_config(void);
/* write command to select LCD register */
void lcd_command_write(uint8_t lcd_register);
/* write data to select LCD register */
void lcd_data_write(uint8_t value);
/* configure the LCD based on the power on sequence(for V1.1 LCD or earlier) */
void lcd_power_on(void);
/* configure the LCD based on the power on sequence 1(for V1.2 LCD) */
void lcd_power_on1(void);

#endif /* GD32F450Z_LCD_EVAL_H */ 
