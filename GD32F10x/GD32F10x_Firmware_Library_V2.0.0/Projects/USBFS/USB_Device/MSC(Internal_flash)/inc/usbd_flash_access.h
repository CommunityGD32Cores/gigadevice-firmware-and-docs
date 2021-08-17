/*!
    \file  flash_access.h 
    \brief the header file of flash_access.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef FLASH_ACCESS_H
#define FLASH_ACCESS_H

#include "gd32f10x.h"

#define ISFLASH_BLOCK_SIZE         2048
#define ISFLASH_BLOCK_NUM          32

/* function declarations */
/* initialize the nand flash */
uint32_t  flash_init (void);
/* read data from multiple blocks of nand flash */
uint32_t  flash_multi_blocks_read (uint8_t* pBuf, uint32_t read_addr, uint16_t block_size, uint32_t block_num);
/* write data to multiple blocks of flash */
uint32_t  flash_multi_blocks_write (uint8_t* pBuf, uint32_t write_addr, uint16_t block_size, uint32_t block_num);

#endif /* FLASH_ACCESS_H */
