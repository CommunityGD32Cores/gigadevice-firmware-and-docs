/*!
    \file    main.c
    \brief   master receiver

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

#include "gd32e50x.h"

#define I2C_10BIT_ADDRESS      0
#define I2C2_SLAVE_ADDRESS7    0x82
#define I2C2_SLAVE_ADDRESS10   0x0322

uint8_t i2c_receiver[16];

void rcu_config(void);
void i2c_gpio_config(void);
void i2c_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t i;

    /* RCU configure */
    rcu_config();
    /* GPIO configure */
    i2c_gpio_config();
    /* I2C configure */
    i2c_config();

    /* wait until I2C bus is idle */
    while(i2c2_flag_get(I2C2, I2C2_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C2);
        
    for(i=0; i<16; i++){
        /* wait until the RBNE bit is set */
        while(!i2c2_flag_get(I2C2, I2C2_FLAG_RBNE));
        /* read a data from I2C_DATA */
        i2c_receiver[i] = i2c_data_receive(I2C2);
    }
    while(!i2c2_flag_get(I2C2, I2C2_FLAG_TC));
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C2);
    /* wait until stop condition generate */
    while(!i2c2_flag_get(I2C2, I2C2_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c2_flag_clear(I2C2, I2C2_FLAG_STPDET);

    while(1){
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable AFIO clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable I2C2 clock */
    rcu_periph_clock_enable(RCU_I2C2);
}

/*!
    \brief      configure the I2C2 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_gpio_config(void)
{
    /* configure AFIO port alternate function */
    gpio_afio_port_config(AFIO_PA8_I2C2_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC9_I2C2_CFG, ENABLE);

    /* connect PA8 to I2C2_SCL */
    /* connect PC9 to I2C2_SDA */
    gpio_init(GPIOC, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

/*!
    \brief      configure the I2C2 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2C2, 0, 0x8, 0);
    i2c_master_clock_config(I2C2, 0x33, 0xA8);

#if I2C_10BIT_ADDRESS
    /* enable 10-bit addressing mode in master mode */
    i2c_address10_enable(I2C2);
    /* configure I2C address */
    i2c_address_config(I2C2, I2C2_SLAVE_ADDRESS10, I2C2_ADDFORMAT_10BITS);
    /* send slave address to I2C bus */
    i2c2_master_addressing(I2C2, I2C2_SLAVE_ADDRESS10, I2C2_MASTER_RECEIVE);

#else
    /* configure I2C address */
    i2c_address_config(I2C2, I2C2_SLAVE_ADDRESS7, I2C2_ADDFORMAT_7BITS);
    /* send slave address to I2C bus */
    i2c2_master_addressing(I2C2, I2C2_SLAVE_ADDRESS7, I2C2_MASTER_RECEIVE);

#endif
    i2c_transfer_byte_number_config(I2C2, 16);
    /* enable I2C2 */
    i2c_enable(I2C2);
}
