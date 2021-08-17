/*!
    \file  main.c
    \brief ADC test demo

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
#include "Lcd_Driver.h"
#include "GUI.h"
#include "systick.h"
#include "gd32f150r_eval.h"

__IO uint16_t ad_value[220];

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void timer_config(void);
void adc_config(void);
void flash_led(int times);
void test_display_init(void);
void diaplay_adc_value(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init test status led */ 
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* SYSTICK configuration */
    systick_config();
    /* flash led for test */
    flash_led(3);

    /* RCU configuration */
    rcu_config();
    /* GPIO configuration */
    gpio_config();
    /* DMA configuration */
    dma_config();
    /* TIMER configuration */
    timer_config();
    /* ADC configuration */
    adc_config();
    /* TFT LCD initialize */
    lcd_init();
    lcd_clear(BLUE);
    test_display_init();

    /* infinite loop */
    while(1){
        /* TIMER1 counter enable */
        timer_enable(TIMER1);

        /* test on channel1 transfer complete flag */
        while( !dma_flag_get(DMA_CH0,DMA_FLAG_FTF ));

        /* clear channel1 transfer complete flag */
        dma_flag_clear(DMA_CH0,DMA_FLAG_FTF ); 

        /* TIMER1 counter disable */
        timer_disable(TIMER1);

        diaplay_adc_value();
    }
  
}

/*!
    \brief      RCU configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* ADCCLK = PCLK2/6 */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    /* enable dma1 and gpioc clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* enable adc1 clock */
    rcu_periph_clock_enable(RCU_ADC);

    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
}

/*!
    \brief      GPIO configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PC1 (adc channel11) as analog input */
    gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_1);
}

/*!
    \brief      DMA configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA channel0 */
    dma_deinit(DMA_CH0);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)ad_value;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.number = 220;
    dma_init_struct.periph_addr = (uint32_t)&(ADC_RDATA);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0,&dma_init_struct);
    
    /* configure DMA mode */
    dma_circulation_enable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);
    
    /* enable DMA channel0 */
    dma_channel_enable(DMA_CH0);
}

/*!
    \brief      TIMER configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 5;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 199;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH1 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,100);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
}

/*!
    \brief      ADC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL,1);

    /* ADC regular channel config */
    adc_regular_channel_config(0,ADC_CHANNEL_11,ADC_SAMPLETIME_55POINT5);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);
    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_T1_CH1);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* enable ADC interface */
    adc_enable();
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
    /* ADC SCAN function enable */
    adc_special_function_config(ADC_SCAN_MODE,ENABLE);
    /* ADC DMA function enable */
    adc_dma_mode_enable();
}

/*!
    \brief      test status led
    \param[in]  times: the flash times of leds
    \param[out] none
    \retval     none
*/
void flash_led(int times)
{
    int i;

    for(i = 0;i < times;i ++){
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on led */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off led */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}

/*!
    \brief      LCD content initialize
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_display_init( void )
{
    int i;

    #define x_offset 2
    {
        gui_draw_font_gbk16(x_offset,10,WHITE,BLUE," GigaDevice Semiconductor Inc.");
        gui_draw_font_gbk16(x_offset,30,WHITE,BLUE,"  -- GD32F1x0 Series MCU --   ");

        gui_draw_font_gbk16(x_offset,50,WHITE,BLUE,"     GD32F150R_EAVL_V1.3  ");

        gui_draw_font_gbk16(x_offset,70,WHITE,BLUE," ADC Test :");
        gui_draw_font_gbk16(x_offset,90,WHITE,BLUE,"    Please adjust the POT !");

        gui_draw_line(x_offset + 6,110,x_offset + 6,310,YELLOW);
        gui_draw_line(x_offset + 6,310,x_offset + 6 + 220,310,YELLOW);

        for(i = 0;i < 4;i ++){
            gui_draw_line(x_offset + 6 - 4,110 + i * 50,x_offset + 6,110 + i * 50,YELLOW);
        }

        for(i = 0;i < 4;i ++){
            gui_draw_line(x_offset + 6 - 2,110 + 25 + i * 50,x_offset + 6,110 + 25 + i * 50,YELLOW);
        }
    }
}

/*!
    \brief      display ADC value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void diaplay_adc_value(void)
{
    int ix,iy,i;
    int deta = 200 /4 * 3.3;

    LCD_CS_CLR;

    for(ix = x_offset + 6 + 1,i = 0;ix < x_offset + 6 +220;ix++,i++){
        for(iy = 110;iy < 310-1;iy++){
            if(iy == (310 - ad_value[ i ] * deta / 0x0FFF)){
                /* set the pixel */
                gui_draw_point(ix,iy,YELLOW);
            }else{
                /* set the pixel */
                gui_draw_point(ix,iy,BLUE);
            }
        }
    }

    LCD_CS_SET;
}
