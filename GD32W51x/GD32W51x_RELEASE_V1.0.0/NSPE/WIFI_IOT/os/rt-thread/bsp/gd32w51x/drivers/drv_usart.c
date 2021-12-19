/*!
    \file    drv_usart.c
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

#include <rthw.h>

#ifdef RT_USING_SERIAL

#if !defined(BSP_USING_UART0) && !defined(BSP_USING_UART1)
#error "Please define at least one UARTx"
#endif

#include "drv_usart.h"

static struct gd32_usart_config usart_config[] =
{
#ifdef BSP_USING_UART0
    UART0_BUS_CONFIG,
#endif
#ifdef BSP_USING_UART1
    UART1_BUS_CONFIG,
#endif
};

static struct gd32_usart_bus usart_obj[sizeof(usart_config) / sizeof(usart_config[0])];

#if defined(__ARMCC_VERSION)
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
    usart_data_transmit(LOG_UART, (uint8_t)ch);

    return ch;
}
#elif defined(__ICCARM__)
int putchar(int ch)
{
    /* Send byte to USART */
    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
    usart_data_transmit(LOG_UART, (uint8_t)ch);

    /* Return character written */
    return ch;
}
#elif defined(__GNUC__)
int _write(int fd, char *str, int len)
{
    (void)fd;
    int32_t i = 0;

    /* Send string and return the number of characters written */
    while (i != len) {
        while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
        usart_data_transmit(LOG_UART, *str);
        str++;
        i++;
    }

    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TC));

    return i;
}
#endif

void gd32_usart_gpio_init(struct gd32_usart_bus *bus)
{
    rcu_periph_clock_enable(bus->config->per_clk);
    rcu_periph_clock_enable(bus->config->tx_gpio_clk);
    rcu_periph_clock_enable(bus->config->rx_gpio_clk);

    gpio_af_set(bus->config->tx_port, bus->config->tx_af, bus->config->tx_pin);
    gpio_af_set(bus->config->rx_port, bus->config->rx_af, bus->config->rx_pin);

    gpio_mode_set(bus->config->tx_port, GPIO_MODE_AF, GPIO_PUPD_NONE, bus->config->tx_pin);
    gpio_output_options_set(bus->config->tx_port, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, bus->config->tx_pin);

    gpio_mode_set(bus->config->rx_port, GPIO_MODE_AF, GPIO_PUPD_NONE, bus->config->rx_pin);
    gpio_output_options_set(bus->config->rx_port, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, bus->config->rx_pin);

    nvic_irq_enable(bus->config->irqn, 7, 0);
    usart_deinit(bus->config->periph);
}

rt_err_t gd32_usart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct gd32_usart_bus *bus = RT_NULL;

    bus = (struct gd32_usart_bus *)serial->parent.user_data;

    gd32_usart_gpio_init(bus);

    usart_baudrate_set(bus->config->periph, cfg->baud_rate);
    usart_receive_config(bus->config->periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(bus->config->periph, USART_TRANSMIT_ENABLE);
    usart_enable(bus->config->periph);

    return RT_EOK;
}

rt_err_t gd32_usart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct gd32_usart_bus *bus = RT_NULL;

    bus = (struct gd32_usart_bus *)serial->parent.user_data;

    switch (cmd)
    {
        case RT_DEVICE_CTRL_CLR_INT:
            /* disable rx irq */
            NVIC_DisableIRQ(bus->config->irqn);
            /* disable interrupt */
            usart_interrupt_disable(bus->config->periph, USART_INT_RBNE);

            break;
        case RT_DEVICE_CTRL_SET_INT:
            /* enable rx irq */
            NVIC_EnableIRQ(bus->config->irqn);
            /* enable interrupt */
            usart_interrupt_enable(bus->config->periph, USART_INT_RBNE);
            break;
    }

    return RT_EOK;
}

int gd32_usart_putc(struct rt_serial_device *serial, char c)
{
    struct gd32_usart_bus *bus = RT_NULL;

    bus = (struct gd32_usart_bus *)serial->parent.user_data;

    while (RESET == usart_flag_get(bus->config->periph, USART_FLAG_TBE));
    usart_data_transmit(bus->config->periph, c);

    return 1;
}

int gd32_usart_getc(struct rt_serial_device *serial)
{
    int ch;
    struct gd32_usart_bus *bus = RT_NULL;

    bus = (struct gd32_usart_bus *)serial->parent.user_data;

    ch = -1;
    if (RESET != usart_flag_get(bus->config->periph, USART_FLAG_RBNE))
        ch = usart_data_receive(bus->config->periph);
    return ch;
}

rt_size_t gd32_usart_dma_transmit(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction)
{
	return RT_EOK;
}

static struct rt_uart_ops usart_ops =
{
    gd32_usart_configure,
    gd32_usart_control,
    gd32_usart_putc,
    gd32_usart_getc,
    gd32_usart_dma_transmit,
};

static void uart_isr(rt_uint32_t periph)
{
    int obj_num = 0;
    struct gd32_usart_bus *bus = RT_NULL;

    obj_num = sizeof(usart_config) / sizeof(usart_config[0]);

    for(int i = 0; i < obj_num; i++)
    {
        if(usart_obj[i].config->periph == periph)
        {
            bus = &usart_obj[i];
            break;
        }
    }

    if(bus != RT_NULL)
    {
        if ((RESET != usart_interrupt_flag_get(bus->config->periph, USART_INT_FLAG_RBNE)) &&
            (RESET != usart_flag_get(bus->config->periph, USART_FLAG_RBNE))) {
            rt_hw_serial_isr(&bus->serial, RT_SERIAL_EVENT_RX_IND);
            usart_flag_clear(bus->config->periph, USART_FLAG_RBNE);
        }
    }
}

#ifdef BSP_USING_UART0
void USART0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(USART0);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif

#ifdef BSP_USING_UART1
void USART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(USART1);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif

int rt_hw_usart_init(void)
{
    int obj_num = 0;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    obj_num = sizeof(usart_config) / sizeof(usart_config[0]);

    for(int i = 0; i < obj_num; i++)
    {
        usart_obj[i].serial.ops = &usart_ops;
        usart_obj[i].serial.config = config;
        usart_obj[i].config = &usart_config[i];

        /* register UART device */
        rt_hw_serial_register(&usart_obj[i].serial,
                              usart_obj[i].config->dev_name,
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              (void *)&usart_obj[i]);
    }

    return RT_EOK;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);

#endif