/*!
    \file  lcd_font.h
    \brief the header file of LCD font
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef LCD_FONTS_H
#define LCD_FONTS_H

#include <stdint.h>

typedef struct _tFont
{
    const uint16_t *table;
    uint16_t Width;
    uint16_t Height;
} sFONT;

extern sFONT Font16x24;
extern sFONT Font12x12;
extern sFONT Font8x16;
extern sFONT Font8x12;
extern sFONT Font8x8;

#define LINE(x) ((x) * (((sFONT *)lcd_font_get())->Height))

#endif /* LCD_FONTS_H */

