/*!
    \file  gd25qxx.c
    \brief SPI flash gd25qxx driver

    \version 2021-08-15, V1.0.0, demo for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "gd25qxx.h"
#include "gd32l23x.h"
#include <string.h>

#define WRITE            0x02     /* write to memory instruction */
#define QUADWRITE        0x32     /* quad write to memory instruction */
#define WRSR             0x01     /* write status register instruction */
#define WREN             0x06     /* write enable instruction */

#define READ             0x03     /* read from memory instruction */
#define QUADREAD         0x6B     /* read from memory instruction */
#define RDSR             0x05     /* read status register instruction */
#define RDID             0x9F     /* read identification */
#define SE               0x20     /* sector erase instruction */
#define BE               0xC7     /* bulk erase instruction */

#define WTSR             0x05     /* write status register instruction */

#define WIP_FLAG         0x01     /* write in progress(wip) flag */
#define DUMMY_BYTE       0xA5

/*!
    \brief      initialize SPI1 GPIO and parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_SPI0);

    /* SPI0_CLK(PB3), SPI0_MISO_IO1(PB4), SPI0_MOSI_IO0(PB5),SPI0_IO2(PB6) and SPI0_IO3(PB7) GPIO pin configuration */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    /* SPI0_CS(PD2) GPIO pin configuration */
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,  GPIO_PIN_2);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_bit_set(GPIOD, GPIO_PIN_2);

    /* chip select invalid */
    SPI_FLASH_CS_HIGH();

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* configure SPI0 byte access to FIFO */
    spi_fifo_access_size_config(SPI0, SPI_BYTE_ACCESS);

    /* set crc polynomial */
    spi_crc_polynomial_set(SPI0, 7);

    /* quad wire SPI_IO2 and SPI_IO3 pin output enable */
    qspi_io23_output_enable(SPI0);

    /* enable SPI0 */
    spi_enable(SPI0);
}

/*!
    \brief      erase the specified flash sector
    \param[in]  sector_addr: address of the sector to erase
    \param[out] none
    \retval     none
*/
void spi_flash_sector_erase(uint32_t sector_addr)
{
    /* send write enable instruction */
    spi_flash_write_enable();

    /* sector erase */
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send sector erase instruction */
    spi_flash_send_byte(SE);
    /* send sector_addr high nibble address byte */
    spi_flash_send_byte((sector_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    spi_flash_send_byte((sector_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    spi_flash_send_byte(sector_addr & 0xFF);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      erase the entire flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_bulk_erase(void)
{
    /* send write enable instruction */
    spi_flash_write_enable();

    /* bulk erase */
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send bulk erase instruction  */
    spi_flash_send_byte(BE);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      write more than one byte to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void spi_flash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    /* enable the write access to the flash */
    spi_flash_write_enable();

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write to memory" instruction */
    spi_flash_send_byte(WRITE);
    /* send write_addr high nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF0000) >> 16);
    /* send write_addr medium nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF00) >> 8);
    /* send write_addr low nibble address byte to write to */
    spi_flash_send_byte(write_addr & 0xFF);

    /* while there is data to be written on the flash */
    while(num_byte_to_write--) {
        /* send the current byte */
        spi_flash_send_byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      write block of data to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void spi_flash_buffer_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr          = write_addr % SPI_FLASH_PAGE_SIZE;
    count         = SPI_FLASH_PAGE_SIZE - addr;
    num_of_page   = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
    num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

    /* write_addr is SPI_FLASH_PAGE_SIZE aligned */
    if(0 == addr) {
        /* num_byte_to_write < SPI_FLASH_PAGE_SIZE */
        if(0 == num_of_page) {
            spi_flash_page_write(pbuffer, write_addr, num_byte_to_write);
        } else {
            /* num_byte_to_write >= SPI_FLASH_PAGE_SIZE */
            while(num_of_page--) {
                spi_flash_page_write(pbuffer, write_addr, SPI_FLASH_PAGE_SIZE);
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }
            spi_flash_page_write(pbuffer, write_addr, num_of_single);
        }
    } else {
        /* write_addr is not SPI_FLASH_PAGE_SIZE aligned */
        if(0 == num_of_page) {
            /* (num_byte_to_write + write_addr) > SPI_FLASH_PAGE_SIZE */
            if(num_of_single > count) {
                temp = num_of_single - count;
                spi_flash_page_write(pbuffer, write_addr, count);
                write_addr += count;
                pbuffer += count;
                spi_flash_page_write(pbuffer, write_addr, temp);
            } else {
                spi_flash_page_write(pbuffer, write_addr, num_byte_to_write);
            }
        } else {
            /* num_byte_to_write >= SPI_FLASH_PAGE_SIZE */
            num_byte_to_write -= count;
            num_of_page = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
            num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

            spi_flash_page_write(pbuffer, write_addr, count);
            write_addr += count;
            pbuffer += count;

            while(num_of_page--) {
                spi_flash_page_write(pbuffer, write_addr, SPI_FLASH_PAGE_SIZE);
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }

            if(0 != num_of_single) {
                spi_flash_page_write(pbuffer, write_addr, num_of_single);
            }
        }
    }
}

/*!
    \brief      read a block of data from the flash
    \param[in]  pbuffer: pointer to the buffer that receives the data read from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[in]  num_byte_to_read: number of bytes to read from the flash
    \param[out] none
    \retval     none
*/
void spi_flash_buffer_read(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
    /* select the flash: chip slect low */
    SPI_FLASH_CS_LOW();

    /* send "read from memory " instruction */
    spi_flash_send_byte(READ);

    /* send read_addr high nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF00) >> 8);
    /* send read_addr low nibble address byte to read from */
    spi_flash_send_byte(read_addr & 0xFF);

    /* while there is data to be read */
    while(num_byte_to_read--) {
        /* read a byte from the flash */
        *pbuffer = spi_flash_send_byte(DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      read flash identification
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/
uint32_t spi_flash_read_id(void)
{
    uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "RDID " instruction */
    spi_flash_send_byte(RDID);

    /* read a byte from the flash */
    temp0 = spi_flash_send_byte(DUMMY_BYTE);

    /* read a byte from the flash */
    temp1 = spi_flash_send_byte(DUMMY_BYTE);

    /* read a byte from the flash */
    temp2 = spi_flash_send_byte(DUMMY_BYTE);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    temp = (temp0 << 16) | (temp1 << 8) | temp2;

    return temp;
}

/*!
    \brief      start a read data byte (read) sequence from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[out] none
    \retval     none
*/
void spi_flash_start_read_sequence(uint32_t read_addr)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "read from memory " instruction */
    spi_flash_send_byte(READ);

    /* send the 24-bit address of the address to read from */
    /* send read_addr high nibble address byte */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte */
    spi_flash_send_byte((read_addr & 0xFF00) >> 8);
    /* send read_addr low nibble address byte */
    spi_flash_send_byte(read_addr & 0xFF);
}

/*!
    \brief      read a byte from the SPI flash
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI flash
*/
uint8_t spi_flash_read_byte(void)
{
    return(spi_flash_send_byte(DUMMY_BYTE));
}

/*!
    \brief      send a byte through the SPI interface and return the byte received from the SPI bus
    \param[in]  byte: byte to send
    \param[out] none
    \retval     the value of the received byte
*/
uint8_t spi_flash_send_byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));

    /* send byte through the SPI1 peripheral */
    spi_i2s_data_transmit(SPI0, byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI0));
}

/*!
    \brief      send a half word through the SPI interface and return the half word received from the SPI bus
    \param[in]  half_word: half word to send
    \param[out] none
    \retval     the value of the received byte
*/
uint16_t spi_flash_send_halfword(uint16_t half_word)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));

    /* send half word through the SPI1 peripheral */
    spi_i2s_data_transmit(SPI0, half_word);

    /* wait to receive a half word */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));

    /* return the half word read from the SPI bus */
    return spi_i2s_data_receive(SPI0);
}

/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_write_enable(void)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write enable" instruction */
    spi_flash_send_byte(WREN);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_wait_for_write_end(void)
{
    uint8_t flash_status = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "read status register" instruction */
    spi_flash_send_byte(RDSR);

    /* loop as long as the memory is busy with a write cycle */
    do {
        /* send a dummy byte to generate the clock needed by the flash
        and put the value of the status register in flash_status variable */
        flash_status = spi_flash_send_byte(DUMMY_BYTE);
    } while(SET == (flash_status & WIP_FLAG));

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/**
  * @brief  enable the flash quad mode
  * @param  None
  * @retval None
  */
void qspi_flash_quad_enable(void)
{
    /* enable the write access to the flash */
    spi_flash_write_enable();
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send "write status register" instruction */
    spi_flash_send_byte(WRSR);

    spi_flash_send_byte(0x00);
    spi_flash_send_byte(0x02);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/**
  * @brief  write block of data to the flash using qspi
  * @param  pbuffer : pointer to the buffer
  * @param  write_addr : flash's internal address to write to
  * @param  num_byte_to_write : number of bytes to write to the flash
  * @retval None
  */
void qspi_flash_buffer_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr = write_addr % SPI_FLASH_PAGE_SIZE;
    count = SPI_FLASH_PAGE_SIZE - addr;
    num_of_page =  num_byte_to_write / SPI_FLASH_PAGE_SIZE;
    num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;
    /* write_addr is SPI_FLASH_PAGE_SIZE aligned */
    if(addr == 0) {
        /* num_byte_to_write < SPI_FLASH_PAGE_SIZE */
        if(num_of_page == 0) {
            qspi_flash_page_write(pbuffer, write_addr, num_byte_to_write);
        } else {
            /* num_byte_to_write >= SPI_FLASH_PAGE_SIZE */
            while(num_of_page--) {
                qspi_flash_page_write(pbuffer, write_addr, SPI_FLASH_PAGE_SIZE);
                write_addr +=  SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }
            qspi_flash_page_write(pbuffer, write_addr, num_of_single);
        }
    } else {
        /* write_addr is not SPI_FLASH_PAGE_SIZE aligned */
        if(num_of_page == 0) {
            /* (num_byte_to_write + write_addr) > SPI_FLASH_PAGE_SIZE */
            if(num_of_single > count) {
                temp = num_of_single - count;
                qspi_flash_page_write(pbuffer, write_addr, count);
                write_addr +=  count;
                pbuffer += count;
                qspi_flash_page_write(pbuffer, write_addr, temp);
            } else {
                qspi_flash_page_write(pbuffer, write_addr, num_byte_to_write);
            }
        } else {
            /* num_byte_to_write >= SPI_FLASH_PAGE_SIZE */
            num_byte_to_write -= count;
            num_of_page =  num_byte_to_write / SPI_FLASH_PAGE_SIZE;
            num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

            qspi_flash_page_write(pbuffer, write_addr, count);
            write_addr +=  count;
            pbuffer += count;

            while(num_of_page--) {
                qspi_flash_page_write(pbuffer, write_addr, SPI_FLASH_PAGE_SIZE);
                write_addr +=  SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }

            if(num_of_single != 0) {
                qspi_flash_page_write(pbuffer, write_addr, num_of_single);
            }
        }
    }
}

/**
  * @brief  read a block of data from the flash using qspi
  * @param  pbuffer : pointer to the buffer that receives the data read from the flash
  * @param  read_addr : flash's internal address to read from
  * @param  num_byte_to_read : number of bytes to read from the flash
  * @retval None
  */
void qspi_flash_buffer_read(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send "quad fast read from memory " instruction */
    spi_flash_send_byte(QUADREAD);

    /* send read_addr high nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF00) >> 8);
    /* send read_addr low nibble address byte to read from */
    spi_flash_send_byte(read_addr & 0xFF);
    spi_flash_send_byte(0xA5);

    /* enable the qspi */
    qspi_enable(SPI0);
    /* enable the qspi read operation */
    qspi_read_enable(SPI0);

    /* while there is data to be read */
    while(num_byte_to_read--) {
        /* read a byte from the flash */
        *pbuffer = spi_flash_send_byte(DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
    /* disable the qspi */
    qspi_disable(SPI0);
    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/**
  * @brief  write more than one byte to the flash using qspi
  * @param  pbuffer : pointer to the buffer
  * @param  write_addr : flash's internal address to write to
  * @param  num_byte_to_write : number of bytes to write to the flash
  * @retval none
  */
void qspi_flash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    /* enable the flash quad mode */
    qspi_flash_quad_enable();
    /* enable the write access to the flash */
    spi_flash_write_enable();

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send "quad write to memory " instruction */
    spi_flash_send_byte(QUADWRITE);
    /* send writeaddr high nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF0000) >> 16);
    /* send writeaddr medium nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF00) >> 8);
    /* send writeaddr low nibble address byte to write to */
    spi_flash_send_byte(write_addr & 0xFF);
    /* enable the qspi */
    qspi_enable(SPI0);
    /* enable the qspi write operation */
    qspi_write_enable(SPI0);

    /* while there is data to be written on the flash */
    while(num_byte_to_write--) {
        /* send the current byte */
        spi_flash_send_byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
    /* disable the qspi function */
    qspi_disable(SPI0);
    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}
