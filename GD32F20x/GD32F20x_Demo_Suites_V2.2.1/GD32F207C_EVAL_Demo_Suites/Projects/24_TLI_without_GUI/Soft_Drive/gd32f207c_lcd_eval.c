/*!
    \file  gd32f207c_lcd_eval.c
    \brief gd32f207c LCD driver

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2018-10-31, V2.1.0, demo for GD32F20x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f207c_lcd_eval.h"

#define HORIZONTAL_SYNCHRONOUS_PULSE  11
#define HORIZONTAL_BACK_PORCH         20
#define ACTIVE_WIDTH                  LCD_PIXEL_WIDTH
#define HORIZONTAL_FRONT_PORCH        40

#define VERTICAL_SYNCHRONOUS_PULSE    3
#define VERTICAL_BACK_PORCH           1
#define ACTIVE_HEIGHT                 LCD_PIXEL_HEIGHT
#define VERTICAL_FRONT_PORCH          4

static void delay(uint32_t count);
tli_layer_parameter_struct tli_layer_initstruct;

/*!
    \brief      LCD GPIO configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_gpio_config(void)
{
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_AF);
    
    /* TLI GPIO pin remap configuration */
    gpio_pin_remap1_config(GPIO_PCF3, GPIO_PCF3_TLI_HSYNC_PC6_REMAP | GPIO_PCF3_TLI_VSYNC_PA4_REMAP | 
                                    GPIO_PCF3_TLI_DE_PE13_REMAP | GPIO_PCF3_TLI_R7_PE15_REMAP | 
                                    GPIO_PCF3_TLI_R6_PB1_REMAP | GPIO_PCF3_TLI_R5_PA12_REMAP | 
                                    GPIO_PCF3_TLI_R4_PA11_REMAP | GPIO_PCF3_TLI_R3_PB0_REMAP | 
                                    GPIO_PCF3_TLI_G7_PD3_REMAP | GPIO_PCF3_TLI_G6_PC7_REMAP | 
                                    GPIO_PCF3_TLI_G5_PB11_REMAP | GPIO_PCF3_TLI_G4_PB10_REMAP | 
                                    GPIO_PCF3_TLI_G3_PE11_REMAP | GPIO_PCF3_TLI_B7_PB9_REMAP |
                                    GPIO_PCF3_TLI_B6_PB8_REMAP | GPIO_PCF3_TLI_B5_PA3_REMAP |
                                    GPIO_PCF3_TLI_B4_PE12_REMAP | GPIO_PCF3_TLI_B3_PD10_REMAP | 
                                    GPIO_PCF4_TLI_B1_PG12_REMAP | GPIO_PCF3_TLI_CLK_PE14_REMAP, ENABLE);

    /* configure HSYNC(PC6), VSYNC(PA4), PCLK(PE14), DE(PE13) */
    /* configure LCD_R7(PE15), LCD_R6(PB1), LCD_R5(PA12), LCD_R4(PA11), LCD_R3(PB0), LCD_G7(PD3), 
                 LCD_G6(PC7), LCD_G5(PB11), LCD_G4(PB10), LCD_G3(PE11), LCD_G2(PA6), LCD_B7(PB9), 
                 LCD_B6(PB8), LCD_B5(PA3), LCD_B4(PE12), LCD_B3(PD10) */
    gpio_init(GPIOE,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | 
                                                    GPIO_PIN_14 | GPIO_PIN_15); 
    
    gpio_init(GPIOD,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_3 | GPIO_PIN_10);
    
    gpio_init(GPIOC,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6 | GPIO_PIN_7);  
    
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 |
                                                    GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);     

    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_11 | 
                                                    GPIO_PIN_12);
}

/*!
    \brief      initialize the LCD clock, GPIO and TLI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    tli_parameter_struct tli_init_struct;
    rcu_periph_clock_enable(RCU_TLI);

    /* configure the TLI clock to generate LCD clock */
    rcu_pllt_config(RCU_PLLTSRC_HXTAL);
#ifdef __CC_ARM
    if(ERROR == rcu_pllt_vco_config(13, 192, 3)){
        while(1);
    }
#endif /* __CC_ARM */
#ifdef __IAR_SYSTEMS_ICC__
    if(ERROR == rcu_pllt_vco_config(25, 192, 4)){
        while(1);
    }
#endif /* __IAR_SYSTEMS_ICC__ */
    rcu_tli_clock_config(RCU_CKTLI_CKPLLTR_DIV8);

    rcu_osci_on(RCU_PLLT_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLT_CK)){
        while(1);
    }
    
    /* TLI initialization */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* LCD background color configure*/
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF; 
    tli_init(&tli_init_struct);
    
    lcd_gpio_config();
}

/*!
    \brief      initialize TLI layer0 or layer1
    \param[in]  layer: LCD layer
      \arg        LCD_LAYER_BACKGROUND
      \arg        LCD_LAYER_FOREGROUND
    \param[in]  width: width of the window
    \param[in]  height: height of the window
*/
void lcd_layer_init(uint32_t layer, uint32_t width, uint32_t height)
{
    if(LCD_LAYER_BACKGROUND == layer){
        /* layer 0 configuration */
        /* windowing configuration */ 
//        tli_layer_initstruct.layer_window_leftpos = (HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
//        tli_layer_initstruct.layer_window_rightpos = (width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
//        tli_layer_initstruct.layer_window_toppos = (VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
//        tli_layer_initstruct.layer_window_bottompos = (height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);

        tli_layer_initstruct.layer_window_leftpos = 30;
        tli_layer_initstruct.layer_window_rightpos = 320 + 30 - 1;
        tli_layer_initstruct.layer_window_toppos = 4 + 200;
        tli_layer_initstruct.layer_window_bottompos = 162 + 4 + 200 - 1;
        
        /* pixel format configuration */
        tli_layer_initstruct.layer_ppf = LAYER_PPF_RGB565;

        /* alpha constant configuration: the constant alpha for layer 1 is decreased 
        to see the layer 0 in the intersection zone*/
        tli_layer_initstruct.layer_sa = 255;
      
        /* default color configuration (configure A,R,G,B component values) */
        tli_layer_initstruct.layer_default_blue = 0xFF;
        tli_layer_initstruct.layer_default_green = 0xFF;
        tli_layer_initstruct.layer_default_red = 0xFF;
        tli_layer_initstruct.layer_default_alpha = 0x00;

        /* blending factors */    
        tli_layer_initstruct.layer_acf1 = LAYER_ACF1_PASA;
        tli_layer_initstruct.layer_acf2 = LAYER_ACF2_PASA;

        tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_background;

        tli_layer_initstruct.layer_frame_line_length = ((width * 2) + 3);
        tli_layer_initstruct.layer_frame_buf_stride_offset = (width * 2);
        tli_layer_initstruct.layer_frame_total_line_number = height; 

        tli_layer_init(LAYER0, &tli_layer_initstruct);
    }else if(LCD_LAYER_FOREGROUND == layer){
        /* layer 1 configuration */
        /* windowing configuration */
//        tli_layer_initstruct.layer_window_leftpos = (HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
//        tli_layer_initstruct.layer_window_rightpos = (width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
//        tli_layer_initstruct.layer_window_toppos = (VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
//        tli_layer_initstruct.layer_window_bottompos = (height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
        
        tli_layer_initstruct.layer_window_leftpos = 110;
        tli_layer_initstruct.layer_window_rightpos = 140 + 110 - 1;
        tli_layer_initstruct.layer_window_toppos = 100;
        tli_layer_initstruct.layer_window_bottompos = 60 + 100 - 1;
        
        /* pixel format configuration */           
        tli_layer_initstruct.layer_ppf = LAYER_PPF_RGB565;

        /* alpha constant configuration: the constant alpha for layer 1 is decreased 
        to see the layer 0 in the intersection zone*/
        tli_layer_initstruct.layer_sa = 255;
      
        /* default color configuration (configure A,R,G,B component values) */           
        tli_layer_initstruct.layer_default_blue = 0xFF;
        tli_layer_initstruct.layer_default_green = 0xFF;
        tli_layer_initstruct.layer_default_red = 0xFF;
        tli_layer_initstruct.layer_default_alpha = 0x0;

        /* blending factors */    
        tli_layer_initstruct.layer_acf1 = LAYER_ACF1_PASA;
        tli_layer_initstruct.layer_acf2 = LAYER_ACF2_PASA;

//        tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)&image_background;

        tli_layer_initstruct.layer_frame_line_length = ((width * 2) + 3);
        tli_layer_initstruct.layer_frame_buf_stride_offset = (width * 2);
        tli_layer_initstruct.layer_frame_total_line_number = height; 

        tli_layer_init(LAYER1, &tli_layer_initstruct);
    }

    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*!
    \brief      configure the LCD control lines
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_control_lines_config(void)
{
    /* enable LCD_CS and LCD_RS clock */
    rcu_periph_clock_enable(LCD_CS_GPIO_CLK);
    rcu_periph_clock_enable(LCD_RS_GPIO_CLK);
    
    /* configure LCD_CS and LCD_RS line */
    gpio_init(LCD_CS_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_CS_PIN);
    gpio_init(LCD_RS_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_RS_PIN);
    
    /* set chip select pin high */
    lcd_control_lines_set(LCD_CS_GPIO_PORT, LCD_CS_PIN);
}

/*!
    \brief      set the LCD control lines
    \param[in]  gpiox: LCD_CS_GPIO_PORT, LCD_RS_GPIO_PORT
    \param[in]  gpio_pin: LCD_CS_PIN, LCD_RS_PIN
    \param[out] none
    \retval     none
*/
void lcd_control_lines_set(uint32_t gpiox, uint32_t gpio_pin)
{
    gpio_bit_set(gpiox, gpio_pin);
}

/*!
    \brief      reset the LCD control lines
    \param[in]  gpiox: LCD_CS_GPIO_PORT, LCD_RS_GPIO_PORT
    \param[in]  gpio_pin: LCD_CS_PIN, LCD_RS_PIN
    \param[out] none
    \retval     none
*/
void lcd_control_lines_reset(uint32_t gpiox, uint32_t gpio_pin)
{
    gpio_bit_reset(gpiox, gpio_pin);
}

/*!
    \brief      configure the LCD SPI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_spi_config(void)
{
    spi_parameter_struct spi_initstruct;
    /* enable LCD SPI GPIO clock */
    rcu_periph_clock_enable(LCD_SPI_SCK_GPIO_CLK);
    rcu_periph_clock_enable(LCD_SPI_MISO_GPIO_CLK);
    rcu_periph_clock_enable(LCD_SPI_MOSI_GPIO_CLK);
    rcu_periph_clock_enable(LCD_SPI_CLK);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure the LCD SPI pins */
    gpio_init(LCD_SPI_SCK_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LCD_SPI_SCK_PIN);
    gpio_init(LCD_SPI_MOSI_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MOSI_PIN);
    
    spi_i2s_deinit(LCD_SPI);
    
    /* configure SPI */
    spi_initstruct.device_mode = SPI_MASTER;
    spi_initstruct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_initstruct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_initstruct.nss = SPI_NSS_SOFT;
    spi_initstruct.endian = SPI_ENDIAN_MSB;
    spi_initstruct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_initstruct.prescale = SPI_PSC_16;
    spi_init(LCD_SPI, &spi_initstruct);
    spi_crc_polynomial_set(LCD_SPI, 7);
    
    /* enable LCD SPI */
    spi_enable(LCD_SPI);
}

/*!
    \brief      configure LCD chip select (CS) pin
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_chip_select(ControlStatus staus)
{
    if(ENABLE == staus){
        /* chip select (CS) pin low: LCD enabled */
        gpio_bit_set(LCD_CS_GPIO_PORT, LCD_CS_PIN);
    }else{
        /* chip select (CS) pin low: LCD disabled */
        gpio_bit_reset(LCD_CS_GPIO_PORT, LCD_CS_PIN);
    }
}

/*!
    \brief      write command to the LCD register
    \param[in]  lcd_reg: selected register address
    \param[out] none
    \retval     none
*/
void lcd_command_write(uint8_t lcd_reg)
{
    /* reset RS to send command */
    lcd_control_lines_reset(LCD_RS_GPIO_PORT, LCD_RS_PIN);
    
    lcd_chip_select(DISABLE);
    
    spi_i2s_data_transmit(LCD_SPI, lcd_reg);
    while(RESET == spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TBE)) ;
    while(RESET != spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TRANS)) ;
    
    lcd_chip_select(ENABLE);
}

/*!
    \brief      write data to the LCD register
    \param[in]  data: data to be written to the selected register
    \param[out] none
    \retval     none
*/
void lcd_data_write(uint8_t data)
{
    /* set RS to send data */
    lcd_control_lines_set(LCD_RS_GPIO_PORT, LCD_RS_PIN);
    
    lcd_chip_select(DISABLE);
    
    spi_i2s_data_transmit(LCD_SPI, data);
    while(RESET == spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TBE)) ;
    while(RESET != spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TRANS)) ;
    
    lcd_chip_select(ENABLE);
}

/*!
    \brief      configure the LCD controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_power_on(void)
{
    lcd_command_write(0x11);
    delay(120);
    lcd_command_write(0x36);
    lcd_data_write(0x48);
    lcd_command_write(0x3A);
    lcd_data_write(0x55);
    lcd_command_write(0xB4);
    lcd_data_write(0x11);
    lcd_command_write(0xB3);
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    lcd_data_write(0x20);
    lcd_command_write(0xC0);
    lcd_data_write(0x10);
    lcd_data_write(0x3B);
    lcd_data_write(0x00);
    lcd_data_write(0x12);
    lcd_data_write(0x01);
    lcd_command_write(0xC5);
    lcd_data_write(0x07);
    lcd_command_write(0xC8);
    lcd_data_write(0x01 );
    lcd_data_write(0x36);
    lcd_data_write(0x00);
    lcd_data_write(0x02);
    lcd_data_write(0x00);
    lcd_data_write(0x1C);
    lcd_data_write(0x77);
    lcd_data_write(0x14);
    lcd_data_write(0x67);
    lcd_data_write(0x20);
    lcd_data_write(0x0E);
    lcd_data_write(0x00);
    lcd_command_write(0xD0);
    lcd_data_write(0x44);
    lcd_data_write(0x41);
    lcd_data_write(0x08);
    lcd_data_write(0xC2);
    lcd_command_write(0xD1);
    lcd_data_write(0x50); 
    lcd_data_write(0x11);
    lcd_command_write(0xD2);
    lcd_data_write(0x05);
    lcd_data_write(0x12);

    lcd_command_write(0xC6);
    lcd_data_write(0x83);
    lcd_command_write(0x29);
    delay(5);
}

/*!
    \brief      configure the LCD based on the power on sequence 1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_power_on1(void)
{
    lcd_command_write(0xC0); //power control1 command/w/
    lcd_data_write(0x0A);    // P-Gamma level//4.1875v
    lcd_data_write(0x0A);    // N-Gamma level
    lcd_command_write(0xC1); // BT & VC Setting//power contrl2 command/w/
    lcd_data_write(0x41);
    lcd_data_write(0x07);    // VCI1 = 2.5V
    lcd_command_write(0xC2); // DC1.DC0 Setting//power control3 for normal mode
    lcd_data_write(0x33);
    lcd_command_write(0xC5); //VCOM control
    lcd_data_write(0x00);    //NV memory is not programmed
    lcd_data_write(0x42);    // VCM Setting
    lcd_data_write(0x80);    // VCM Register Enable
    lcd_command_write(0xB0); //interface mode control //Polarity Setting
    lcd_data_write(0x02);
    lcd_command_write(0xB1); //frame rate control for normal mode
    lcd_data_write(0xB0);    // Frame Rate Setting//70 frame per second//no division for internal clocks
    lcd_data_write(0x11);    //17 clocks per line period for idle mode at cpu interface
    lcd_command_write(0xB4); //dispaly inversion control
    lcd_data_write(0x00);    // disable Z-inversion , column inversion
    lcd_command_write(0xB6); //display function control// RM.DM Setting
    lcd_data_write(0x70);    //0xF0
    lcd_data_write(0x02);    //direction of gate scan: G1->G480 one by one, source scan: S1->S960, scan cycle if interval scan in non-display area
    lcd_data_write(0x3B);    // number of lines to drive LCD: 8*(0x3C) = 480
    lcd_command_write(0xB7); // Entry Mode
    lcd_data_write(0x07);    // disable low voltage detection, normal display, 
    lcd_command_write(0xF0); // Enter ENG , must be set before gamma setting
    lcd_data_write(0x36);
    lcd_data_write(0xA5);
    lcd_data_write(0xD3);
    lcd_command_write(0xE5); // Open gamma function , must be set before gamma setting
    lcd_data_write(0x80);
    lcd_command_write(0xE5); // Page 1
    lcd_data_write(0x01);
    lcd_command_write(0XB3); // WEMODE=0(Page 1) , pixels over window setting will be ignored.//frame rate control in partial mode/full colors
    lcd_data_write(0x00);
    lcd_command_write(0xE5); // Page 0
    lcd_data_write(0x00);
    lcd_command_write(0xF0); // Exit ENG , must be set before gamma setting
    lcd_data_write(0x36);
    lcd_data_write(0xA5);
    lcd_data_write(0x53);
    lcd_command_write(0xE0); // Gamma setting
    //y fine adjustment register for positive polarity
    lcd_data_write(0x00);
    lcd_data_write(0x35);
    lcd_data_write(0x33);
    //y gradient adjustment register for positive polarity
    lcd_data_write(0x00);
    //y amplitude adjustment register for positive polarity
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    //y fine adjustment register for negative polarity
    lcd_data_write(0x00);
    lcd_data_write(0x35);
    lcd_data_write(0x33);
    //y gradient adjustment register for negative polarity
    lcd_data_write(0x00);
    //y amplitude adjustment register for negative polarity
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    lcd_command_write(0x36); // memory data access control //
    lcd_data_write(0x48);
    lcd_command_write(0x3A); // interface pixel format setting
    lcd_data_write(0x55);    //16-bits
    lcd_command_write(0x11); // Exit sleep mode
    lcd_command_write(0x29); // Display on 

    delay(10);
}

/*!
    \brief      insert a delay time
    \param[in]  count: delay time
    \param[out] none
    \retval     none
*/
static void delay(__IO uint32_t count)
{
    __IO uint32_t index = 0; 
    for(index = count; index != 0; index--){
    }
}
