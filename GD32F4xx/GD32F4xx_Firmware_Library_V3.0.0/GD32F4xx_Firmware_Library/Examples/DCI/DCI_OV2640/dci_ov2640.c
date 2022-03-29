/*!
    \file    dci_ov2640.c
    \brief   DCI config file
    
    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
    \version 2020-09-30, V2.1.0, firmware for GD32F4xx
    \version 2022-03-09, V3.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "dci_ov2640.h"
#include "dci_ov2640_init_table.h"
#include "gd32f4xx_dci.h"
#include "systick.h"

extern uint8_t capture_image[320*240*2];
/*!
    \brief      configure the DCI to interface with the camera module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_config(void)
{
    dci_parameter_struct dci_struct;
    dma_single_data_parameter_struct dma_single_struct;
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_DCI);
    
    /* DCI GPIO AF configuration */ 
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_4);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_7);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_8);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_9);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_11);
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_9);
    gpio_af_set(GPIOD, GPIO_AF_13, GPIO_PIN_3);
    gpio_af_set(GPIOG, GPIO_AF_13, GPIO_PIN_9);

    /* configure DCI_PIXCLK(PA6), DCI_VSYNC(PG9), DCI_HSYNC(PA4), DCI_D0(PC6), DCI_D1(PC7)
                 DCI_D2(PC8), DCI_D3(PC9), DCI_D4(PC11), DCI_D5(PD3), DCI_D6(PB8), DCI_D7(PB9) */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_4);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_6);
    
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_6);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_7);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_8);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_9);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_11);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_9);
    
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_3);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_9);
    
    /* DCI configuration */ 
    dci_struct.capture_mode=DCI_CAPTURE_MODE_CONTINUOUS;
    dci_struct.clock_polarity=  DCI_CK_POLARITY_RISING;
    dci_struct.hsync_polarity= DCI_HSYNC_POLARITY_LOW;
    dci_struct.vsync_polarity= DCI_VSYNC_POLARITY_LOW;
    dci_struct.frame_rate= DCI_FRAME_RATE_ALL;
    dci_struct.interface_format= DCI_INTERFACE_FORMAT_8BITS;
    dci_init(&dci_struct);

    /* DCI DMA configuration */ 
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1,DMA_CH7);
    dma_single_struct.periph_addr= (uint32_t)DCI_DR_ADDRESS;
    dma_single_struct.memory0_addr= (uint32_t)(&capture_image);
    dma_single_struct.direction= DMA_PERIPH_TO_MEMORY;
    dma_single_struct.number= 38400;
    dma_single_struct.periph_inc= DMA_PERIPH_INCREASE_DISABLE;
    dma_single_struct.memory_inc= DMA_MEMORY_INCREASE_ENABLE;
    dma_single_struct.periph_memory_width= DMA_PERIPH_WIDTH_32BIT;
    dma_single_struct.priority= DMA_PRIORITY_HIGH;
    dma_single_struct.circular_mode= DMA_CIRCULAR_MODE_ENABLE;
    dma_single_data_mode_init(DMA1,DMA_CH7,&dma_single_struct);

    dma_channel_subperipheral_select(DMA1,DMA_CH7,DMA_SUBPERI1);
}

/*!
    \brief      DCI camera outsize set
    \param[in]  width: outsize width
    \param[in]  height: outsize height
    \param[out] none
    \retval     0x00 or 0xFF
*/
uint8_t ov2640_outsize_set(uint16_t width,uint16_t height)
{
    uint16_t outh;
    uint16_t outw;
    uint8_t temp; 
    if(width%4)return 0xFF;
    if(height%4)return 0xFF;
    outw=width/4;
    outh=height/4; 
    dci_byte_write(0xFF,0x00);
    dci_byte_write(0xE0,0x04);
    dci_byte_write(0x5A,outw&0xFF);
    dci_byte_write(0x5B,outh&0xFF);
    temp=(outw>>8)&0x03;
    temp|=(outh>>6)&0x04;
    dci_byte_write(0x5C,temp);
    dci_byte_write(0xE0,0x00);
    return 0;
}

/*!
    \brief      DCI camera initialization
    \param[in]  none
    \param[out] none
    \retval     0x00 or 0xFF
*/
uint8_t dci_ov2640_init(void)
{
    uint8_t i;
    sccb_config();
    dci_config();
    
    ckout0_init();
    delay_1ms(10);
    /* OV2640 reset */
    if(dci_byte_write(0xFF, 0x01)!=0)
        return 0xFF;
    if(dci_byte_write(0x12, 0x80)!=0)
        return 0xFF;
    delay_1ms(10);
    for(i=0;i<sizeof(ov2640_svga_init_reg_tbl)/2;i++){
        if(0 != dci_byte_write(ov2640_svga_init_reg_tbl[i][0],ov2640_svga_init_reg_tbl[i][1])){
            return 0xFF;
        }
    }
    
    delay_1ms(100);
    for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++){
        if(0 != dci_byte_write(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1])){
            return 0xFF;
        }
    } 
    delay_1ms(100);
    ov2640_outsize_set(320,240);
    return 0;
}

/*!
    \brief      ckout0 initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ckout0_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_8);
    
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV2);
}

/*!
    \brief      read the ov2640 manufacturer identifier
    \param[in]  ov2640id: pointer to the ov2640 manufacturer struct
    \param[out] none
    \retval     0x00 or 0xFF
*/
uint8_t dci_ov2640_id_read(ov2640_id_struct* ov2640id)
{
    uint8_t temp;
    dci_byte_write(0xFF, 0x01);
    if(dci_byte_read(OV2640_MIDH,&temp)!=0)
        return 0xFF;
    ov2640id->manufacturer_id1 = temp;
    if(dci_byte_read(OV2640_MIDL,&temp)!=0)
        return 0xFF;
    ov2640id->manufacturer_id2 = temp;
    if(dci_byte_read(OV2640_VER,&temp)!=0)
        return 0xFF;
    ov2640id->version = temp;
    if(dci_byte_read(OV2640_PID,&temp)!=0)
        return 0xFF;
    ov2640id->pid = temp;

    return 0x00;
}

