/*!
    \file  lcd_log.h
    \brief the header file of LCD log
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "stdint.h"
#include "lcd_font.h"

/* initialize the LCD log module */
void lcd_log_init (void);
/* de-initialize the LCD log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen */
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x);
/* redirect the printf to the lcd */
void lcd_log_print (uint8_t *p_str, uint16_t offset, uint16_t char_color);
/* clear the text zone */
void lcd_log_text_zone_clear (uint16_t start_x,
                              uint16_t start_y,
                              uint16_t width,
                              uint16_t height);
