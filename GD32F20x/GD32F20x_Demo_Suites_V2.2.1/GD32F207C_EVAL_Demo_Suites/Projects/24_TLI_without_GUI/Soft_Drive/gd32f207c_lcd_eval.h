/*!
    \file  gd32f207c_lcd_eval.h
    \brief the header file of the LCD eval

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2018-10-31, V2.1.0, demo for GD32F20x
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

#ifndef GD32F207C_LCD_EVAL_H
#define GD32F207C_LCD_EVAL_H

#include "stdint.h"
#include "gd32f20x.h"
#include "picture.h"

#define LCD_CS_PIN                GPIO_PIN_7
#define LCD_CS_GPIO_PORT          GPIOD
#define LCD_CS_GPIO_CLK           RCU_GPIOD

#define LCD_RS_PIN                GPIO_PIN_2
#define LCD_RS_GPIO_PORT          GPIOE
#define LCD_RS_GPIO_CLK           RCU_GPIOE

#define LCD_SPI_SCK_PIN           GPIO_PIN_5
#define LCD_SPI_SCK_GPIO_PORT     GPIOA
#define LCD_SPI_SCK_GPIO_CLK      RCU_GPIOA

#define LCD_SPI_MISO_PIN          GPIO_PIN_6
#define LCD_SPI_MISO_GPIO_PORT    GPIOA
#define LCD_SPI_MISO_GPIO_CLK     RCU_GPIOA

#define LCD_SPI_MOSI_PIN          GPIO_PIN_7
#define LCD_SPI_MOSI_GPIO_PORT    GPIOA
#define LCD_SPI_MOSI_GPIO_CLK     RCU_GPIOA

#define LCD_SPI                   SPI0
#define LCD_SPI_CLK               RCU_SPI0

#define LCD_PIXEL_WIDTH           ((uint16_t)320)
#define LCD_PIXEL_HEIGHT          ((uint16_t)480)

#define LCD_LAYER_BACKGROUND      0x0000
#define LCD_LAYER_FOREGROUND      0x0001

/* initialize the LCD clock, GPIO and TLI */
void lcd_init(void);
/* initialize TLI layer0 or layer1 */
void lcd_layer_init(uint32_t layer, uint32_t width, uint32_t height);

/* configure the LCD control lines */
void lcd_control_lines_config(void);
/* set the LCD control lines */
void lcd_control_lines_set(uint32_t gpiox, uint32_t gpio_pin);
/* reset the LCD control lines */
void lcd_control_lines_reset(uint32_t gpiox, uint32_t gpio_pin);
/* configure the LCD SPI */
void lcd_spi_config(void);

/* configure LCD chip select (CS) pin */
void lcd_chip_select(ControlStatus staus);
/* write command to the LCD register */
void lcd_command_write(uint8_t lcd_reg);
/* write data to the LCD register */
void lcd_data_write(uint8_t data);
/* configure the LCD controller */
void lcd_power_on(void);
/* configure the LCD controller on sequence 1 */
void lcd_power_on1(void);

#endif /* GD32F207C_LCD_EVAL_H */
