/*!
    \file    main.c
    \brief   master transmitter and slave receiver

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "gd32l23x.h"
#include "gd32l233r_eval.h"

#define I2C0_SLAVE_ADDRESS7 0x82
#define I2C1_SLAVE_ADDRESS7 0x72

uint8_t i2c_transmitter[16];
uint8_t i2c_receiver[16];
volatile ErrStatus state = ERROR;

void rcu_config(void);
void i2c0_gpio_config(void);
void i2c1_gpio_config(void);
void i2c_config(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int i = 0;

    /* initialize LED1, LED3, as the transfer instruction */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED3);
    /* enable the peripheral clock */
    rcu_config();
    /* configure the I2C0 and I2C1 GPIO ports */
    i2c0_gpio_config();
    i2c1_gpio_config();
    /* configure I2C0 and I2C1 */
    i2c_config();

    for(i = 0; i < 16; i++) {
        i2c_transmitter[i] = i + 0x80;
    }

    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until ADDSEND bit is set*/
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
    /* clear I2C_TDATA register */
    I2C_STAT(I2C0) |= I2C_STAT_TBE;
    for(i = 0; i < 16; i++) {
        /* send a data byte */
        i2c_data_transmit(I2C0, i2c_transmitter[i]);
        /* wait until the transmission data register is empty*/
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C1, I2C_FLAG_RBNE));
        /* read a data from I2C_RDATA */
        i2c_receiver[i] = i2c_data_receive(I2C1);
    }
    while(!i2c_flag_get(I2C0, I2C_FLAG_TC));
    /* send a stop condition to I2C bus*/
    i2c_stop_on_bus(I2C0);
    /* wait until stop condition generate */
    while(!i2c_flag_get(I2C1, I2C_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c_flag_clear(I2C1, I2C_FLAG_STPDET);
    /* wait until stop condition generate */
    while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
    state = memory_compare(i2c_transmitter, i2c_receiver, 16);
    if(SUCCESS == state) {
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED3);
    } else {
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED3);
    }
    while(1) {
    }
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
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
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* enable I2C1 clock */
    rcu_periph_clock_enable(RCU_I2C1);
}

/*!
    \brief      configure the I2C0 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c0_gpio_config(void)
{
    /* connect PB6 to I2C0_SCL */
    gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_6);
    /* connect PB7 to I2C0_SDA */
    gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_7);
    /* configure GPIO pins of I2C0 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}

/*!
    \brief      configure the I2C1 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c1_gpio_config(void)
{
    /* connect PB10 to I2C1_SCL */
    gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_10);
    /* connect PB11 to I2C1_SDA */
    gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_11);
    /* configure GPIO pins of I2C1 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
}

/*!
    \brief      configure the I2C0 and I2C1 interfaces
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2C0, 0x0, 0x3, 0);
    i2c_master_clock_config(I2C0, 0x13, 0x36);
    /* configure I2C address */
    i2c_address_config(I2C0, I2C0_SLAVE_ADDRESS7, I2C_ADDFORMAT_7BITS);
    /* configure slave address */
    i2c_master_addressing(I2C0, I2C1_SLAVE_ADDRESS7, I2C_MASTER_TRANSMIT);
    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(I2C0, 16);
    /* enable I2C0 */
    i2c_enable(I2C0);

    /* configure I2C address */
    i2c_address_config(I2C1, I2C1_SLAVE_ADDRESS7, I2C_ADDFORMAT_7BITS);
    /* enable I2C1 */
    i2c_enable(I2C1);
}
