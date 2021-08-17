/*!
    \file  flash_access.c
    \brief flash access functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "usbd_conf.h"
#include "usbd_flash_access.h"

/* pages 0 and 1 base and end addresses */
#define  NAND_FLASH_BASE_ADDRESS 0x8008000
#define  PAGE_SIZE 0x800


/*!
    \brief      initialize the nand flash
    \param[in]  none
    \param[out] none
    \retval     status
  */
uint32_t  flash_init ()
{
    fmc_unlock();

    return 0;
}

/*!
    \brief      read data from multiple blocks of nand flash
    \param[in]  pBuf: pointer to user buffer
    \param[in]  read_addr: address to be read
    \param[in]  block_size: size of block
    \param[in]  block_num: number of block
    \param[out] none
    \retval     status
*/
uint32_t  flash_multi_blocks_read (uint8_t *pBuf, uint32_t read_addr, uint16_t block_size, uint32_t block_num)
{
    uint32_t i;
    uint8_t *pSource = (uint8_t *)(read_addr + NAND_FLASH_BASE_ADDRESS);

    /* Data transfer */
    while (block_num--) {
        for (i = 0; i < block_size; i++) {
            *pBuf++ = *pSource++;
        }
    }

    return 0;
}

/*!
    \brief      write data to multiple blocks of flash
    \param[in]  pBuf: pointer to user buffer
    \param[in]  write_addr: address to be write
    \param[in]  block_size: block size
    \param[in]  block_num: number of block
    \param[out] none
    \retval     status
*/
uint32_t flash_multi_blocks_write (uint8_t *pBuf,
                           uint32_t write_addr,
                           uint16_t block_size,
                           uint32_t block_num)
{
    uint32_t i, page;
    uint32_t start_page = (write_addr / PAGE_SIZE) * PAGE_SIZE + NAND_FLASH_BASE_ADDRESS;
    uint32_t *ptrs = (uint32_t *)pBuf;
    uint16_t word_count = block_size / 4;

    page = block_num;

    for(; page > 0; page--){
        fmc_page_erase(start_page);

        i = 0;

        do{
            fmc_word_program(start_page, *ptrs++);
            start_page += 4;
        }while(++i < word_count);
    }

    return 0;
}
