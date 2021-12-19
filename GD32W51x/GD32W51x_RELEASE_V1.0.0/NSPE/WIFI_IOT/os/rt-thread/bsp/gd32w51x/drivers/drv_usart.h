/*!
    \file    drv_uart.h
    \brief   UART BSP for GD32W51x WiFi SDK

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

#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "gd32w51x.h"

#define LOG_UART USART1

#define UART_BUFFER_SIZE     128

#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)

struct gd32_usart_config
{
    char *dev_name;
    rt_uint32_t periph;
    IRQn_Type irqn;
    rcu_periph_enum per_clk;
    rcu_periph_enum tx_gpio_clk;
    rcu_periph_enum rx_gpio_clk;
    rt_uint32_t tx_port;
    rt_uint32_t tx_af;
    rt_uint32_t tx_pin;
    rt_uint32_t rx_port;
    rt_uint32_t rx_af;
    rt_uint32_t rx_pin;
};

struct gd32_usart_bus
{
    struct rt_serial_device serial;
    struct gd32_usart_config *config;
};

#ifdef BSP_USING_UART0
#define UART0_BUS_CONFIG                \
    {                                   \
        .dev_name    = "uart0",         \
        .periph      = USART0,          \
        .irqn        = USART0_IRQn,     \
        .per_clk     = RCU_USART0,      \
        .tx_gpio_clk = RCU_GPIOA,       \
        .rx_gpio_clk = RCU_GPIOA,       \
        .tx_port     = GPIOA,           \
        .tx_af       = GPIO_AF_7,       \
        .tx_pin      = GPIO_PIN_9,      \
        .rx_port     = GPIOA,           \
        .rx_af       = GPIO_AF_7,       \
        .rx_pin      = GPIO_PIN_10,     \
    }
#endif /* BSP_USING_UART0 */

#ifdef BSP_USING_UART1
#define UART1_BUS_CONFIG                \
    {                                   \
        .dev_name    = "uart1",         \
        .periph      = USART1,          \
        .irqn        = USART1_IRQn,     \
        .per_clk     = RCU_USART1,      \
        .tx_gpio_clk = RCU_GPIOB,       \
        .rx_gpio_clk = RCU_GPIOA,       \
        .tx_port     = GPIOB,           \
        .tx_af       = GPIO_AF_7,       \
        .tx_pin      = GPIO_PIN_15,     \
        .rx_port     = GPIOA,           \
        .rx_af       = GPIO_AF_3,       \
        .rx_pin      = GPIO_PIN_8,      \
    }
#endif /* BSP_USING_UART1 */

#endif