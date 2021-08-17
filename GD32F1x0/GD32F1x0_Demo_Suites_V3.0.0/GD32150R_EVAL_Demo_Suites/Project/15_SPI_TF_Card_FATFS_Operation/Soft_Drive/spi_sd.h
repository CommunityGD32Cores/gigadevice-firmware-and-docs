/*!
    \file  spi_sd.h
    \brief the header file of SPI sd card driver

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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


#ifndef SPI_SD_H
#define SPI_SD_H

#include <stdint.h>

typedef enum
{
    /* SD card reponses error */
    SD_RESPONSE_NO_ERROR                   = (0x00),
    SD_IN_IDLE_STATE                       = (0x01),
    SD_COM_CRC_ERROR                       = (0x08),
    SD_RESPONSE_FAILURE                    = (0xFF),
}sd_error_enum;

/* the SD status: read data response xxx0<status>1 */
#define SD_DATA_OK                         0x05           /* status 010: data accecpted */
#define SD_DATA_CRC_ERROR                  0x0B           /* status 101: data rejected due to a crc error */
#define SD_DATA_WRITE_ERROR                0x0D           /* status 110: data rejected due to a write error */

/* dummy byte */
#define SD_DUMMY_BYTE   0xFF

/* start data tokens */
#define SD_START_DATA_SINGLE_BLOCK_READ    0xFE           /* data token start byte, start single block read */
#define SD_START_DATA_SINGLE_BLOCK_WRITE   0xFE           /* data token start byte, start single block write */

/* commands: cmdx = x | 0x40 */
#define SD_CMD_GO_IDLE_STATE               0              /* CMD0 = 0x40 */
#define SD_CMD_SEND_IF_COND                8              /* CMD8 = 0x48 */
#define SD_CMD_SEND_CSD                    9              /* CMD9 = 0x49 */
#define SD_CMD_SEND_CID                    10             /* CMD10 = 0x4A */
#define SD_CMD_SEND_STATUS                 13             /* CMD13 = 0x4D */
#define SD_CMD_READ_SINGLE_BLOCK           17             /* CMD17 = 0x51 */
#define SD_CMD_WRITE_SINGLE_BLOCK          24             /* CMD24 = 0x58 */
#define SD_CMD_SEND_OP_COND                41             /* ACMD41 = 0x69 */
#define SD_CMD_APP_CMD                     55             /* CMD55 = 0x77 */
#define SD_CMD_READ_OCR                    58             /* CMD58 = 0x7A */

/* chipselect definitions */
#define SD_CS_LOW()    gpio_bit_reset(GPIOF,GPIO_PIN_4);  /* select SD card: chipselect pin low */  
#define SD_CS_HIGH()   gpio_bit_set(GPIOF,GPIO_PIN_4);    /* deselect SD card: chipselect pin high */ 

/* SPI SD card function */
/* SD card initialization */
sd_error_enum sd_card_init(void);  
/* get SD card capacity */
sd_error_enum sd_card_capacity_get(void);
/* read a block from SD card */
sd_error_enum sd_block_read(uint8_t* pbuffer, uint32_t readaddr, uint16_t blocksize);
/* read multiple block from SD card */
sd_error_enum sd_multiblocks_read(uint8_t* pbuffer, uint32_t readaddr, uint16_t blocksize, uint32_t numberofblocks);
/* write a block to SD card */
sd_error_enum sd_block_write(uint8_t* pbuffer, uint32_t writeaddr, uint16_t blocksize);
/* write multiple block to SD card */
sd_error_enum sd_multiblocks_write(uint8_t* pbuffer, uint32_t writeaddr, uint16_t blocksize, uint32_t numberofblocks);
/* get SD card data response */
uint8_t sd_data_response_get(void);
/* read the csd register */
sd_error_enum sd_csd_register_get(uint8_t* sd_csd_table);
/* read the cid register */
sd_error_enum sd_cid_register_get(uint8_t* sd_cid_table);
/* send command to SD card */
uint8_t sd_cmd_send(uint8_t cmdx, uint32_t argument, uint8_t crc);
/* get the SD response */
sd_error_enum sd_response_get(uint8_t response);
/* put SD card in idle state */
sd_error_enum sd_to_idle_state(void);
/* get the SD status */
uint16_t sd_status_get(void);
/* use SPI bus write a byte to SD card */
uint8_t spi_write(uint8_t byte);
/* use SPI bus read a byte from SD card */
uint8_t spi_read(void);

#endif /* SPI_SD_H */
