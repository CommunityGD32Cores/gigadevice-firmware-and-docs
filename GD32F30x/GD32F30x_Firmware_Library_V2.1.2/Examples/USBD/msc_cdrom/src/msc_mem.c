/*!
    \file    msc_mem.c
    \brief   msc memories operation functions

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

#include "usbd_msc_mem.h"
#include "internal_flash_if.h"

uint8_t mem_inquiry_data[] = 
{
    /* LUN 0 */
    0x05,                        /* 0x05 for CD-ROM; 0x00 for usb mass storage */
    0x80,                        /* RMB BYTE is set by inquiry data */
    0x02,                        /* ANSI SCSI 2 */
    0x00,                        /* Data format = 0 */
    0x1F,                        /* Additional length */
    0x00,
    0x00,
    0x00,
    /* vendor information = GD32 */
    'G', 'D', '3', '2', 0x20, 0x20, 0x20, 0x20 ,
    /* product identification = CDROM DEMO */
    'C', 'D', 'R', 'O', 'M', 0x20,'D', 'E', 'M','0',0x20,0x20,0x20, 0x20, 0x20, 0x20, 
    /* Product revision = 1.0 */
    0x31, 0x2E, 0x30, 0x20
};

/* USB CDROM TOC(Table Of Content) Data */
uint8_t usbcdrom_read_toc_data[] = {
    0x00,                        
    0x12,                        /* number of bytes below */
    0x01,                        /* first track */
    0x01,                        /* last track */
    0x00,                        /* reserved */
    0x14,                        /* (adr, control) */
    0x01,                        /* (track being described) */ 
    0x00,                        /* (reserved) */
    00, 00, 00, 00,              /* (start logical block address 0) */
    0x00,                        /* (reserved) */
    0x14,                        /* (adr, control) */
    0xAA,                        /* (track being described (leadout)) */
    0x00,                        /* (reserved) */
    00, 00, 0xA2, 0x8A           /* (start logical block address 41610) */
};

/* local function prototypes ('static') */
static int8_t mem_init      (uint8_t lun);
static int8_t mem_ready     (uint8_t lun);
static int8_t mem_protected (uint8_t lun);
static int8_t mem_maxlun    (void);
static int8_t mem_read      (uint8_t lun, uint8_t *buf, uint32_t blkaddr, uint16_t blklen);
static int8_t mem_write     (uint8_t lun, uint8_t *buf, uint32_t blkaddr, uint16_t blklen);

usbd_mem_cb usbd_mem_opera =
{
    .mem_init         = mem_init,
    .mem_ready        = mem_ready,
    .mem_protected    = mem_protected,
    .mem_read         = mem_read,
    .mem_write        = mem_write,
    .mem_maxlun       = mem_maxlun,

    .mem_toc_data     = usbcdrom_read_toc_data,
    .mem_inquiry_data = {(uint8_t *)mem_inquiry_data},
    .mem_block_size   = {BLOCK_SIZE},
    .mem_block_len    = {BLOCK_NUM}
};

usbd_mem_cb *usbd_mem_fops = &usbd_mem_opera;

/*!
    \brief      initialize the storage medium
    \param[in]  lun: logical unit number
    \param[out] none
    \retval     status
*/
static int8_t mem_init (uint8_t lun)
{
    flash_init();

    return 0;
}

/*!
    \brief      check whether the medium is ready
    \param[in]  lun: logical unit number
    \param[out] none
    \retval     status
*/
static int8_t mem_ready (uint8_t lun)
{
    return 0;
}

/*!
    \brief      check whether the medium is write-protected
    \param[in]  lun: logical unit number
    \param[out] none
    \retval     status
*/
static int8_t mem_protected (uint8_t lun)
{
    return 0;
}

/*!
    \brief      read data from the medium
    \param[in]  lun: logical unit number
    \param[in]  buf: pointer to the buffer to save data
    \param[in]  blkaddr: address of 1st block to be read
    \param[in]  blklen: number of blocks to be read
    \param[out] none
    \retval     status
*/
static int8_t mem_read (uint8_t lun, uint8_t *buf, uint32_t blkaddr, uint16_t blklen)
{
    flash_read_multi_blocks(buf, blkaddr, BLOCK_SIZE, (uint32_t)blklen);

    return 0;
}

/*!
    \brief      write data to the medium
    \param[in]  lun: logical unit number
    \param[in]  buf: pointer to the buffer to save data
    \param[in]  blkaddr: address of 1st block to be read
    \param[in]  blklen: number of blocks to be read
    \param[out] none
    \retval     status
*/
static int8_t mem_write (uint8_t lun, uint8_t *buf, uint32_t blkaddr, uint16_t blklen)
{
    flash_write_multi_blocks (buf, blkaddr, BLOCK_SIZE, (uint32_t)blklen);

    return 0;
}

/*!
    \brief      get number of supported logical unit
    \param[in]  none
    \param[out] none
    \retval     status
*/
static int8_t mem_maxlun (void)
{
    return ((int8_t)MEM_LUN_NUM - 1);
}
