/*!
    \file  lcd_log.c
    \brief LCD log driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "lcd_log.h"
#include "string.h"
#include "gd32f10x_lcd_eval.h"

uint16_t LINE;

char_format_struct charform = {
    .font = CHAR_FONT_8_16,
    .direction = CHAR_DIRECTION_HORIZONTAL,
    .char_color = RED,
    .bk_color = BLACK
};

/*!
    \brief      initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(BLACK);
}

/*!
    \brief      de-initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  p_title: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x)
{
    uint16_t i = 0;
    uint16_t str_len = strlen((const char *)p_title);

    lcd_rectangle_fill(210, 0, 240, 320, BLUE);

    charform.bk_color = BLUE;

    for (i = 0; i < str_len; i++) {
        lcd_char_display (230, (start_x + 8 * i), p_title[i], charform);
    }

}

/*!
    \brief      display the application footer (status) on the LCD screen 
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x)
{
    uint16_t i = 0;
    uint16_t str_len = strlen((const char *)p_status);

    lcd_rectangle_fill(0, 0, 30, 320, BLUE);


    charform.bk_color = BLUE;

    for (i = 0; i < str_len; i++) {
        lcd_char_display (20, (start_x + 8 * i), p_status[i], charform);
    }

}

/*!
    \brief      clear the text zone 
    \param[in]  start_x: the start x position
    \param[in]  start_y: the start y position
    \param[in]  width: the width to clear text zone
    \param[in]  height: the heitht to clear text zone
    \param[out] none
    \retval     none
*/
void lcd_log_text_zone_clear(uint16_t start_x,
                             uint16_t start_y,
                             uint16_t width,
                             uint16_t height)
{
    lcd_rectangle_fill(start_x, start_y, width, height, BLACK);
}

/*!
    \brief      redirect the printf to the lcd 
    \param[in]  p_str: pointer to string to be displayed
    \param[in]  offset: the offset to set
    \param[in]  char_color: the clar color to set
    \param[out] none
    \retval     none
*/
void lcd_log_print (uint8_t *p_str, uint16_t offset, uint16_t char_color)
{
    uint16_t i;

#if defined(USE_HOST_MODE) && defined(USE_DEVICE_MODE)
    if(LINE <= 60)
    {
        LINE = 190;

        lcd_rectangle_fill(60, 0, 210, 320, BLACK);
    }
#else
    if(LINE <= 30)
    {
        LINE = 190;

        lcd_rectangle_fill(30, 0, 210, 320, BLACK);
    }
#endif

    charform.bk_color = BLACK;
    charform.char_color = char_color;

    for (i = 0; i < offset; i++) {
        lcd_char_display(LINE, (10 + 8 * i), *p_str++, charform);
    }

    LINE -= 20;
}
