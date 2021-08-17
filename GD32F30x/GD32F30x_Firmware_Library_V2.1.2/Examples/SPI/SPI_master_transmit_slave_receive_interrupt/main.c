/*!
    \file    main.c
    \brief   master send and slave receive data use interrupt mode 

    \version 2017-02-10, V1.0.0, firmware for GD32F30x
    \version 2018-10-10, V1.1.0, firmware for GD32F30x
    \version 2018-12-25, V2.0.0, firmware for GD32F30x
    \version 2020-09-30, V2.1.0, firmware for GD32F30x
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

#include "gd32f30x.h"
#include "gd32f30x_it.h"
#include "gd32f307c_eval.h"

#define arraysize                      10
#define SET_SPI0_NSS_HIGH              gpio_bit_set(GPIOA,GPIO_PIN_3);
#define SET_SPI0_NSS_LOW               gpio_bit_reset(GPIOA,GPIO_PIN_3);
__IO uint32_t send_n = 0, receive_n = 0;
uint8_t spi0_send_array[arraysize] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
uint8_t spi2_receive_array[arraysize];
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
    /* init led2 */
    gd_eval_led_init(LED2);

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

    SET_SPI0_NSS_HIGH

    /* SPI int enable */
    spi_i2s_interrupt_enable(SPI0, SPI_I2S_INT_TBE);
    spi_i2s_interrupt_enable(SPI2, SPI_I2S_INT_RBNE);

    SET_SPI0_NSS_LOW

    /* SPI enable */
    spi_enable(SPI2);
    spi_enable(SPI0);

    /* wait transmit complete */
    while(receive_n < arraysize);

    SET_SPI0_NSS_HIGH

    /* compare receive data with send data */
    if(memory_compare(spi2_receive_array, spi0_send_array, arraysize))
        gd_eval_led_on(LED2);
    else
        gd_eval_led_off(LED2);

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
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI2);
    rcu_periph_clock_enable(RCU_AF);
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
    /* config PA3 as SPI0_NSS */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    gpio_pin_remap_config(GPIO_SPI2_REMAP,ENABLE);
    /* SPI2 GPIO config: NSS/PA4, SCK/PC10, MISO/PC11 */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10 |GPIO_PIN_11);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
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

    /* SPI2 parameter config */
    spi_init_struct.trans_mode  = SPI_TRANSMODE_BDRECEIVE;
    spi_init_struct.device_mode = SPI_SLAVE;
    spi_init_struct.nss         = SPI_NSS_HARD;
    spi_init(SPI2, &spi_init_struct);
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
