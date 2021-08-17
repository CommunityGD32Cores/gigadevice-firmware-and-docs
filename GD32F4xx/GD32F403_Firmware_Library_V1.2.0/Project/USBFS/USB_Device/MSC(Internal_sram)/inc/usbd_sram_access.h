/*!
    \file  usbd_sram_access.h
    \brief the header file of sram_access.c
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBD_SRAM_ACCESS_H
#define USBD_SRAM_ACCESS_H

#include "GD32F403.h"

#define ISRAM_BLOCK_SIZE         512
#define ISRAM_BLOCK_NUM          160

/* function declarations */
/* read data from multiple blocks of internal SRAM */
uint32_t  sram_multi_blocks_read (uint8_t *pbuf,
                                  uint32_t read_addr,
                                  uint16_t block_size,
                                  uint32_t block_num);
/* write data from multiple blocks of internal SRAM */
uint32_t  sram_multi_blocks_write (uint8_t *pbuf,
                                   uint32_t write_addr,
                                   uint16_t block_size,
                                   uint32_t block_num);

#endif /* USBD_SRAM_ACCESS_H */

