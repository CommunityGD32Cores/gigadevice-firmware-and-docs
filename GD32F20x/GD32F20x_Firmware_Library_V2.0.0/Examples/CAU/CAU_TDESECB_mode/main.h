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

#define BLOCKS_NBR            5
#define DATA_SIZE             ((BLOCKS_NBR*64)/32)
#define CAU_DI_REG_ADDR       ((uint32_t)0x50060008)
#define CAU_DO_REG_ADDR       ((uint32_t)0x5006000C)

/* 192 bit key */
uint32_t tdes_key[6]={
              0x40414243,0x44454647,     /* KEY1 */
              0x48494A4B,0x4C4D4E4F,     /* KEY2 */
              0x50515253,0x54555657};    /* KEY3 */

uint32_t plaintext[DATA_SIZE]={
              0x20212223,0x24252627,     /* block 0 */
              0x28292a2b,0x2c2d2e2f,     /* block 1 */
              0xFFEEDDCC,0xBBAA9988,     /* block 2 */
              0x77665544,0x33221100,     /* block 3 */
              0x10000000,0x20000000 };   /* block 4 */

#endif /* __MAIN_H */
