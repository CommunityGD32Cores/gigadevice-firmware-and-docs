/*!
    \file    sqpi_flash.c
    \brief   SQPI flash driver
    
    \version 2020-03-10, V1.0.0, demo for GD32E50x
    \version 2020-08-26, V1.1.0, demo for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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
#include "sqpi_flash.h"
#include "gd32e50x_sqpi.h"
#include "systick.h"

#define WREN             0x06     /* write enable instruction */
#define WRDI             0x04     /* write disable instruction */
#define WRSR             0x01     /* write status register instruction */
#define RDSR             0x05     /* read status register instruction */
#define READ             0x03     /* read from memory instruction */
#define FASTREAD         0x0B     /* read data at higher speed */
#define QUADREAD         0x6B     /* read data at quad fast read */
#define WRITE            0x02     /* write to memory instruction */
#define QUADWRITE        0x32     /* quad write to memory instruction */
#define SE               0x20     /* sector erase instruction */
#define CE               0xC7     /* chip erase instruction */
#define RDID             0x9F     /* read identification */
#define WIP_FLAG         0x01     /* write in progress(wip) flag */
#define WEL_FLAG         0x02     /* write enable latch(wel) flag */

#define SPI_FLASH_PAGE_SIZE       0x100

#define SQPI_ADDR_0BIT       (SQPI_INIT & (~(uint32_t)0x1F000000u))
#define SQPI_ADDR_16BITS     (SQPI_ADDR_0BIT |0x10000000u)
#define SQPI_ADDR_24BITS     (SQPI_ADDR_0BIT |0x18000000u)
#define SQPI_LOGIC_ADDR      ((uint32_t)0xB0000000)

static void sqpi_state_check(uint8_t mask, uint8_t check);
static void sqpi_flash_page_write(uint32_t write_addr,uint8_t * write_buff,uint32_t size);
static void sqpi_flash_write_enable(void);
/*!
    \brief      check SQPI status register
    \param[in]  mask: mask value
    \param[in]  check: check value
    \param[out] none
    \retval     none
*/
static void sqpi_state_check(uint8_t mask, uint8_t check)
{
    uint32_t temp= 0;
    SQPI_INIT = SQPI_ADDR_0BIT;
    sqpi_read_command_config(SQPI_MODE_SSS,0,RDSR);

    do{
        temp = *(uint8_t *)SQPI_LOGIC_ADDR;
    }while((temp & mask)!=check);
}

/*!
    \brief      write more than one byte to the flash
    \param[in]  write_addr: flash's internal address to write
    \param[in]  write_buff: pointer to the buffer
    \param[in]  size: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
static void sqpi_flash_page_write(uint32_t write_addr,uint8_t * write_buff,uint32_t size)
{
    uint32_t i =0;

    for(i=0;i<size;i++){  
        sqpi_flash_write_enable();   
        SQPI_INIT = SQPI_ADDR_24BITS;
        sqpi_write_command_config(SQPI_MODE_SSQ,0,QUADWRITE);
        /* read a byte from the flash */
        SQPI_INIT = SQPI_ADDR_24BITS;
        *(uint8_t *)(SQPI_LOGIC_ADDR+write_addr+i)  = *write_buff;
        /* point to the next location where the byte read will be saved */
        write_buff++;
        sqpi_state_check(0x01,0x00);
    }
}

/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void sqpi_flash_write_enable(void)
{
    SQPI_INIT = SQPI_ADDR_0BIT;
    sqpi_write_command_config(SQPI_MODE_SSS,0,WREN);
    sqpi_special_command();
    sqpi_state_check(WEL_FLAG,0x02);
}

/*!
    \brief      initialize SQPI parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_flash_init(void)
{ 
    sqpi_parameter_struct  sqpi_para;

    /* SQPI parameter config */
    sqpi_para.addr_bit = 0;
    sqpi_para.clk_div = 20;
    sqpi_para.cmd_bit = SQPI_CMDBIT_8_BITS;
    sqpi_para.id_length = SQPI_ID_LENGTH_32_BITS;
    sqpi_para.polarity = SQPI_SAMPLE_POLARITY_RISING;

    sqpi_init( &sqpi_para);
    
}

/*!
    \brief      enable flash quad operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_flash_quad_enable(void)
{   
    sqpi_flash_write_enable();
    SQPI_INIT = SQPI_ADDR_0BIT;
    sqpi_write_command_config(SQPI_MODE_SSS,0,WRSR);
    *(uint16_t *)SQPI_LOGIC_ADDR = 0x0200;
}

/*!
    \brief      read flash identification
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/
uint32_t sqpi_flash_read_id(void)
{
    uint32_t temp= 0;
    
    SQPI_INIT = SQPI_ADDR_0BIT;
    sqpi_read_command_config(SQPI_MODE_SSS,0,RDID);
    sqpi_read_id_command();
    delay_1ms(1);
    temp = sqpi_high_id_receive();
    temp = sqpi_low_id_receive();    
    return ( temp & 0xFFFFFF00);
}

/*!
    \brief      erase the specified flash sector
    \param[in]  sector_addr: address of the sector to erase
    \param[out] none
    \retval     none
*/
void sqpi_flash_sector_erase(uint32_t sector_addr)
{
    sqpi_flash_write_enable();   
    SQPI_INIT = SQPI_ADDR_16BITS;
    sqpi_write_command_config(SQPI_MODE_SSS,0,SE);
    *(uint8_t *)(SQPI_LOGIC_ADDR+ ((sector_addr&0x00FFFF00)>>8))= (uint8_t)(sector_addr&0xFF);
    
    sqpi_state_check(WIP_FLAG,0x00);
}

/*!
    \brief      erase the entire flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sqpi_flash_chip_erase(void)
{
    sqpi_flash_write_enable();   

    SQPI_INIT = SQPI_ADDR_0BIT;
    sqpi_write_command_config(SQPI_MODE_SSS,0,CE);
    sqpi_special_command();

    sqpi_state_check(WIP_FLAG,0x00);
}

/*!
    \brief      read a block of data from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[in]  read_buff: pointer to the buffer that receives the data read from the flash
    \param[in]  size: number of bytes to read from the flash
    \param[out] none
    \retval     none
*/
void sqpi_flash_buffer_read(uint32_t read_addr,uint8_t * read_buff,uint32_t size)
{
    uint32_t i =0;
    SQPI_INIT = SQPI_ADDR_24BITS;
    sqpi_read_command_config(SQPI_MODE_SSQ,8,QUADREAD);
    for(i=0;i<size;i++){
        /* read a byte from the flash */
        *read_buff =*(uint8_t *)(SQPI_LOGIC_ADDR+read_addr+i);
        /* point to the next location where the byte read will be saved */
        read_buff++;
    }
}

/*!
    \brief      write block of data to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void sqpi_flash_buffer_write(uint32_t write_addr, uint8_t* pbuffer,  uint16_t num_byte_to_write)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr          = write_addr % SPI_FLASH_PAGE_SIZE;
    count         = SPI_FLASH_PAGE_SIZE - addr;
    num_of_page   = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
    num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

    /* write_addr is SPI_FLASH_PAGE_SIZE aligned */
    if(0 == addr){
        /* num_byte_to_write < SPI_FLASH_PAGE_SIZE */
        if(0 == num_of_page){
            sqpi_flash_page_write(write_addr,pbuffer,num_byte_to_write);
        }else{
            /* num_byte_to_write >= SPI_FLASH_PAGE_SIZE */
            while(num_of_page--){
                sqpi_flash_page_write(write_addr,pbuffer,SPI_FLASH_PAGE_SIZE);
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }
            sqpi_flash_page_write(write_addr,pbuffer,num_of_single);
        }
    }else{
        /* write_addr is not SPI_FLASH_PAGE_SIZE aligned */
        if(0 == num_of_page){
            /* (num_byte_to_write + write_addr) > SPI_FLASH_PAGE_SIZE */
            if(num_of_single > count){
                temp = num_of_single - count;
                sqpi_flash_page_write(write_addr,pbuffer,count);
                write_addr += count;
                pbuffer += count;
                sqpi_flash_page_write(write_addr,pbuffer,temp);
            }else{
                sqpi_flash_page_write(write_addr,pbuffer,num_byte_to_write);
            }
        }else{
            /* num_byte_to_write >= SPI_FLASH_PAGE_SIZE */
            num_byte_to_write -= count;
            num_of_page = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
            num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

            sqpi_flash_page_write(write_addr,pbuffer, count);
            write_addr += count;
            pbuffer += count;

            while(num_of_page--){
                sqpi_flash_page_write(write_addr,pbuffer,SPI_FLASH_PAGE_SIZE);
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }

            if(0 != num_of_single){
                sqpi_flash_page_write(write_addr,pbuffer,num_of_single);
            }
        }
    }
}
