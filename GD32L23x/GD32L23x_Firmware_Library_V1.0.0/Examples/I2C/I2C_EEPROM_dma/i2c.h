/*!
    \file    i2c.h
    \brief   the header file of I2C

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

#ifndef I2C_H
#define I2C_H

#include "systick.h"

#define I2C_SPEED       400000
#define I2C_PAGE_SIZE   8
#define I2CX            I2C1
#define RCU_GPIO_I2C    RCU_GPIOB
#define RCU_I2C         RCU_I2C1
#define I2C_SCL_PORT    GPIOB
#define I2C_SDA_PORT    GPIOB
#define I2C_SCL_PIN     GPIO_PIN_10
#define I2C_SDA_PIN     GPIO_PIN_11
#define I2C_GPIO_AF     GPIO_AF_4

/* function declarations */
/* configure the GPIO ports */
void gpio_config(void);
/* configure the I2C interface */
void i2c_config(void);
/* reset I2C gpio configure */
void i2c_gpio_reset(void);
/* reset i2c bus */
void i2c_bus_reset(void);

#endif /* I2C_H */
