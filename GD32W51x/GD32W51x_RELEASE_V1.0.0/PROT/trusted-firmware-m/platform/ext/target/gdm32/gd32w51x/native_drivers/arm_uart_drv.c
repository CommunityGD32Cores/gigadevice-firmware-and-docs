/*
 * Copyright (c) 2016-2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gd32w51x.h"
#include "arm_uart_drv.h"

#include <stddef.h>

/* UART state definitions */
#define ARM_UART_INITIALIZED  (1ul << 0)

enum arm_uart_error_t arm_uart_init(struct arm_uart_dev_t* dev)
{
    uint32_t usart_periph = dev->cfg->base;

    dev->data->baudrate = dev->cfg->default_baudrate;

    rcu_periph_clock_enable(RCU_GPIOA);
    if (usart_periph == USART0) {
    	rcu_periph_clock_enable(RCU_USART0);
    	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
    	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);
    	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_9);
    	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_10);
    } else if (usart_periph == USART1) {
    	rcu_periph_clock_enable(RCU_USART1);
        rcu_periph_clock_enable(RCU_GPIOB);
    	gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_15);
    	gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_8);
    	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_15);
    	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_8);
    }

    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, dev->data->baudrate);
    usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(usart_periph);
    // usart_interrupt_enable(usart_periph, USART_INTEN_RBNEIE);

    dev->data->state = ARM_UART_INITIALIZED;

    return ARM_UART_ERR_NONE;
}

enum arm_uart_error_t arm_uart_set_baudrate(struct arm_uart_dev_t* dev,
                                            uint32_t baudrate)
{
    uint32_t usart_periph = dev->cfg->base;

    if (baudrate == 0) {
        return ARM_UART_ERR_INVALID_BAUD;
    }

    if (!(dev->data->state & ARM_UART_INITIALIZED)) {
        return ARM_UART_ERR_NOT_INIT;
    }

    dev->data->baudrate = baudrate;
    usart_baudrate_set(usart_periph, dev->data->baudrate);

    return ARM_UART_ERR_NONE;
}

uint32_t arm_uart_get_baudrate(struct arm_uart_dev_t* dev)
{
    return dev->data->baudrate;
}

enum arm_uart_error_t arm_uart_read(struct arm_uart_dev_t* dev, uint8_t* byte)
{
    uint32_t usart_periph = dev->cfg->base;

    if (!usart_flag_get(usart_periph, USART_FLAG_RBNE)) {
        return ARM_UART_ERR_NOT_READY;
    }

    /* Reads data */
    *byte = (uint8_t)usart_data_receive(usart_periph);

    return ARM_UART_ERR_NONE;
}

enum arm_uart_error_t arm_uart_write(struct arm_uart_dev_t* dev, uint8_t byte)
{
    uint32_t usart_periph = dev->cfg->base;

    if (!usart_flag_get(usart_periph, USART_FLAG_TBE)) {
        return ARM_UART_ERR_NOT_READY;
    }

    /* Sends data */
    usart_data_transmit(usart_periph, byte);

    return ARM_UART_ERR_NONE;
}

uint32_t arm_uart_tx_ready(struct arm_uart_dev_t* dev)
{
    uint32_t usart_periph = dev->cfg->base;

    if (!(dev->data->state & ARM_UART_INITIALIZED)) {
        return 0;
    }

    return usart_flag_get(usart_periph, USART_FLAG_TBE);
}

uint32_t arm_uart_rx_ready(struct arm_uart_dev_t* dev)
{
    uint32_t usart_periph = dev->cfg->base;

    if (!(dev->data->state & ARM_UART_INITIALIZED)) {
        return 0;
    }

    return usart_flag_get(usart_periph, USART_FLAG_RBNE);
}
