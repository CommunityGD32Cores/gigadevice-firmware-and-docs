/*!
    \file    i2c.c
    \brief   I2C configuration file

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

#include "i2c.h"

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIO_I2C);
    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C);

    /* connect PB10 to I2C_SCL */
    gpio_af_set(I2C_SCL_PORT, I2C_GPIO_AF, I2C_SCL_PIN);
    /* connect PB11 to I2C_SDA */
    gpio_af_set(I2C_SDA_PORT, I2C_GPIO_AF, I2C_SDA_PIN);
    /* configure GPIO pins of I2C */
    gpio_mode_set(I2C_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN);
    gpio_output_options_set(I2C_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_PIN);
    gpio_output_options_set(I2C_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}

/*!
    \brief      configure the I2C interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2CX, 0, 0x3, 0);
    i2c_master_clock_config(I2CX, 0x13, 0x36);
    /* enable I2C */
    i2c_enable(I2CX);
    /* enable the I2CX interrupt */
    i2c_interrupt_enable(I2CX, I2C_INT_ERR | I2C_INT_STPDET | I2C_INT_RBNE | I2C_INT_TC | I2C_INT_TI);
    nvic_irq_enable(I2C1_EV_IRQn, 2);
    nvic_irq_enable(I2C1_ER_IRQn, 1);
}

/*!
    \brief      reset I2C gpio configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_gpio_reset(void)
{
    /* reset PB10 and PB11 */
    gpio_mode_set(I2C_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SCL_PIN);
    gpio_output_options_set(I2C_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SDA_PIN);
    gpio_output_options_set(I2C_SDA_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}

/*!
    \brief      reset i2c bus
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_bus_reset()
{
    /* configure SDA/SCL for GPIO */
    GPIO_BC(I2C_SCL_PORT) |= I2C_SCL_PIN;
    GPIO_BC(I2C_SDA_PORT) |= I2C_SDA_PIN;
    /* reset PB10 and PB11 */
    i2c_gpio_reset();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    /* stop signal */
    GPIO_BOP(I2C_SCL_PORT) |= I2C_SCL_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(I2C_SDA_PORT) |= I2C_SDA_PIN;
    /* connect PB10 to I2C_SCL */
    /* connect PB11 to I2C_SDA */
    gpio_mode_set(I2C_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN);
    gpio_output_options_set(I2C_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_PIN);
    gpio_output_options_set(I2C_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}
