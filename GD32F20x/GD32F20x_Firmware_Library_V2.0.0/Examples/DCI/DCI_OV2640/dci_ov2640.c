/*!
    \file  dci_ov2640.c
    \brief DCI config file
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "dci_ov2640.h"
#include "dci_ov2640_init_table.h"
#include "gd32f20x_dci.h"
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
    dma_parameter_struct dma_struct;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_DCI);
    rcu_periph_clock_enable(RCU_AF);
    
    /* DCI GPIO remap configuration */
    gpio_pin_remap1_config(GPIO_PCF2, GPIO_PCF2_DCI_D5_PD3_REMAP | GPIO_PCF2_DCI_D0_PC6_REMAP | 
                                    GPIO_PCF2_DCI_D1_PC7_REMAP | GPIO_PCF2_DCI_VSYNC_PG9_REMAP | 
                                    GPIO_PCF2_DCI_HSYNC_PH8_REMAP, ENABLE);

    /* configure DCI_PIXCLK(PA6), DCI_VSYNC(PG9), DCI_HSYNC(PA4), DCI_D0(PC6), DCI_D1(PC7)
                 DCI_D2(PC8), DCI_D3(PC9), DCI_D4(PC11), DCI_D5(PD3), DCI_D6(PB8), DCI_D7(PB9) */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
    
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8 | GPIO_PIN_9);
    
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11);
    
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_3);
    
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
    dma_deinit(DMA1, DMA_CH5);
    dma_struct.periph_addr= (uint32_t)DCI_DR_ADDRESS;
    dma_struct.memory_addr= (uint32_t)(&capture_image);
    dma_struct.direction= DMA_PERIPHERAL_TO_MEMORY;
    dma_struct.number= 38400;
    dma_struct.periph_inc= DMA_PERIPH_INCREASE_DISABLE;
    dma_struct.memory_inc= DMA_MEMORY_INCREASE_ENABLE;
    dma_struct.periph_width= DMA_PERIPHERAL_WIDTH_32BIT;
    dma_struct.memory_width= DMA_MEMORY_WIDTH_32BIT;
    dma_struct.priority= DMA_PRIORITY_HIGH;
    dma_init(DMA1, DMA_CH5, dma_struct);
    dma_circulation_enable(DMA1, DMA_CH5);
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
    delay_1ms(100);
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
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV2);
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
