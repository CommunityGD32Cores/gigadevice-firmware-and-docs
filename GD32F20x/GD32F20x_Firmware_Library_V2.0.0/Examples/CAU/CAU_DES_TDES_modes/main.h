/*!
    \file  main.h
    \brief the header file of main 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "stdio.h"

#ifndef __MAIN_H
#define __MAIN_H

#define DATA_SIZE                    8
#define ECB                          1
#define CBC                          2
#define DES                         ' '
#define TDES                        'T'

/* key size 64 bytes */
uint8_t des_key[8] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6}; 

/* key : 192 bits  */
uint8_t tdes_key[24] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF, 
                        0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10, 
                        0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67};

uint8_t plaintext[DATA_SIZE] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};

/* initialization vector */
uint8_t vectors[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};

#endif /* __MAIN_H */
