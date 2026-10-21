/*!
    \file    main.c
    \brief   slave transmitter

    \version 2023-12-31, V3.5.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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
#include <stdio.h>
#include "gd32f1x0r_eval.h"

#define I2CX               I2C0
#define I2C_SCL_PIN        GPIO_PIN_6
#define I2C_SDA_PIN        GPIO_PIN_7
#define RCU_I2C            RCU_I2C0

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

    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* configure RCU */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure I2C */
    i2c_config();

    for(i = 0; i < 16; i++){
        i2c_transmitter[i] = i + 0x80;
    }

#if I2C_10BIT_ADDRESS
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
#else
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
#endif
    /* wait until the transmission data register is empty */
    while(!i2c_flag_get(I2CX, I2C_FLAG_TBE));

    for(i = 0; i < 16; i++) {
        /* send a data byte */
        i2c_data_transmit(I2CX, i2c_transmitter[i]);
        /* wait until the transmission data register is empty */
        while(!i2c_flag_get(I2CX, I2C_FLAG_TBE));
    }
    /* the master doesn't acknowledge for the last byte */
    while(!i2c_flag_get(I2CX, I2C_FLAG_AERR));
    /* clear the bit of AERR */
    i2c_flag_clear(I2CX, I2C_FLAG_AERR);

    /* turn on LEDs */
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED3);

    while(1) {
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
    rcu_periph_clock_enable(RCU_I2C);
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* I2C GPIO ports */
    /* connect I2C_SCL_GPIO_PIN to I2C_SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SCL_PIN);
    /* connect I2C_SDA_GPIO_PIN to I2C_SDA */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SDA_PIN);
    /* configure GPIO pins of I2C */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}

/*!
    \brief      configure the I2C interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C clock */
    i2c_clock_config(I2CX, 400000, I2C_DTCY_2);
    /* configure I2C address */
#if I2C_10BIT_ADDRESS
    i2c_mode_addr_config(I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_10BITS, I2C_OWN_ADDRESS10);
#else
    i2c_mode_addr_config(I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_OWN_ADDRESS7);
#endif
    /* enable I2CX */
    i2c_enable(I2CX);
    /* enable acknowledge */
    i2c_ack_config(I2CX, I2C_ACK_ENABLE);
}
