/*!
    \file    main.c
    \brief   master receiver

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h"

#ifdef GD32F130_150
#define BOARD_I2C I2C0
#define I2C_SCL_GPIO_PIN GPIO_PIN_6
#define I2C_SDA_GPIO_PIN GPIO_PIN_7
#define I2C_RCC RCU_I2C0
#elif defined GD32F170_190
#define BOARD_I2C I2C1
#define I2C_SCL_GPIO_PIN GPIO_PIN_10
#define I2C_SDA_GPIO_PIN GPIO_PIN_11
#define I2C_RCC RCU_I2C1
#endif

#define I2C_10BIT_ADDRESS     0

#define I2C_OWN_ADDRESS7      0x72
#define I2C_SLAVE_ADDRESS7    0x82
#define I2C_SLAVE_ADDRESS10   0x0322

uint8_t i2c_receiver[16];

void rcu_config(void);
void gpio_config(void);
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
#if I2C_10BIT_ADDRESS
    uint8_t slave10_first_byte,slave10_second_byte;
#endif

    /* RCU config */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* I2C config */
    i2c_config();

    /* wait until I2C bus is idle */
    while(i2c_flag_get(BOARD_I2C, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(BOARD_I2C);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_SBSEND));

#if I2C_10BIT_ADDRESS
    slave10_first_byte = (0xF0) | (uint8_t)((I2C_SLAVE_ADDRESS10 & 0x0300) >> 7);
    /* send slave address first byte to I2C bus */
    i2c_master_addressing(BOARD_I2C, slave10_first_byte, I2C_TRANSMITTER);
    /* wait until ADD10SEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_ADD10SEND));
    /* the second byte contains the remaining 8 bits of the 10-bit address */
    slave10_second_byte = (uint8_t)(I2C_SLAVE_ADDRESS10 & 0x00FF);
    /* send slave address 2nd byte to I2C bus */
    i2c_master_addressing(BOARD_I2C, slave10_second_byte, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(BOARD_I2C, I2C_FLAG_ADDSEND);
    /* send a start condition to I2C bus */
    i2c_start_on_bus(BOARD_I2C);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_SBSEND));
    /* send slave address first byte to I2C bus */
    i2c_master_addressing(BOARD_I2C, slave10_first_byte, I2C_RECEIVER);
#else
    /* send slave address to I2C bus */
    i2c_master_addressing(BOARD_I2C, I2C_SLAVE_ADDRESS7, I2C_RECEIVER);
#endif
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(BOARD_I2C, I2C_FLAG_ADDSEND);

    for(i = 0; i < 16; i++){
        if(13 == i){
            /* wait until the second last data byte is received into the shift register */
            while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_BTC));
            /* disable acknowledge */
            i2c_ack_config(BOARD_I2C, I2C_ACK_DISABLE);
        }
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_RBNE));
        /* read a data from I2C_DATA */
        i2c_receiver[i] = i2c_data_receive(BOARD_I2C);
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(BOARD_I2C);
    /* wait until stop condition generate */
    while(I2C_CTL0(BOARD_I2C) & 0x0200);
    /* enable acknowledge */
    i2c_ack_config(BOARD_I2C, I2C_ACK_ENABLE);

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
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C clock */
    rcu_periph_clock_enable(I2C_RCC);
}

/*!
    \brief      cofigure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* I2C GPIO ports */
    /* connect I2C_SCL_GPIO_PIN to I2C SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SCL_GPIO_PIN);
    /* connect I2C_SDA_GPIO_PIN to I2C SDA */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SDA_GPIO_PIN);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_GPIO_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PIN);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_GPIO_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PIN);
}

/*!
    \brief      cofigure the I2C interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* I2C clock configure */
    i2c_clock_config(BOARD_I2C, 400000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(BOARD_I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_OWN_ADDRESS7);
    /* enable I2C */
    i2c_enable(BOARD_I2C);
    /* enable acknowledge */
    i2c_ack_config(BOARD_I2C, I2C_ACK_ENABLE);
}
