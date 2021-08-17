/*!
    \file  main.c
    \brief LCD test demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"
#include "exmc_lcd.h"
#include "ili9320.h"
#include "picture.h"

uint8_t gd_website_string[]={"www.gigadevice.com"} ;
uint16_t device_code;
char_format_struct char_format;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i;

    /* configure the EXMC access mode */
    exmc_lcd_init();

    /* read the LCD Controller device code:(0x8989) */
    device_code = lcd_register_read(0x0000);

    /* initialize the LCD */
    lcd_init();

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
    
    while(1){
    }
}
