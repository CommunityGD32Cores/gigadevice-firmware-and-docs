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

/* plain text size in words, it must be a multiple of 4(128 bits = 16 bytes = 4 words) for AES algorithm */   
#define DATA_SIZE             ((uint32_t)8)

#define CAU_DI_REG_ADDR       ((uint32_t)0x50060008)
#define CAU_DO_REG_ADDR       ((uint32_t)0x5006000C)

uint32_t key_128[4]={0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c};
uint32_t plaintext[DATA_SIZE]={
              0x6bc1bee2, 0x2e409f96, 0xe93d7e11, 0x7393172a,  /* block 1 */
              0xae2d8a57, 0x1e03ac9c, 0x9eb76fac, 0x45af8e51}; /* block 2 */

#endif /* __MAIN_H */
