/*!
    \file  lcd_driver.c
    \brief lcd driver functions

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
#include "lcd_driver.h"
#include "systick.h"
#include "spi_sd.h"
#ifdef H_VIEW
    #define X_MAX_PIXEL         (uint16_t)320
    #define Y_MAX_PIXEL         (uint16_t)240
#else
    #define X_MAX_PIXEL         (uint16_t)240
    #define Y_MAX_PIXEL         (uint16_t)320
#endif

static uint8_t spi_write_byte(uint32_t spi_periph,uint8_t byte);
static void spi0_init(void);
static void lcd_write_index(uint8_t index);
static void lcd_write_data(uint8_t data);

static void lcd_write_data_16bit(uint8_t datah,uint8_t datal);
static void lcd_reset(void);

/*!
    \brief      send a byte through the SPI interface and return a byte received from the SPI bus
    \param[in]  spi_periph: SPIx(x=0,1)
    \param[in]  byte: data to be send
    \param[out] none
    \retval     the value of the received byte
*/
static uint8_t spi_write_byte(uint32_t spi_periph,uint8_t byte)
{
    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_TBE));
    SPI_DATA(spi_periph) = byte;

    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_RBNE));
    return(SPI_DATA(spi_periph));
} 

/*!
    \brief      initialize SPI0
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi0_init(void)
{
    spi_parameter_struct spi_init_struct;
    rcu_periph_clock_enable(RCU_SPI0);

    /* GPIOB config, PB3(LCD_SPI_CLK), PB4(SPI0_MISO), PB5(LCD_SPI_MOSI) */
    gpio_af_set(GPIOB, GPIO_AF_0,GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    /* GPIOC config */
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4);

    /* GPIOF config */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	
    /* SD card CS GPIO config */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	
    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* set crc polynomial */
    spi_crc_polynomial_set(SPI0, 7);
    spi_enable(SPI0);
}

/*!
    \brief      write the register address
    \param[in]  index: the value of register address to be written
    \param[out] none
    \retval     none
*/
static void lcd_write_index(uint8_t index)
{
    LCD_RS_CLR;
    spi_write_byte(SPI0,index);
}

/*!
    \brief      write the register data
    \param[in]  data: the value of register data to be written
    \param[out] none
    \retval     none
*/
static void lcd_write_data(uint8_t data)
{
    LCD_RS_SET;
    spi_write_byte(SPI0,data);
}

/*!
    \brief      write the register data(an unsigned 16-bit data)
    \param[in]  datah: the high 8bit of register data to be written
    \param[in]  datal: the low 8bit of register data to be written
    \param[out] none
    \retval     none
*/
static void lcd_write_data_16bit(uint8_t datah,uint8_t datal)
{
    lcd_write_data(datah);
    lcd_write_data(datal);
}

/*!
    \brief      reset the lcd
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_reset(void)
{
    LCD_RST_CLR;
    delay_1ms(100);
    LCD_RST_SET;
    delay_1ms(50);
}

/*!
    \brief      initialize the lcd
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    spi0_init();
    /* deselect SD card , avoid disturb the SD card init*/
    SD_CS_HIGH();
    LCD_CS_CLR;
    lcd_reset();

    /* write the register address 0xCB*/
    lcd_write_index(0xCB);
    lcd_write_data(0x39);
    lcd_write_data(0x2C);
    lcd_write_data(0x00);
    lcd_write_data(0x34);
    lcd_write_data(0x02);

    /* write the register address 0xCF*/
    lcd_write_index(0xCF);
    lcd_write_data(0x00);
    lcd_write_data(0XC1);
    lcd_write_data(0X30);

    /* write the register address 0xE8*/
    lcd_write_index(0xE8);
    lcd_write_data(0x85);
    lcd_write_data(0x00);
    lcd_write_data(0x78);

    /* write the register address 0xEA*/
    lcd_write_index(0xEA);
    lcd_write_data(0x00);
    lcd_write_data(0x00);

    /* write the register address 0xED*/
    lcd_write_index(0xED);
    lcd_write_data(0x64);
    lcd_write_data(0x03);
    lcd_write_data(0X12);
    lcd_write_data(0X81);

    /* write the register address 0xF7*/
    lcd_write_index(0xF7);
    lcd_write_data(0x20);

    /* power control VRH[5:0] */
    lcd_write_index(0xC0);
    lcd_write_data(0x23);

    /* power control SAP[2:0];BT[3:0] */
    lcd_write_index(0xC1);
    lcd_write_data(0x10);

    /* vcm control */
    lcd_write_index(0xC5);
    lcd_write_data(0x3e);
    lcd_write_data(0x28); 

    /* vcm control2 */
    lcd_write_index(0xC7);
    lcd_write_data(0x86);

    lcd_write_index(0x36);
#ifdef H_VIEW
    lcd_write_data(0xE8);
#else
    lcd_write_data(0x48); 
#endif
    /* write the register address 0x3A*/
    lcd_write_index(0x3A);
    lcd_write_data(0x55);

    /* write the register address 0xB1*/
    lcd_write_index(0xB1);
    lcd_write_data(0x00);
    lcd_write_data(0x18);

    /* display function control */
    lcd_write_index(0xB6);
    lcd_write_data(0x08); 
    lcd_write_data(0x82);
    lcd_write_data(0x27);  

    /* 3gamma function disable */
    lcd_write_index(0xF2);
    lcd_write_data(0x00); 

    /* gamma curve selected  */
    lcd_write_index(0x26);
    lcd_write_data(0x01); 

    /* set gamma  */
    lcd_write_index(0xE0);
    lcd_write_data(0x0F);
    lcd_write_data(0x31);
    lcd_write_data(0x2B);
    lcd_write_data(0x0C);
    lcd_write_data(0x0E);
    lcd_write_data(0x08);
    lcd_write_data(0x4E);
    lcd_write_data(0xF1);
    lcd_write_data(0x37);
    lcd_write_data(0x07);
    lcd_write_data(0x10);
    lcd_write_data(0x03);
    lcd_write_data(0x0E);
    lcd_write_data(0x09);
    lcd_write_data(0x00);

    /* set gamma  */
    lcd_write_index(0XE1);
    lcd_write_data(0x00);
    lcd_write_data(0x0E);
    lcd_write_data(0x14);
    lcd_write_data(0x03);
    lcd_write_data(0x11);
    lcd_write_data(0x07);
    lcd_write_data(0x31);
    lcd_write_data(0xC1);
    lcd_write_data(0x48);
    lcd_write_data(0x08);
    lcd_write_data(0x0F);
    lcd_write_data(0x0C);
    lcd_write_data(0x31);
    lcd_write_data(0x36);
    lcd_write_data(0x0F);

    /* exit sleep */
    lcd_write_index(0x11);
    delay_1ms(120); 

    /* display on */
    lcd_write_index(0x29);
    lcd_write_index(0x2c);

    LCD_CS_SET;
}

/*!
    \brief      set lcd display region
    \param[in]  x_start: the x position of the start point
    \param[in]  y_start: the y position of the start point
    \param[in]  x_end: the x position of the end point
    \param[in]  y_end: the y position of the end point
    \param[out] none
    \retval     none
*/
void lcd_set_region(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    LCD_CS_CLR;

    /* write the register address 0x2A*/
    lcd_write_index(0x2A);
    lcd_write_data_16bit(x_start >> 8,x_start);
    lcd_write_data_16bit(x_end >> 8,x_end);

    /* write the register address 0x2B*/
    lcd_write_index(0x2B);
    lcd_write_data_16bit(y_start >> 8,y_start);
    lcd_write_data_16bit(y_end >> 8,y_end);

    /* write the register address 0x2C*/
    lcd_write_index(0x2C);
    LCD_CS_SET;
}

/*!
    \brief      set the start display point of lcd
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[out] none
    \retval     none
*/
void lcd_set_xy(uint16_t x, uint16_t y)
{
    /* write the register address 0x2A*/
    lcd_write_index(0x2A);
    lcd_write_data_16bit(x >> 8,x);

    /* write the register address 0x2B*/
    lcd_write_index(0x2B);
    lcd_write_data_16bit(y >> 8,y);

    /* write the register address 0x2C*/
    lcd_write_index(0x2C);
}

/*!
    \brief      draw a point on the lcd
    \param[in]  x: the x position of the point 
    \param[in]  y: the y position of the point 
    \param[in]  data: the register data to be written
    \param[out] none
    \retval     none
*/
void gui_draw_point(uint16_t x,uint16_t y,uint16_t data)
{
    lcd_set_xy(x,y);
    lcd_write_data(data >> 8);
    lcd_write_data(data);
}

/*!
    \brief      clear the lcd
    \param[in]  color: lcd display color 
    \param[out] none
    \retval     none
*/
void lcd_clear(uint16_t color)
{
    unsigned int i,m;
    /* set lcd display region */
    lcd_set_region(0,0,X_MAX_PIXEL - 1,Y_MAX_PIXEL - 1);
    LCD_RS_SET;

    LCD_CS_CLR;
    for(i = 0;i < Y_MAX_PIXEL;i ++){
        for(m = 0;m < X_MAX_PIXEL;m ++){
            spi_write_byte(SPI0,color >> 8);
            spi_write_byte(SPI0,color);
        }
    }
    LCD_CS_SET;
}
