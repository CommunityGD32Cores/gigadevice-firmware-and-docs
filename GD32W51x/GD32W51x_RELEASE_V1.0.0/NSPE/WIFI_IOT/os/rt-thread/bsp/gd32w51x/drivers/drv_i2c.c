/*!
    \file    drv_i2c.c
    \brief   I2C BSP for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#include <rtdevice.h>
#include <rthw.h>

#ifdef RT_USING_I2C

#include "drv_i2c.h"

#define DBG_TAG               "drv.I2C"
#ifdef RT_I2C_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#include <rtdbg.h>

#ifdef RT_USING_I2C_BITOPS

/*user can change this*/
#define I2C_BUS_NAME  "i2c2"

/*user should change this to adapt specific board*/
#define I2C_SCL_PIN          GPIO_PIN_0
#define I2C_SCL_PORT         GPIOA
#define I2C_SCL_CLK          RCU_GPIOA
#define I2C_SDA_PIN          GPIO_PIN_1
#define I2C_SDA_PORT         GPIOA
#define I2C_SDA_CLK          RCU_GPIOA

struct gd32_i2c_bit_data
{
    struct
    {
        rcu_periph_enum clk;
        rt_uint32_t port;
        rt_uint32_t pin;
    }scl, sda;
};

static void gpio_set_sda(void *data, rt_int32_t state)
{
    struct gd32_i2c_bit_data* bd = data;

    if (state)
    {
        gpio_bit_set(bd->sda.port, bd->sda.pin);
    }
    else
    {
        gpio_bit_reset(bd->sda.port, bd->sda.pin);
    }
}

static void gpio_set_scl(void *data, rt_int32_t state)
{
    struct gd32_i2c_bit_data* bd = data;
    if (state)
    {
        gpio_bit_set(bd->scl.port, bd->scl.pin);
    }
    else
    {
        gpio_bit_reset(bd->scl.port, bd->scl.pin);
    }
}

static rt_int32_t gpio_get_sda(void *data)
{
    struct gd32_i2c_bit_data* bd = data;

    return gpio_input_bit_get(bd->sda.port, bd->sda.pin);
}

static rt_int32_t gpio_get_scl(void *data)
{
    struct gd32_i2c_bit_data* bd = data;

    return gpio_input_bit_get(bd->scl.port, bd->scl.pin);
}

static void gpio_udelay(rt_uint32_t us)
{
    int i = ( rcu_clock_freq_get(CK_SYS) / 4000000 * us);
    while(i)
    {
        i--;
    }
}

static void drv_i2c_gpio_init(const struct gd32_i2c_bit_data* bd)
{
    rcu_periph_clock_enable(bd->sda.clk);
    rcu_periph_clock_enable(bd->scl.clk);
    gpio_init(bd->sda.port, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, bd->sda.pin);
    gpio_init(bd->scl.port, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, bd->scl.pin);

    gpio_bit_set(bd->sda.port, bd->sda.pin);
    gpio_bit_set(bd->scl.port, bd->scl.pin);
}

#else

struct gd32_i2c_bus
{
    struct rt_i2c_bus_device parent;
    rt_uint32_t i2c_periph;
};

static int gd32_i2c_read(rt_uint32_t i2c_periph, rt_uint16_t slave_address, rt_uint8_t* p_buffer, rt_uint16_t data_byte)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);

    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, slave_address, I2C_MASTER_RECEIVE);

    if(1 == data_byte){
        /* disable acknowledge */
        i2c_nack_enable(i2c_periph);
        /* send a stop condition to I2C bus */
        i2c_stop_on_bus(i2c_periph);
    }

    /* while there is data to be read */
    while(data_byte)
    {
        /* wait until the RBNE bit is set and clear it */
        if(i2c_flag_get(i2c_periph, I2C_FLAG_RBNE))
        {
            /* read a byte from the EEPROM */
            *p_buffer = i2c_data_receive(i2c_periph);

            /* point to the next location where the byte read will be saved */
            p_buffer++;

            /* decrement the read bytes counter */
            data_byte--;
            if(1 == data_byte)
            {
                /* disable acknowledge */
                i2c_nack_enable(i2c_periph);
                /* send a stop condition to I2C bus */
                i2c_stop_on_bus(i2c_periph);
            }
        }
    }

    /* wait until the stop condition is finished */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_STPDET));
    i2c_flag_clear(i2c_periph, I2C_FLAG_STPDET);

    return 0;
}

static int gd32_i2c_write(rt_uint32_t i2c_periph, uint16_t slave_address, uint8_t* p_buffer, uint16_t data_byte)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);

    /* send slave address to I2C bus */
    i2c_master_addressing(i2c_periph, slave_address, I2C_MASTER_TRANSMIT);

    /* wait until the transmit data buffer is empty */
    I2C_STAT(i2c_periph) |= I2C_STAT_TBE;

    while(data_byte)
    {
        i2c_data_transmit(i2c_periph, *p_buffer);

        /* point to the next byte to be written */
        p_buffer++;

        /* decrement the write bytes counter */
        data_byte --;

        /* wait until the I2C_FLAG_TBE bit is set */
        while(!i2c_flag_get(i2c_periph, I2C_FLAG_TBE));
    }
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_TC));

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(i2c_periph);
    /* wait until stop condition generate */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_STPDET);

    return 0;
}

static rt_size_t gd32_i2c_xfer(struct rt_i2c_bus_device *bus, struct rt_i2c_msg msgs[], rt_uint32_t num)
{
    struct rt_i2c_msg *msg;
    rt_uint32_t i;
    rt_err_t ret = RT_ERROR;

    struct gd32_i2c_bus *gd32_i2c = (struct gd32_i2c_bus *)bus;

    for (i = 0; i < num; i++)
    {
        msg = &msgs[i];

        if (msg->flags & RT_I2C_ADDR_10BIT)
        {
            i2c_address_config(gd32_i2c->i2c_periph, 0, I2C_ADDFORMAT_10BITS);
        }
        else
        {
            i2c_address_config(gd32_i2c->i2c_periph, 0, I2C_ADDFORMAT_7BITS);
        }
        if (msg->flags & RT_I2C_RD)
        {
            if (gd32_i2c_read(gd32_i2c->i2c_periph, msg->addr, msg->buf, msg->len) != 0)
            {
                LOG_E("i2c bus read failed,i2c bus stop!");
                goto out;
            }
        }
        else
        {
            if (gd32_i2c_write(gd32_i2c->i2c_periph, msg->addr, msg->buf, msg->len) != 0)
            {
                LOG_E("i2c bus write failed,i2c bus stop!");
                goto out;
            }
        }
    }

    ret = i;

out:
    LOG_D("send stop condition");

    return ret;
}

static const struct rt_i2c_bus_device_ops i2c_ops =
{
    gd32_i2c_xfer,
    RT_NULL,
    RT_NULL
};

#endif

int rt_hw_i2c_init(void)
{
#ifdef RT_USING_I2C_BITOPS
    {
        static struct rt_i2c_bus_device i2c_device;
        static const struct gd32_i2c_bit_data _i2c_bdata =
        {
            /* SCL */
            {    I2C_SCL_CLK, I2C_SCL_PORT, I2C_SCL_PIN},
            /* SDA */
            {    I2C_SDA_CLK, I2C_SDA_PORT, I2C_SDA_PIN},
        };

        static const struct rt_i2c_bit_ops _i2c_bit_ops =
        {
            (void*)&_i2c_bdata,
            gpio_set_sda,
            gpio_set_scl,
            gpio_get_sda,
            gpio_get_scl,
            gpio_udelay,
            1,
            100
        };

        drv_i2c_gpio_init(&_i2c_bdata);

        i2c_device.priv = (void *)&_i2c_bit_ops;
        rt_i2c_bit_add_bus(&i2c_device, I2C_BUS_NAME);
    }
#else

#ifdef BSP_USING_I2C0
    static struct gd32_i2c_bus gd32_i2c0;
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    /* connect PA15 to I2C0_SCL */
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_15);
    /* connect PC8 to I2C0_SDA */
    gpio_af_set(GPIOC, GPIO_AF_5, GPIO_PIN_8);
    /* configure GPIO pins of I2C0 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,GPIO_PIN_15);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,GPIO_PIN_8);

    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* configure I2C clock */
    i2c_timing_config(I2C0, 1, 0x5, 0);
    i2c_master_clock_config(I2C0, 0x59, 0x81);

    i2c_enable(I2C0);

    rt_memset((void *)&gd32_i2c0, 0, sizeof(struct gd32_i2c_bus));
    gd32_i2c0.parent.ops = &i2c_ops;
    gd32_i2c0.i2c_periph = I2C0;
    rt_i2c_bus_device_register(&gd32_i2c0.parent, "i2c0");
#endif

#ifdef BSP_USING_I2C1
    static struct gd32_i2c_bus gd32_i2c1;
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* connect PC3 to I2C1_SCL */
    gpio_af_set(GPIOC, GPIO_AF_6, GPIO_PIN_3);
    /* connect PC2 to I2C1_SDA */
    gpio_af_set(GPIOC, GPIO_AF_9, GPIO_PIN_2);
    /* configure GPIO pins of I2C1 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_3);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_3);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_2);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_2);

    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C1);
    /* configure I2C clock */
    i2c_timing_config(I2C1, 0, 0x4, 0);
    i2c_master_clock_config(I2C1, 0x1A, 0x4F);

    i2c_enable(I2C1);

    rt_memset((void *)&gd32_i2c1, 0, sizeof(struct gd32_i2c_bus));
    gd32_i2c1.parent.ops = &i2c_ops;
    gd32_i2c1.i2c_periph = I2C1;
    rt_i2c_bus_device_register(&gd32_i2c1.parent, "i2c1");
#endif

#endif
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_i2c_init);

#endif
