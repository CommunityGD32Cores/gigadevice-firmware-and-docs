/*!
    \file  main.c
    \brief TLI_IPA display demo

    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx.h"
#include "gd32f450z_eval.h"
#include "gd32f450z_lcd_eval.h"
#include "image1.h"
#include "image2.h"
#include "image3.h"
#include "image4.h"
#include "image5.h"
#include "image6.h"
#include "image7.h"
#include "image8.h"
#include "image9.h"
#include "image10.h"
#include "image11.h"
#include "logo.h"

#define HORIZONTAL_SYNCHRONOUS_PULSE  10
#define HORIZONTAL_BACK_PORCH         20
#define ACTIVE_WIDTH                  320
#define HORIZONTAL_FRONT_PORCH        40

#define VERTICAL_SYNCHRONOUS_PULSE    2
#define VERTICAL_BACK_PORCH           2
#define ACTIVE_HEIGHT                 480
#define VERTICAL_FRONT_PORCH          4

/* choose one of them based on the version of LCD */
#define LCD_VERSION_1_1               0            /* LCD V1.1 or earlier */
#define LCD_VERSION_1_2               1            /* LCD V1.2 */

uint8_t blended_address_buffer[87680];
uint32_t timecount = 0;

static void ipa_config(uint32_t baseaddress);
static void tli_config(void);
static void tli_blend_config(void);
static void tli_gpio_config(void);
static void lcd_config(void);
static void delay(uint32_t time);

/*!
    \brief      main program
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{ 
    lcd_config();
    tli_config();
    tli_layer_enable(LAYER0);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
    tli_enable();
    tli_blend_config();
    tli_reload_config(TLI_REQUEST_RELOAD_EN);

    while(1){
        /* IPA configuration and display the images one by one */
        ipa_config((uint32_t)&gImage_image1);
        ipa_transfer_enable(); 
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image2);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image3);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image4);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image5);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image6);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image7);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image8);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF); 
        ipa_config((uint32_t)&gImage_image9);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image10);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
        ipa_config((uint32_t)&gImage_image11);
        ipa_transfer_enable();
        while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));
        delay(0x0FFFFF);
    }
}

/*!
    \brief      LCD configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_config(void)
{
    /* configure the LCD control line */
    lcd_ctrl_line_config();
    lcd_disable();
    lcd_enable();
    
    /* configure the GPIO of TLI */
    tli_gpio_config();
    /* configure the LCD_SPI */
    lcd_spi_config(); 

    /* power on the LCD */
#if (LCD_VERSION_1_1 == 1)
    lcd_power_on();
#elif (LCD_VERSION_1_2 == 1)
    lcd_power_on1();
#endif
}

/*!
    \brief      configure TLI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_config(void)
{
    tli_parameter_struct               tli_init_struct;
    tli_layer_parameter_struct         tli_layer_init_struct;

    /* initialize the parameters of structure */
    tli_struct_para_init(&tli_init_struct);
    tli_layer_struct_para_init(&tli_layer_init_struct);
    
    rcu_periph_clock_enable(RCU_TLI);
    tli_gpio_config();

    /* configure the PLLSAI clock to generate lcd clock */
    if(ERROR == rcu_pllsai_config(128, 2, 3)){
        while(1);
    }
    rcu_tli_clock_div_config(RCU_PLLSAIR_DIV4);
    rcu_osci_on(RCU_PLLSAI_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK)){
        while(1);
    }
    
    /* TLI initialization */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    
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

    /* TLI layer0 configuration */
    tli_layer_init_struct.layer_window_leftpos = (0 + 147 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
    tli_layer_init_struct.layer_window_rightpos = (162 + 147 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = (0 + 90 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
    tli_layer_init_struct.layer_window_bottompos = (320 + 90 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    tli_layer_init_struct.layer_sa = 0xFF;
    tli_layer_init_struct.layer_default_blue = 0xFF;
    tli_layer_init_struct.layer_default_green = 0xFF;
    tli_layer_init_struct.layer_default_red = 0xFF;
    tli_layer_init_struct.layer_default_alpha = 0x0;
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)gImage_logo;
    tli_layer_init_struct.layer_frame_line_length = ((162 * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (162 * 2);
    tli_layer_init_struct.layer_frame_total_line_number = 320;
    tli_layer_init(LAYER0, &tli_layer_init_struct);
}

/*!
    \brief      IPA initialize and configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void ipa_config(uint32_t baseaddress)
{
    ipa_destination_parameter_struct  ipa_destination_init_struct;
    ipa_foreground_parameter_struct   ipa_fg_init_struct;  
  
    /* initialize the parameters of structure */
    ipa_foreground_struct_para_init(&ipa_fg_init_struct);
    ipa_destination_struct_para_init(&ipa_destination_init_struct);
    
    rcu_periph_clock_enable(RCU_IPA);
   
    ipa_deinit();
    /* IPA pixel format convert mode configure */
    ipa_pixel_format_convert_mode_set(IPA_FGTODE);
    /* destination pixel format configure */
    ipa_destination_init_struct.destination_pf = IPA_DPF_RGB565;  
    /* destination memory base address configure */
    ipa_destination_init_struct.destination_memaddr = (uint32_t)&blended_address_buffer;
    /* destination pre-defined alpha value RGB configure */
    ipa_destination_init_struct.destination_pregreen = 0;
    ipa_destination_init_struct.destination_preblue = 0;
    ipa_destination_init_struct.destination_prered = 0;
    ipa_destination_init_struct.destination_prealpha = 0;
    /* destination line offset configure */
    ipa_destination_init_struct.destination_lineoff = 0;
    /* height of the image to be processed configure */
    ipa_destination_init_struct.image_height = 137;
    /* width of the image to be processed configure */
    ipa_destination_init_struct.image_width = 320;
    /* IPA destination initialization */
    ipa_destination_init(&ipa_destination_init_struct);
  
    /* IPA foreground configure */
    ipa_fg_init_struct.foreground_memaddr = baseaddress;
    ipa_fg_init_struct.foreground_pf = FOREGROUND_PPF_RGB565;
    ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;
    ipa_fg_init_struct.foreground_prealpha = 0;
    ipa_fg_init_struct.foreground_lineoff = 0;
    ipa_fg_init_struct.foreground_preblue = 0;
    ipa_fg_init_struct.foreground_pregreen = 0;
    ipa_fg_init_struct.foreground_prered = 0;
    /* foreground initialization */
    ipa_foreground_init(&ipa_fg_init_struct);
}

/*!
    \brief      configure TLI peripheral and display blend image
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_blend_config(void)
{
    tli_layer_parameter_struct         tli_layer_init_struct;
    /* initialize the parameters of structure */
    tli_layer_struct_para_init(&tli_layer_init_struct);
  
    /* TLI layer1 configuration */
    tli_layer_init_struct.layer_window_leftpos = (0 + 0 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
    tli_layer_init_struct.layer_window_rightpos = (137 + 0 + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = (0 + 100 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
    tli_layer_init_struct.layer_window_bottompos = (320 + 100 + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    tli_layer_init_struct.layer_sa = 0xFF; 
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
    tli_layer_init_struct.layer_default_alpha = 0;
    tli_layer_init_struct.layer_default_blue = 0;
    tli_layer_init_struct.layer_default_green = 0;
    tli_layer_init_struct.layer_default_red = 0;
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&blended_address_buffer;
    tli_layer_init_struct.layer_frame_line_length = ((137 * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (137 * 2);
    tli_layer_init_struct.layer_frame_total_line_number = 320;
    tli_layer_init(LAYER1, &tli_layer_init_struct);
}
/*!
    \brief      configure TLI GPIO  
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_gpio_config(void)
{
    /* enable the periphral clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure HSYNC(PC6), VSYNC(PA4), PCLK(PG7), DE(PF10) */
    /* configure LCD_R7(PG6), LCD_R6(PA8), LCD_R5(PA12), LCD_R4(PA11), LCD_R3(PB0), 
                 LCD_G7(PD3), LCD_G6(PC7), LCD_G5(PB11), LCD_G4(PB10), LCD_G3(PG10), LCD_G2(PA6),
                 LCD_B7(PB9), LCD_B6(PB8), LCD_B5(PA3), LCD_B4(PG12), LCD_B3(PG11) */
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_3);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_4);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_6);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_8);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_11);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_12);
    
    gpio_af_set(GPIOB,GPIO_AF_9,GPIO_PIN_0);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_8);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_9);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_10);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_11);
    
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_6);
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_7);
    
    gpio_af_set(GPIOD,GPIO_AF_14,GPIO_PIN_3);

    gpio_af_set(GPIOF,GPIO_AF_14,GPIO_PIN_10);
    
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_6);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOG,GPIO_AF_9,GPIO_PIN_10);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_11);
    gpio_af_set(GPIOG,GPIO_AF_9,GPIO_PIN_12);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6
                                |GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3|GPIO_PIN_4
                                |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                                |GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                                |GPIO_PIN_11);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7);

    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11
                                |GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11
                                |GPIO_PIN_12);
}

/*!
    \brief      insert a delay time
    \param[in]  time: delay time length
    \param[out] none
    \retval     none
*/
static void delay(uint32_t time)
{
    uint32_t timecount = time;
    while(0 != timecount--);
}
