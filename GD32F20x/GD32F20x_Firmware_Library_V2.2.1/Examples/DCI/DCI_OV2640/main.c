/*!
    \file    main.c
    \brief   DCI display demo

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
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

#include "gd32f20x.h"
#include "systick.h"
#include <stdio.h>
#include "dci_ov2640.h"

static void lcd_gpio_config(void);
static void lcd_config(void);

uint8_t capture_image[320*240*2];

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{    
    ov2640_id_struct ov2640id;  
    /* configure Systick */
    systick_config();

    dci_ov2640_init();
    dci_ov2640_id_read(&ov2640id); 
    delay_1ms(100);  
    
    dma_channel_enable(DMA1, DMA_CH5);
    dci_enable();  
    dci_capture_enable();
    delay_1ms(100);
    
    lcd_config();
    tli_layer_enable(LAYER0);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();

    while(1){
    }
}

/*!
    \brief      LCD configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_config(void)
{
    tli_parameter_struct               tli_init_struct;
    tli_layer_parameter_struct         tli_layer_initstruct;

    rcu_periph_clock_enable(RCU_TLI);

    lcd_gpio_config();  
  
    /* configure the TLI clock to generate lcd clock */
    rcu_pllt_config(RCU_PLLTSRC_HXTAL);
    if(ERROR == rcu_pllt_vco_config(25, 192, 3)){
        while(1);
    }
    rcu_tli_clock_config(RCU_CKTLI_CKPLLTR_DIV4);

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
    tli_init_struct.synpsz_hpsz = 40;
    tli_init_struct.synpsz_vpsz = 9;
    tli_init_struct.backpsz_hbpsz = 42; 
    tli_init_struct.backpsz_vbpsz = 11;  
    tli_init_struct.activesz_hasz = 42 + 480;
    tli_init_struct.activesz_vasz = 11 + 272;
    tli_init_struct.totalsz_htsz = 42 + 480 + 2; 
    tli_init_struct.totalsz_vtsz = 11 + 272 + 2;
    /* LCD background color configure */
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF; 
    tli_init(&tli_init_struct);

    /* windowing configuration */ 
    tli_layer_initstruct.layer_window_leftpos = 120;
    tli_layer_initstruct.layer_window_rightpos = (120 + 320 - 1); 
    tli_layer_initstruct.layer_window_toppos =  28;
    tli_layer_initstruct.layer_window_bottompos = (240 + 28- 1);

    /* pixel format configuration */           
    tli_layer_initstruct.layer_ppf = LAYER_PPF_RGB565;

    /* alpha constant configuration : the constant alpha for layer 1 is decreased 
    to see the layer 0 in the intersection zone*/
    tli_layer_initstruct.layer_sa = 255;
  
    /* default color configuration (configure A,R,G,B component values) */           
    tli_layer_initstruct.layer_default_blue = 0x0;        
    tli_layer_initstruct.layer_default_green = 0x0;       
    tli_layer_initstruct.layer_default_red = 0x0;         
    tli_layer_initstruct.layer_default_alpha = 0x0;

    /* blending factors */    
    tli_layer_initstruct.layer_acf1 = LAYER_ACF1_PASA;    
    tli_layer_initstruct.layer_acf2 = LAYER_ACF2_PASA;

    tli_layer_initstruct.layer_frame_bufaddr = (uint32_t)capture_image;

    tli_layer_initstruct.layer_frame_line_length = ((320 * 2) + 3); 
    tli_layer_initstruct.layer_frame_buf_stride_offset = (320 * 2);

    tli_layer_initstruct.layer_frame_total_line_number = 240; 

    tli_layer_init(LAYER0, &tli_layer_initstruct);
}

/*!
    \brief      LCD GPIO configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_gpio_config(void)
{
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
    
    /* TLI GPIO pin remap configuration */
    gpio_pin_remap1_config(GPIO_PCF3, GPIO_PCF3_TLI_B3_PG11_REMAP | GPIO_PCF3_TLI_B2_PG10_REMAP | 
                                    GPIO_PCF3_TLI_CLK_PG7_REMAP | GPIO_PCF3_TLI_R7_PG6_REMAP |
                                    GPIO_PCF3_TLI_DE_PF10_REMAP | GPIO_PCF3_TLI_B0_PE4_REMAP | 
                                    GPIO_PCF3_TLI_G0_PE5_REMAP | GPIO_PCF3_TLI_G1_PE6_REMAP |
                                    GPIO_PCF3_TLI_R2_PC10_REMAP, ENABLE);
    
    gpio_pin_remap1_config(GPIO_PCF4, GPIO_PCF4_TLI_HSYNC_PI10_REMAP | GPIO_PCF4_TLI_VSYNC_PI9_REMAP |
                                    GPIO_PCF4_TLI_B7_PI7_REMAP | GPIO_PCF4_TLI_B6_PI6_REMAP |
                                    GPIO_PCF4_TLI_B5_PI5_REMAP | GPIO_PCF4_TLI_B4_PI4_REMAP |
                                    GPIO_PCF4_TLI_G7_PI2_REMAP | GPIO_PCF4_TLI_R1_PI3_REMAP |
                                    GPIO_PCF4_TLI_G6_PI1_REMAP | GPIO_PCF4_TLI_G5_PI0_REMAP |
                                    GPIO_PCF4_TLI_G4_PH15_REMAP | GPIO_PCF4_TLI_G3_PH14_REMAP |
                                    GPIO_PCF4_TLI_G2_PH13_REMAP | GPIO_PCF4_TLI_R6_PH12_REMAP |
                                    GPIO_PCF4_TLI_R5_PH11_REMAP | GPIO_PCF4_TLI_R4_PH10_REMAP |
                                    GPIO_PCF4_TLI_R3_PH9_REMAP | GPIO_PCF4_TLI_R0_PH4_REMAP |
                                    GPIO_PCF4_TLI_B1_PG12_REMAP, ENABLE);

    /* configure HSYNC(PI10), VSYNC(PI9), PCLK(PG7) */
    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PH10), LCD_R3(PH9),LCD_R2(PC10), 
                 LCD_R1(PI3), LCD_R0(PH4), LCD_G7(PI2), LCD_G6(PI1), LCD_G5(PI0), LCD_G4(PH15), 
                 LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PE6), LCD_G0(PE5),LCD_B7(PI7), LCD_B6(PI6), 
                 LCD_B5(PI5), LCD_B4(PI4), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PE4),
                 LCD_CLK(PG7), LCD_HSYNC(PI10), LCD_VSYNC(PI9), LCD_DE(PF10) */    
    gpio_init(GPIOE,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); 
    
    gpio_init(GPIOH,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_4 | GPIO_PIN_9 | 
                                                    GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                                                    GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    
    gpio_init(GPIOI,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                                                    GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
                                                    GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10);  
    
    gpio_init(GPIOG,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 |
                                                    GPIO_PIN_11 | GPIO_PIN_12);     

    gpio_init(GPIOF,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_10);  
    
    gpio_init(GPIOC,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_10);
    /* back light */
    gpio_init(GPIOH, GPIO_MODE_OUT_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_6);
    gpio_bit_set(GPIOH,GPIO_PIN_6);
}
