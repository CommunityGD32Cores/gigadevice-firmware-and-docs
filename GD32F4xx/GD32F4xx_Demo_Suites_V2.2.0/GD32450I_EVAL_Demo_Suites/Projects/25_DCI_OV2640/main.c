/*!
    \file  main.c
    \brief DCI display demo
    
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
#include "systick.h"
#include <stdio.h>
#include "dci_ov2640.h"
#include "picture.h"
#include "exmc_sdram.h"
#include "main.h"

static void lcd_gpio_config(void);
static void nvic_configuration(void);

tli_parameter_struct               tli_initstruct;
tli_layer_parameter_struct         tli_layer0_initstruct;
tli_layer_parameter_struct         tli_layer1_initstruct;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{    
    ov2640_id_struct ov2640id;
    systick_config();
    nvic_configuration();
    
    /* init SDRAM*/
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    delay_1ms(1000);
    /* key configuration */
    key_config();
  
    /* camera initialization */
    dci_ov2640_init();
    dci_ov2640_id_read(&ov2640id);
    
    /* DMA interrupt and channel enable */
    dma_interrupt_enable(DMA1, DMA_CH7,DMA_CHXCTL_FTFIE);
    dma_channel_enable(DMA1,DMA_CH7);
  
    /* DCI enable */
    dci_enable();  
    dci_capture_enable();
    delay_1ms(100);
  
    /* LCD configure and TLI enable */
    lcd_config();
    tli_layer_enable(LAYER0);
    tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();

    while (1)
    {
    }
}

/*!
    \brief      key configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* configure wakeup key interrupt*/
    {
        /* configure button pin as input */
        gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_0);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOA,EXTI_SOURCE_PIN0);

        /* configure key EXTI line0 */
        exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_0);

        /* enable and set key EXTI interrupt priority */
        nvic_irq_enable(EXTI0_IRQn, 1U, 1U);
    }
    
    /* configure tamper key interrupt*/
    {
        /* configure PC13 pin */
        gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_13);

        /* connect EXTI line13 to PC13 pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN13);

        /* configure key EXTI line13 */
        exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_13);
    }
    /* configure user key interrupt*/
    {
        /* configure PB14 pin */
        gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_14);

        /* connect EXTI line14 to PB14 pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN14);

        /* configure EXTI Line14 */
        exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_14);
               
        nvic_irq_enable(EXTI10_15_IRQn, 1U, 0U);
    }
}

/*!
    \brief      save image to sdram
    \param[in]  none
    \param[out] none
    \retval     none
*/
void image_save()
{
    uint32_t i=0;

    dma_interrupt_disable(DMA1, DMA_CH7,DMA_CHXCTL_FTFIE);
    dma_channel_disable(DMA1, DMA_CH7);
    dci_capture_disable();

    /* save image to sdram */       
    for(i=0;i<32640;i++)
    {
        *(uint32_t *)(0XC0800000+4*i)=*(uint32_t *)(0XC0400000+4*i);
    }
}

/*!
    \brief      display image to lcd
    \param[in]  none
    \param[out] none
    \retval     none
*/
void image_display(uint32_t diapaly_image_addr)
{  
    /* input address configuration */    
    tli_layer1_initstruct.layer_frame_bufaddr =(uint32_t)diapaly_image_addr;
      
    tli_layer_init(LAYER1, &tli_layer1_initstruct);
    /* enable layer0 */
    tli_layer_enable(LAYER0);
    /* enable layer1 */
    tli_layer_enable(LAYER1);
    /* reload configuration */
    tli_reload_config(TLI_REQUEST_RELOAD_EN);

    /* enable TLI */
    tli_enable();
}
/*!
    \brief      LCD configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_config(void)
{
    rcu_periph_clock_enable(RCU_TLI);

    lcd_gpio_config();  
  
    /* enable pixel clock */
    /* configure PLLSAI prescalers for LCD */
    if(ERROR == rcu_pllsai_config(192,2,3))
    {
        while(1);
    }
    rcu_tli_clock_div_config(RCU_PLLSAIR_DIV8);
    rcu_osci_on(RCU_PLLSAI_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK))
    {
        while(1);
    }

    /* TLI initialization */

    /* initialize the horizontal synchronization polarity as active low*/
    tli_initstruct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;     
    /* initialize the vertical synchronization polarity as active low */  
    tli_initstruct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;     
    /* initialize the data enable polarity as active low */ 
    tli_initstruct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;     
    /* initialize the pixel clock polarity as input pixel clock */ 
    tli_initstruct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
  
    /* timing configuration */
    /* configure horizontal synchronization width */
    tli_initstruct.synpsz_hpsz = 40;
    /* configure vertical synchronization height */
    tli_initstruct.synpsz_vpsz = 9;
    /* configure accumulated horizontal back porch */
    tli_initstruct.backpsz_hbpsz = 42; 
    /* configure accumulated vertical back porch */
    tli_initstruct.backpsz_vbpsz = 11;  
    /* configure accumulated active width */  
    tli_initstruct.activesz_hasz = 522;
    /* configure accumulated active height */
    tli_initstruct.activesz_vasz = 283;
    /* configure total width */
    tli_initstruct.totalsz_htsz = 524; 
    /* configure total height */
    tli_initstruct.totalsz_vtsz = 285;
  
    /* configure R,G,B component values for LCD background color */
    tli_initstruct.backcolor_red = 0XFF;
    tli_initstruct.backcolor_green = 0XFF;
    tli_initstruct.backcolor_blue = 0XFF; 

    tli_init(&tli_initstruct);

    /* layer0 windowing configuration */ 
    tli_layer0_initstruct.layer_window_leftpos = 43;
    tli_layer0_initstruct.layer_window_rightpos = (43+480 - 1); 
    tli_layer0_initstruct.layer_window_toppos =  12;
    tli_layer0_initstruct.layer_window_bottompos = (12 +272- 1);

    /* pixel format configuration */           
    tli_layer0_initstruct.layer_ppf = LAYER_PPF_RGB565;

    /* alpha constant configuration : the constant alpha for layer 0 is decreased 
    to see the layer 0 in the intersection zone*/
    tli_layer0_initstruct.layer_sa = 255;
  
    /* default color configuration (configure A,R,G,B component values) */           
    tli_layer0_initstruct.layer_default_blue = 0xFF;        
    tli_layer0_initstruct.layer_default_green = 0xFF;       
    tli_layer0_initstruct.layer_default_red = 0xFF;         
    tli_layer0_initstruct.layer_default_alpha = 0XFF;

    /* blending factors */    
    tli_layer0_initstruct.layer_acf1 = LAYER_ACF1_SA;    
    tli_layer0_initstruct.layer_acf2 = LAYER_ACF1_SA;

    /* configure input address : frame buffer is located at FLASH memory */    
    tli_layer0_initstruct.layer_frame_bufaddr = (uint32_t)image_background0;

    tli_layer0_initstruct.layer_frame_line_length = ((480 * 2) + 3); 
    tli_layer0_initstruct.layer_frame_buf_stride_offset = (480 * 2);

    tli_layer0_initstruct.layer_frame_total_line_number = 272; 

    tli_layer_init(LAYER0, &tli_layer0_initstruct);
    
    /* layer1 windowing configuration */ 
    tli_layer1_initstruct.layer_window_leftpos = 160;
    tli_layer1_initstruct.layer_window_rightpos = (160+240 - 1); 
    tli_layer1_initstruct.layer_window_toppos =  12;
    tli_layer1_initstruct.layer_window_bottompos = (12+272- 1);

    /* pixel format configuration */           
    tli_layer1_initstruct.layer_ppf = LAYER_PPF_RGB565;

    /* alpha constant configuration : the constant alpha for layer 1 is decreased 
    to see the layer 0 in the intersection zone*/
    tli_layer1_initstruct.layer_sa = 255;
  
    /* default color configuration (configure A,R,G,B component values) */           
    tli_layer1_initstruct.layer_default_blue = 0xFF;        
    tli_layer1_initstruct.layer_default_green = 0xFF;       
    tli_layer1_initstruct.layer_default_red = 0xFF;         
    tli_layer1_initstruct.layer_default_alpha = 0;

    /* blending factors */    
    tli_layer1_initstruct.layer_acf1 = LAYER_ACF1_SA;    
    tli_layer1_initstruct.layer_acf2 = LAYER_ACF1_SA;

    /* configure input address : frame buffer is located at memory */    
    tli_layer1_initstruct.layer_frame_bufaddr = (uint32_t)0xC0400000;

    tli_layer1_initstruct.layer_frame_line_length = ((240 * 2) + 3); 
    tli_layer1_initstruct.layer_frame_buf_stride_offset = (240 * 2);

    tli_layer1_initstruct.layer_frame_total_line_number = 272; 

    tli_layer_init(LAYER1, &tli_layer1_initstruct);
    
    tli_dither_config(TLI_DITHER_ENABLE);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    /* enable DMA1 channel 7 */
    nvic_irq_enable(DMA1_Channel7_IRQn, 0U, 0U);
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
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* TLI pins AF configure */
    gpio_af_set(GPIOE,GPIO_AF_14,GPIO_PIN_5); 
    gpio_af_set(GPIOE,GPIO_AF_14,GPIO_PIN_6);  
    gpio_af_set(GPIOE,GPIO_AF_14,GPIO_PIN_4);
    
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_2);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_3);
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_8);
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_9);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_10); 
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_11);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_12); 
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_13);  
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_14);     
    gpio_af_set(GPIOH,GPIO_AF_14,GPIO_PIN_15); 
    
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_0);  
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_1); 
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_2);     
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_4); 
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_5);  
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_6);     
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_9);     
    gpio_af_set(GPIOI,GPIO_AF_14,GPIO_PIN_10);
    
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_6);     
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_10);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_11);     
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_12);    

    /* configure HSYNC(PI10), VSYNC(PI9), PCLK(PG7) */
    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PH10), LCD_R3(PH9),LCD_R2(PH8), 
                 LCD_R1(PH3), LCD_R0(PH2), LCD_G7(PI2), LCD_G6(PI1), LCD_G5(PI0), LCD_G4(PH15), 
                 LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PE6), LCD_G0(PE5),LCD_B7(PI7), LCD_B6(PI6), 
                 LCD_B5(PI5), LCD_B4(PI4), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PE4) */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                 |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                           |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2
                 |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2
                           |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10);
    
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);

    /* configure LCD_PWM_BackLight(PB15) */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    
    gpio_bit_set(GPIOB,GPIO_PIN_15);

}
