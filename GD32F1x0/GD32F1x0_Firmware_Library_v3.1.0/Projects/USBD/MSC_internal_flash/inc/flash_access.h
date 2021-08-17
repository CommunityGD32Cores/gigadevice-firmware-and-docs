/*!
    \file  flash_access.h 
    \brief the header file of flash_access.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#ifndef FLASH_ACCESS_H
#define FLASH_ACCESS_H

#include "gd32f1x0.h"

/* function declarations */
/* initialize the nand flash */
uint32_t  flash_init (void);
/* read data from multiple blocks of nand flash */
uint32_t  flash_read_multi_blocks (uint8_t* pBuf, uint32_t read_addr, uint16_t block_size, uint32_t block_num);
/* write data to multiple blocks of flash */
uint32_t  flash_write_multi_blocks (uint8_t* pBuf, uint32_t write_addr, uint16_t block_size, uint32_t block_num);

#endif /* FLASH_ACCESS_H */
