/*!
    \file    main.c
    \brief   slave transmitter

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

#define I2C_10BIT_ADDRESS      0

#define I2C_OWN_ADDRESS7       0x82
#define I2C_SLAVE_ADDRESS7     0x72
#define I2C_OWN_ADDRESS10      0x0322

uint8_t i2c_transmitter[16];

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
    int i;
    
    /* RCU config */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* I2C config */
    i2c_config();

    for(i = 0; i < 16; i++){
        i2c_transmitter[i] = i + 0x80;
    }

#if I2C_10BIT_ADDRESS
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(BOARD_I2C, I2C_FLAG_ADDSEND);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(BOARD_I2C, I2C_STAT0_ADDSEND);
#else
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(BOARD_I2C, I2C_FLAG_ADDSEND);
#endif
    /* wait until the transmission data register is empty */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_TBE));

    for(i = 0; i < 16; i++){
        /* send a data byte */
        i2c_data_transmit(BOARD_I2C, i2c_transmitter[i]);
        /* wait until the transmission data register is empty */
        while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_TBE));
    }
    /* the master doesn't acknowledge for the last byte */
    while(!i2c_flag_get(BOARD_I2C, I2C_FLAG_AERR));
    /* clear the bit of AERR */
    i2c_flag_clear(BOARD_I2C, I2C_FLAG_AERR);

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
    /* connect I2C_SCL_GPIO_PIN to I2C_SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SCL_GPIO_PIN);
    /* connect I2C_SDA_GPIO_PIN to I2C_SDA */
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
#if I2C_10BIT_ADDRESS
    i2c_mode_addr_config(BOARD_I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_10BITS, I2C_OWN_ADDRESS10);
#else
    i2c_mode_addr_config(BOARD_I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_OWN_ADDRESS7);
#endif
    /* enable I2C */
    i2c_enable(BOARD_I2C);
    /* enable acknowledge */
    i2c_ack_config(BOARD_I2C, I2C_ACK_ENABLE);
}
