/*!
    \file    flash_msd.c
    \brief   flash access functions

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "flash_msd.h"

/*!
    \brief      initialize flash
    \param[in]  none
    \param[out] none
    \retval     status
  */
uint32_t flash_init ( )
{
    fmc_unlock();
    return 0U;
}

/*!
    \brief      read data from multiple blocks of flash
    \param[in]  pBuf: pointer to user buffer
    \param[in]  read_addr: address to be read
    \param[in]  blk_size: size of block
    \param[in]  blk_num: number of block
    \param[out] none
    \retval     status
*/
uint32_t flash_read_multi_blocks (uint8_t *buf, 
                                  uint32_t read_addr, 
                                  uint16_t blk_size,
                                  uint32_t blk_num)
{
    uint32_t i;
    uint8_t *source = (uint8_t *)(read_addr + FLASH_BASE_ADDR);

    /* Data transfer */
    while (blk_num--)
    {
        for (i = 0U; i < blk_size; i++)
        {
            *buf++ = *source++;
        }
    }

    return 0U;
}

/*!
    \brief      write data to multiple blocks of flash
    \param[in]  pBuf: pointer to user buffer
    \param[in]  write_addr: address to be write
    \param[in]  blk_size: block size
    \param[in]  blk_num: number of block
    \param[out] none
    \retval     status
*/
uint32_t flash_write_multi_blocks (uint8_t *buf,
                                   uint32_t write_addr,
                                   uint16_t blk_size,
                                   uint32_t blk_num)
{
    return 0U;
}
