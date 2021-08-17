/*!
    \file  rc5_encode.h
    \brief the header file of rc5 infrared encoder
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/


#ifndef RC5_ENCODE_H
#define RC5_ENCODE_H

#include "gd32f3x0.h"

/* definition of the rc5 control bit value */
typedef enum
{
    RC5_CTRL_RESET = ((uint16_t)0),
    RC5_CTRL_SET   = ((uint16_t)0x0800)
}rc5_ctrl_enum;

void menu_rc5_encode_func(void);
void rc5_encode_init(void);
void rc5_encode_send_frame(uint8_t rc5_address, uint8_t rc5_instruction, rc5_ctrl_enum rc5_ctrl);
void rc5_encode_signal_generate(uint32_t rc5_manchester_frame_format);

#endif  /*RC5_ENCODE_H */
