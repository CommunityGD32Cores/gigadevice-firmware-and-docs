/*!
    \file  main.c
    \brief TLI color key 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "gd32f207i_eval.h"
#include <stdio.h>
#include "image_RGB565.h"

static void tli_gpio_config(void);
static void tli_config(void);

/*!
    \brief      main program
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    tli_config();
    /* layer0 enable */
    tli_layer_enable(LAYER0);
    /* reload layer0 configuration */
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    while(1){
        /* wait for TAMPER button is pressed */
        while(gd_eval_key_state_get(KEY_TAMPER) != RESET);
        /* wait for TAMPER button is released */
        while(gd_eval_key_state_get(KEY_TAMPER) != SET);
        tli_color_key_enable(LAYER0);
        tli_ckey_init(LAYER0,0xFF,0xFF,0xFF);
        /* reload layer0 configuration */
        tli_reload_config(TLI_REQUEST_RELOAD_EN);
        /* wait for TAMPER button is pressed */
        while(gd_eval_key_state_get(KEY_TAMPER) != RESET);
        /* wait for TAMPER button is released */
        while (gd_eval_key_state_get(KEY_TAMPER) != SET);
        tli_color_key_disable(LAYER0);
        /* reload layer0 configuration */
        tli_reload_config(TLI_REQUEST_RELOAD_EN);
    }
}

/*!
    \brief      LCD configuration 
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_config(void)
{
    tli_parameter_struct               tli_init_struct;
    tli_layer_parameter_struct         tli_layer_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    tli_gpio_config();  
    /* configure the TLI clock to generate LCD clock */
    rcu_pllt_config(RCU_PLLTSRC_HXTAL);
    if(ERROR == rcu_pllt_vco_config(25, 192, 3)){
        while(1);
    }
    rcu_tli_clock_config(RCU_CKTLI_CKPLLTR_DIV8);

    rcu_osci_on(RCU_PLLT_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLT_CK)){
        while(1);
    }

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
  
    /* tli layer0 configuration */
    /* tli window size configuration */
    tli_layer_init_struct.layer_window_leftpos = 43;
    tli_layer_init_struct.layer_window_rightpos = (480 + 43 - 1); 
    tli_layer_init_struct.layer_window_toppos = 12;
    tli_layer_init_struct.layer_window_bottompos = (272 + 12 - 1);
    /* tli window pixel format configuration */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    /* tli window specified alpha configuration */
    tli_layer_init_struct.layer_sa = 255;
    /* tli layer default alpha R,G,B value configuration */
    tli_layer_init_struct.layer_default_blue = 0xFF;        
    tli_layer_init_struct.layer_default_green = 0xFF;       
    tli_layer_init_struct.layer_default_red = 0xFF;         
    tli_layer_init_struct.layer_default_alpha = 0xFF;
    /* tli window blend configuration */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;    
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_SA;
    /* tli layer frame buffer base address configuration */
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&image_rgb565;
    tli_layer_init_struct.layer_frame_line_length = ((480 * 2) + 3); 
    tli_layer_init_struct.layer_frame_buf_stride_offset = (480 * 2);
    tli_layer_init_struct.layer_frame_total_line_number = 272; 
    tli_layer_init(LAYER0, &tli_layer_init_struct);
    tli_dither_config(TLI_DITHER_ENABLE);
}

/*!
    \brief      TLI GPIO configure 
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tli_gpio_config(void)
{
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
    rcu_periph_clock_enable(RCU_AF);
    
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
    gpio_init(GPIOE,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); 
    
    gpio_init(GPIOH,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4 | GPIO_PIN_8 |GPIO_PIN_9 | 
                                                    GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12|
                                                    GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    
    gpio_init(GPIOI,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                                                    GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
                                                    GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10);  
    
    gpio_init(GPIOG,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 |
                                                    GPIO_PIN_11 | GPIO_PIN_12);     

    gpio_init(GPIOF,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);  
    
    /* back light */
    gpio_init(GPIOH, GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_bit_set(GPIOH,GPIO_PIN_6);
}
