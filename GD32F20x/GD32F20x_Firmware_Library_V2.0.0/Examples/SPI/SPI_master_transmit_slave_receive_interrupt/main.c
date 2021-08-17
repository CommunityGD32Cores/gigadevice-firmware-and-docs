/*!
    \file  main.c
    \brief master send and slave receive data use interrupt mode 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "gd32f20x_it.h"
#include "gd32f207i_eval.h"

#define  ARRAYSIZE         10
uint32_t send_n = 0, receive_n = 0;
uint8_t spi0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
uint8_t spi2_receive_array[ARRAYSIZE];
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length);

void rcu_config(void);
void gpio_config(void);
void spi_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init led1 */
    gd_eval_led_init(LED1);

    /* NVIC config */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(SPI0_IRQn,1,1);
    nvic_irq_enable(SPI2_IRQn,0,1);

    /* peripheral clock enable */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* SPI config */
    spi_config();

    /* SPI enable */
    spi_enable(SPI2);
    spi_enable(SPI0);

    /* wait transmit complete */
    while(receive_n < ARRAYSIZE);

    /* compare receive data with send data */
    if(memory_compare(spi2_receive_array, spi0_send_array, ARRAYSIZE)){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_off(LED1);
    }

    while(1);
   
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI2);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* SPI0 GPIO config:SCK/PA5, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    
    /* SPI2 GPIO config:SCK/PC10, MISO/PC11 */
    gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10 |GPIO_PIN_11);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct  spi_init_struct;

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* SPI1 parameter config */
    spi_init_struct.trans_mode  = SPI_TRANSMODE_BDRECEIVE;
    spi_init_struct.device_mode = SPI_SLAVE;
    spi_init_struct.nss         = SPI_NSS_SOFT;
    spi_init(SPI2, &spi_init_struct);
    
    spi_nss_internal_high(SPI0);
    spi_nss_internal_low(SPI2);
    
    /* SPI int enable */
    spi_i2s_interrupt_enable(SPI0, SPI_I2S_INT_TBE);
    spi_i2s_interrupt_enable(SPI2, SPI_I2S_INT_RBNE);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length) 
{
    while (length--){
        if (*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}
