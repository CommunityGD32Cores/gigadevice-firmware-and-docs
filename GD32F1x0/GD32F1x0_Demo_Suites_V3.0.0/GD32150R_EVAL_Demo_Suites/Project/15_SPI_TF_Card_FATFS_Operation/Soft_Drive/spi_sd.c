/*!
    \file  spi_sd.c
    \brief description of SPI sd card driver

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

#include "gd32f1x0.h"
#include "spi_sd.h"
#include "stdio.h"
#include "systick.h"

uint8_t sd_csd_table[16];     /* sd card csd register table */
uint8_t sd_cid_table[16];     /* sd card cid register table */
uint32_t cardcapacity;        /* sd card capacity */
static void spi_config(void);

/*!
    \brief      sd card initialization 
    \param[in]  none
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_card_init(void)
{
    uint32_t i = 0;
    uint8_t result=0,count=100;
    /* configure SPI */
    spi_config();

    SD_CS_HIGH();
    /* wait for 80 clocks cycles */
    for(i = 0; i < 100; i++)
        /* send dummy byte 0xFF */
        spi_write(SD_DUMMY_BYTE);
    /* (1) CMD0 put SD card to idle state */
    SD_CS_LOW();
    do{
        result = sd_cmd_send(SD_CMD_GO_IDLE_STATE, 0, 0x95);
    }while((result != 0x01) && count--);
    /* wait for in idle state response (r1 format) equal to 0x01 */
    if(result != 0x01)
        return SD_RESPONSE_FAILURE;
    SD_CS_HIGH();
    spi_write(SD_DUMMY_BYTE); 

    /* (2) CMD8 2.7-3.6V support */
    SD_CS_LOW();
    result = sd_cmd_send(SD_CMD_SEND_IF_COND, 0x000001aa, 0x87);
    /* wait for in idle state response (r1 format) equal to 0x01 */
    if(result != 0x01)
        return SD_RESPONSE_FAILURE;
    /* read r7 response */
    spi_read();
    spi_read();
    spi_read();
    spi_read();
    SD_CS_HIGH();
    spi_write(SD_DUMMY_BYTE);

    /* (3) ACMD41:HCS(High Capacity Support) in the argument of ACMD41 */
    do{
        SD_CS_LOW();
        result = sd_cmd_send(SD_CMD_APP_CMD,0,0x01);
        SD_CS_HIGH();
        spi_write(SD_DUMMY_BYTE);

        SD_CS_LOW();
        result = sd_cmd_send(SD_CMD_SEND_OP_COND, 0x40000000, 0x01);
        SD_CS_HIGH();
        spi_write(SD_DUMMY_BYTE);
        /* wait for no error response (r1 format) equal to 0x00 */
    }while (result != 0x00);
    SD_CS_HIGH();
    spi_write(SD_DUMMY_BYTE);

    /* (4) CMD58:read OCR register */
      SD_CS_LOW();
    do{
        result = sd_cmd_send(SD_CMD_READ_OCR, 0x00, 0x01);  
    }while((result != 0x00) && count--);
    /* wait for no error response (r1 format) equal to 0x00 */
    if(result != 0x00)
        return SD_RESPONSE_FAILURE;
    /* read OCR register */
    spi_read();
    spi_read();
    spi_read();
    spi_read();
    SD_CS_HIGH();
    spi_write(SD_DUMMY_BYTE);

    /* (5) sd card init success and return status */
    return SD_RESPONSE_NO_ERROR;
}

/*!
    \brief      get sd card capacity 
    \param[in]  none
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_card_capacity_get(void)
{
    uint32_t device_size_temp=0;
    sd_error_enum status = SD_RESPONSE_FAILURE;
    /* get sd card cid and csd register value */
    status = sd_cid_register_get(sd_cid_table);  
    status = sd_csd_register_get(sd_csd_table);
    /* in csd register(csd version 2.0), device size define at bits[69:48],memory capacity = (device size+1) * 512K byte */
    /* bits[69:64] */
    device_size_temp = (sd_csd_table[7] & 0x3F) << 16;
    /* bits[63:56] */
    device_size_temp |= (sd_csd_table[8]) << 8;
    /* bits[55:48] */
    device_size_temp |= sd_csd_table[9];
    cardcapacity = (device_size_temp+1)*512;
    return status;
}


/*!
    \brief      read a block from sd card  
    \param[in]  pbuffer: pointer to the buffer that receives the data read from the card
    \param[in]  readaddr: SD card internal address to read from
    \param[in]  blocksize: sd card data block size
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_block_read(uint8_t* pbuffer, uint32_t readaddr, uint16_t blocksize)
{
    uint32_t i = 0;
    sd_error_enum status = SD_RESPONSE_FAILURE;
    uint8_t result = 0;
    SD_CS_LOW();
    /* send CMD17(SD_CMD_READ_SINGLE_BLOCK) to read a block */
    result = sd_cmd_send(SD_CMD_READ_SINGLE_BLOCK, readaddr, 0xFF);
    /* check if the SD card response the read block command: R1 response (0x00: no errors) */
    if(0 == result){
        /* now look for the data token to signify the start of the data */
        if(!sd_response_get(SD_START_DATA_SINGLE_BLOCK_READ)){
            /* read the sd block data */
            for(i = 0; i < blocksize; i++)
                /* save the received data */
                *pbuffer++ = spi_read();
            /* get crc bytes (not really needed by us, but required by sd) */
            spi_read();
            spi_read();
            /* set response value to success */
            status = SD_RESPONSE_NO_ERROR;
        }
    }
    SD_CS_HIGH();
    /* send dummy byte: 8 clock delay */
    spi_write(SD_DUMMY_BYTE);

    return status;
}

/*!
    \brief      read multiple block from sd card  
    \param[in]  pbuffer: pointer to the buffer that receives the data read from the card
    \param[in]  readaddr: SD card internal address to read from
    \param[in]  blocksize: sd card data block size
    \param[in]  numberofblocks: number of blocks
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_multiblocks_read(uint8_t* pbuffer, uint32_t readaddr, uint16_t blocksize, uint32_t numberofblocks)
{
    uint32_t i = 0, offset = 0;
    sd_error_enum status = SD_RESPONSE_FAILURE;
    uint8_t result = 0;

    SD_CS_LOW();
    /* data transfer */
    while(numberofblocks--){
        /* send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
        result = sd_cmd_send(SD_CMD_READ_SINGLE_BLOCK, readaddr + offset, 0xFF);
        /* check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
        if(0x00 != result)
            return SD_RESPONSE_FAILURE;
        /* now look for the data token to signify the start of the data */
        if(!sd_response_get(SD_START_DATA_SINGLE_BLOCK_READ)){
            /* read the SD block data : read numbytetoread data */
            for(i = 0;i < blocksize;i++)
                /* read the pointed data */
                *pbuffer++ = spi_write(SD_DUMMY_BYTE);

            /* set next read address*/
            offset += 1;
            /* get CRC bytes (not really needed by us, but required by SD) */
            spi_write(SD_DUMMY_BYTE);
            spi_write(SD_DUMMY_BYTE);
            /* set response value to success */
            status = SD_RESPONSE_NO_ERROR;
        }else{
            /* set response value to failure */
            status = SD_RESPONSE_FAILURE;
        }
    }
    /* SD chip select high */
    SD_CS_HIGH();
    /* send dummy byte: 8 clock delay */
    spi_write(SD_DUMMY_BYTE);
    /* return the status */
    return status;
}


/*!
    \brief      write a block to sd card  
    \param[in]  pbuffer: pointer to the buffer that send the data to the card
    \param[in]  writeaddr: SD card internal address write to
    \param[in]  blocksize: sd card data block size
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_block_write(uint8_t* pbuffer, uint32_t writeaddr, uint16_t blocksize)
{
    uint32_t i = 0;
    uint8_t result = 0;
    sd_error_enum status = SD_RESPONSE_FAILURE;
    SD_CS_LOW();
    /* send CMD24(SD_CMD_WRITE_SINGLE_BLOCK) to write single block */
    result = sd_cmd_send(SD_CMD_WRITE_SINGLE_BLOCK, writeaddr, 0xFF);
    /* check if the SD acknowledged the write block command: r1 response (0x00: no errors) */
    if(0 == result){
        /* send a dummy byte */
        spi_write(SD_DUMMY_BYTE);
        /* send the data token to signify the start of the data */
        spi_write(SD_START_DATA_SINGLE_BLOCK_WRITE);
        /* write the block data to SD card */
        for(i = 0;i < blocksize;i++)
            spi_write(*pbuffer++);

        /* put CRC bytes (not really needed by us, but required by SD) */
        spi_read();
        spi_read();
        /* read data response */
        if(SD_DATA_OK == sd_data_response_get()){
            status = SD_RESPONSE_NO_ERROR;
        }
    }
    SD_CS_HIGH();
    /* send dummy byte: 8 clock delay */
    spi_write(SD_DUMMY_BYTE);
    /* returns the reponse */
    return status;
}

/*!
    \brief      write multiple block to sd card  
    \param[in]  pbuffer: pointer to the buffer that send the data to the card
    \param[in]  writeaddr: SD card internal address write to
    \param[in]  blocksize: sd card data block size
    \param[in]  numberofblocks : number of blocks
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_multiblocks_write(uint8_t* pbuffer, uint32_t writeaddr, uint16_t blocksize, uint32_t numberofblocks)
{
    uint32_t i = 0, offset = 0;
    sd_error_enum rvalue = SD_RESPONSE_FAILURE;
    uint8_t result = 0;
    SD_CS_LOW();
    /* data transfer */
    while(numberofblocks--){
        /* send CMD24(SD_CMD_WRITE_SINGLE_BLOCK) to write blocks */
        result = sd_cmd_send(SD_CMD_WRITE_SINGLE_BLOCK, writeaddr + offset, 0xFF);
        /* check if the SD acknowledged the write block command: r1 response (0x00: no errors) */
        if(0x00 != result)
            return SD_RESPONSE_FAILURE;
        /* send dummy byte */
        spi_write(SD_DUMMY_BYTE);
        /* send the data token to signify the start of the data */
        spi_write(SD_START_DATA_SINGLE_BLOCK_WRITE);
        /* write the block data to SD card */
        for(i = 0;i < blocksize;i++)
            spi_write(*pbuffer++);
        /* set next write address */
        offset += 1;
        /* put CRC bytes (not really needed by us, but required by SD) */
        spi_write(SD_DUMMY_BYTE);
        spi_write(SD_DUMMY_BYTE);
        /* read data response */
        if(SD_DATA_OK == sd_data_response_get())
            /* set response value to success */
            rvalue = SD_RESPONSE_NO_ERROR;
        else
            /* set response value to failure */
            rvalue = SD_RESPONSE_FAILURE;
    }
    /* SD chip select high */
    SD_CS_HIGH();
    /* send dummy byte 8 clock delay */
    spi_write(SD_DUMMY_BYTE);
    /* return the reponse */
    return rvalue;
}

/*!
    \brief      get SD card data response
    \param[in]  none
    \param[out] none
    \retval     the SD status: read data response xxx0<status>1
                - status 010: data accecpted
                - status 101: data rejected due to a crc error
                - status 110: data rejected due to a write error.
*/
uint8_t sd_data_response_get(void)
{
    uint32_t i = 0;
    uint8_t response = 0;
    uint8_t rvalue = 0;
    while(i <= 64){
        /* read resonse */
        response = spi_read();
        /* mask unused bits */
        response &= 0x1F;
        switch (response){
        case SD_DATA_OK: rvalue = SD_DATA_OK;
             break;
        case SD_DATA_CRC_ERROR:
             return SD_DATA_CRC_ERROR;
        case SD_DATA_WRITE_ERROR:
             return SD_DATA_WRITE_ERROR;
        default:
             break;
        }
        /* exit loop in case of data ok */
        if(SD_DATA_OK == rvalue)
            break;
        /* increment loop counter */
        i++;
    }
    /* wait null data */
    while(0 == spi_read());
    /* return response */
    return response;
}

/*!
    \brief      read the csd register  
    \param[in]  sd_csd_table: sd card csd register table
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_csd_register_get(uint8_t* sd_csd_table)
{
    uint32_t i = 0;
    sd_error_enum status = SD_RESPONSE_FAILURE;

    SD_CS_LOW();
    /* send CMD9 (CSD register) */
    sd_cmd_send(SD_CMD_SEND_CSD, 0, 0xFF);
    /* wait for response in the r1 format (0x00 is no errors) */
    if(!sd_response_get(SD_START_DATA_SINGLE_BLOCK_READ)){
        for(i = 0;i < 16;i++){
            /* store csd register value on sd_csd_table */
            sd_csd_table[i] = spi_read();
        }
    }
    /* get crc bytes (not really needed by us, but required by sd) */
    spi_write(SD_DUMMY_BYTE);
    spi_write(SD_DUMMY_BYTE);
    /* set response value to success */
    status = SD_RESPONSE_NO_ERROR;
    SD_CS_HIGH();
    /* send dummy byte: 8 clock pulses of delay */
    spi_write(SD_DUMMY_BYTE);
    return status;
}

/*!
    \brief      read the cid register  
    \param[in]  sd_cid_table : sd card cid register table
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_cid_register_get(uint8_t* sd_cid_table)
{
    uint32_t i = 0;
    sd_error_enum status = SD_RESPONSE_FAILURE;
    SD_CS_LOW();
    /* send CMD10 (CID register) */
    sd_cmd_send(SD_CMD_SEND_CID,0,0xFF);
    if(!sd_response_get(SD_START_DATA_SINGLE_BLOCK_READ)){
        /* store CID register value on CID_Tab */
        for(i = 0;i < 16;i++){
            sd_cid_table[i] = spi_read();
        }
    }
    /* get crc bytes (not really needed by us, but required by sd) */
    spi_write(SD_DUMMY_BYTE);
    spi_write(SD_DUMMY_BYTE);
    /* set response value  */
    status = SD_RESPONSE_NO_ERROR;
    SD_CS_HIGH();
    /* send dummy byte: 8 clock pulses of delay */
    spi_write(SD_DUMMY_BYTE);
    /* return the reponse */
    return status;
}

/*!
    \brief      get the SD status
    \param[in]  none
    \param[out] none
    \retval     the SD status
*/
uint16_t sd_status_get(void)
{
    uint16_t status = 0;
    SD_CS_LOW();
    /* send CMD13(SD_SEND_STATUS) to get SD status */
    sd_cmd_send(SD_CMD_SEND_STATUS, 0, 0xFF);
    status = spi_read();
    status |= (uint16_t)(spi_read() << 8);
    SD_CS_HIGH();
    spi_write(SD_DUMMY_BYTE);
    return status;
}

/*!
    \brief      SPI parameter and GPIO configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_config(void)
{
    spi_parameter_struct spi_struct;
    /* configure GPIO */
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    /* configure SD CS pin */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    rcu_periph_clock_enable(RCU_SPI0);
    /* configure spi parameter */
    spi_struct.device_mode          = SPI_MASTER;
    spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_struct.endian               = SPI_ENDIAN_MSB;
    spi_struct.nss                  = SPI_NSS_SOFT;
    spi_struct.prescale             = SPI_PSC_16;
    spi_init(SPI0, &spi_struct);
    spi_enable(SPI0);
}

/*!
    \brief      send command to card
    \param[in]  cmdx: user expected command to SD card
    \param[in]  argument: the command argument
    \param[in]  crc: CRC value
    \param[out] none
    \retval     command response
*/
uint8_t sd_cmd_send(uint8_t cmdx, uint32_t argument, uint8_t crc)
{
    uint8_t value;  
    uint8_t timeout=0x1F; 
    spi_write(cmdx | 0x40);
    spi_write(argument >> 24);
    spi_write(argument >> 16);
    spi_write(argument >> 8);
    spi_write(argument);    
    spi_write(crc); 
    do{
        value = spi_write(0xFF);
    }while((value&0x80) && timeout--);
    return value;
}

/*!
    \brief      get the SD response
    \param[in]  response: SD_START_DATA_SINGLE_BLOCK_READ or SD_START_DATA_SINGLE_BLOCK_WRITE
    \param[out] none
    \retval     sdcard_error_enum:SD_RESPONSE_FAILURE or SD_RESPONSE_NO_ERROR
*/
sd_error_enum sd_response_get(uint8_t response)
{
    uint32_t timeout = 0xFFFF;
    /* check if response is get or timeout */
    while((response != spi_read()) && timeout){
        timeout--;
    }
    if(0 == timeout)
        return SD_RESPONSE_FAILURE;
    else
        return SD_RESPONSE_NO_ERROR;
}

/*!
    \brief      use spi bus write a byte to sd card
    \param[in]  data: data to write
    \param[out] none
    \retval     the data read from the SPI bus
*/
uint8_t spi_write(uint8_t data)
{
    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_TBE));
    /* send the data */
    spi_i2s_data_transmit(SPI0, data);
    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE));
    /* return the data read from the SPI bus */ 
    return spi_i2s_data_receive(SPI0);
}

/*!
    \brief      use spi bus read a byte from sd card
    \param[in]  none
    \param[out] none
    \retval     the data read from the SPI bus
*/
uint8_t spi_read(void)
{
    uint8_t value = 0;
    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_TBE));
    /* send the dummy byte to generate clock */
    spi_i2s_data_transmit(SPI0, SD_DUMMY_BYTE);
    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE));
    /* get the data from the SPI bus */
    value = spi_i2s_data_receive(SPI0);
    /* return the data value*/
    return value;
}
