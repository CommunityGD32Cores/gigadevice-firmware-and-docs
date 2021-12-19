/*!
    \file    drv_gpio.c
    \brief   GPIO BSP for GD32W51x WiFi SDK

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

#ifdef RT_USING_PIN

#include "drv_gpio.h"

static const struct pin_index pins[] =
{
    GD32_PIN(1,  A, 0),
    GD32_PIN(2,  A, 1),
    GD32_PIN(3,  A, 2),
    GD32_PIN(4,  A, 3),
    GD32_PIN(5,  A, 4),
    GD32_PIN(6,  A, 5),
    GD32_PIN(7,  A, 6),
    GD32_PIN(8,  A, 7),
    GD32_PIN(9,  A, 8),
    GD32_PIN(10, A, 9),
    GD32_PIN(11, A, 10),
    GD32_PIN(12, A, 11),
    GD32_PIN(13, A, 12),
    GD32_PIN(14, A, 13),
    GD32_PIN(15, A, 14),
    GD32_PIN(16, A, 15),
    GD32_PIN(17, B, 0),
    GD32_PIN(18, B, 1),
    GD32_PIN(19, B, 2),
    GD32_PIN(20, B, 3),
    GD32_PIN(21, B, 4),
    GD32_PIN(22, B, 5),
    GD32_PIN(23, B, 6),
    GD32_PIN(24, B, 7),
    GD32_PIN(25, B, 8),
    GD32_PIN(26, B, 9),
    GD32_PIN(27, B, 10),
    GD32_PIN(28, B, 11),
    GD32_PIN(29, B, 12),
    GD32_PIN(30, B, 13),
    GD32_PIN(31, B, 14),
    GD32_PIN(32, B, 15),
    GD32_PIN(33, C, 0),
    GD32_PIN(34, C, 1),
    GD32_PIN(35, C, 2),
    GD32_PIN(36, C, 3),
    GD32_PIN(37, C, 4),
    GD32_PIN(38, C, 5),
    GD32_PIN(39, C, 6),
    GD32_PIN(40, C, 7),
    GD32_PIN(41, C, 8),
    GD32_PIN_DEFAULT,
    GD32_PIN_DEFAULT,
    GD32_PIN_DEFAULT,
    GD32_PIN_DEFAULT,
    GD32_PIN_DEFAULT,
    GD32_PIN(47, C, 14),
    GD32_PIN(48, C, 15),
};

static const struct pin_irq_map pin_irq_map[] =
{
    {GPIO_PIN_0,  EXTI0_IRQn},
    {GPIO_PIN_1,  EXTI1_IRQn},
    {GPIO_PIN_2,  EXTI2_IRQn},
    {GPIO_PIN_3,  EXTI3_IRQn},
    {GPIO_PIN_4,  EXTI4_IRQn},
    {GPIO_PIN_5,  EXTI5_9_IRQn},
    {GPIO_PIN_6,  EXTI5_9_IRQn},
    {GPIO_PIN_7,  EXTI5_9_IRQn},
    {GPIO_PIN_8,  EXTI5_9_IRQn},
    {GPIO_PIN_9,  EXTI5_9_IRQn},
    {GPIO_PIN_10, EXTI10_15_IRQn},
    {GPIO_PIN_11, EXTI10_15_IRQn},
    {GPIO_PIN_12, EXTI10_15_IRQn},
    {GPIO_PIN_13, EXTI10_15_IRQn},
    {GPIO_PIN_14, EXTI10_15_IRQn},
    {GPIO_PIN_15, EXTI10_15_IRQn},
};

struct rt_pin_irq_hdr pin_irq_hdr_tab[] =
{
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
};

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
const struct pin_index *get_pin(rt_uint8_t pin)
{
    const struct pin_index *index;

    if (pin < ITEM_NUM(pins))
    {
        index = &pins[pin];
        if (index->index == -1)
            index = RT_NULL;
    }
    else
    {
        index = RT_NULL;
    }

    return index;
};

void gd32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index = RT_NULL;
    rt_uint32_t pin_mode = 0, pin_pupd = 0, pin_odpp = 0;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }

    /* GPIO Periph clock enable */
    rcu_periph_clock_enable(index->clk);
    pin_mode = GPIO_MODE_OUTPUT;

    switch(mode)
    {
    case PIN_MODE_OUTPUT:
        /* output setting */
        pin_mode = GPIO_MODE_OUTPUT;
        pin_pupd = GPIO_PUPD_NONE;
        pin_odpp = GPIO_OTYPE_PP;
        break;
    case PIN_MODE_OUTPUT_OD:
        /* output setting: od. */
        pin_mode = GPIO_MODE_OUTPUT;
        pin_pupd = GPIO_PUPD_NONE;
        pin_odpp = GPIO_OTYPE_OD;
        break;
    case PIN_MODE_INPUT:
        /* input setting: floating mode. */
        pin_mode = GPIO_MODE_INPUT;
        pin_pupd = GPIO_PUPD_NONE;
        break;
    case PIN_MODE_INPUT_PULLUP:
        /* input setting: pull up. */
        pin_mode = GPIO_MODE_INPUT;
        pin_pupd = GPIO_PUPD_PULLUP;
        break;
    case PIN_MODE_INPUT_PULLDOWN:
        /* input setting: pull down. */
        pin_mode = GPIO_MODE_INPUT;
        pin_pupd = GPIO_PUPD_PULLDOWN;
        break;
    default:
        break;
    }

    gpio_mode_set(index->gpio_periph, pin_mode, pin_pupd, index->pin);
    if(pin_mode == GPIO_MODE_OUTPUT)
    {
        gpio_output_options_set(index->gpio_periph, pin_odpp, GPIO_OSPEED_25MHZ, index->pin);
    }
}

void gd32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index = RT_NULL;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }

    gpio_bit_write(index->gpio_periph, index->pin, (bit_status)value);
}

int gd32_pin_read(rt_device_t dev, rt_base_t pin)
{
    int value = PIN_LOW;
    const struct pin_index *index = RT_NULL;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return value;
    }

    value = gpio_input_bit_get(index->gpio_periph, index->pin);
    return value;
}

rt_inline rt_int32_t bit2bitno(rt_uint32_t bit)
{
    rt_uint8_t i;
    for (i = 0; i < 32; i++)
    {
        if ((0x01 << i) == bit)
        {
            return i;
        }
    }
    return -1;
}

rt_inline const struct pin_irq_map *get_pin_irq_map(rt_uint32_t pinbit)
{
    rt_int32_t map_index = bit2bitno(pinbit);
    if (map_index < 0 || map_index >= ITEM_NUM(pin_irq_map))
    {
        return RT_NULL;
    }
    return &pin_irq_map[map_index];
};

rt_err_t gd32_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
                              rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    const struct pin_index *index = RT_NULL;
    rt_base_t level;
    rt_int32_t hdr_index = -1;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_EINVAL;
    }

    hdr_index = bit2bitno(index->pin);
    if (hdr_index < 0 || hdr_index >= ITEM_NUM(pin_irq_map))
    {
        return RT_EINVAL;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[hdr_index].pin == pin &&
        pin_irq_hdr_tab[hdr_index].hdr == hdr &&
        pin_irq_hdr_tab[hdr_index].mode == mode &&
        pin_irq_hdr_tab[hdr_index].args == args)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    if (pin_irq_hdr_tab[hdr_index].pin != -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EFULL;
    }
    pin_irq_hdr_tab[hdr_index].pin = pin;
    pin_irq_hdr_tab[hdr_index].hdr = hdr;
    pin_irq_hdr_tab[hdr_index].mode = mode;
    pin_irq_hdr_tab[hdr_index].args = args;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

rt_err_t gd32_pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
    const struct pin_index *index = RT_NULL;
    rt_base_t level;
    rt_int32_t hdr_index = -1;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_EINVAL;
    }

    hdr_index = bit2bitno(index->pin);
    if (hdr_index < 0 || hdr_index >= ITEM_NUM(pin_irq_map))
    {
        return RT_EINVAL;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[hdr_index].pin == -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    pin_irq_hdr_tab[hdr_index].pin = -1;
    pin_irq_hdr_tab[hdr_index].hdr = RT_NULL;
    pin_irq_hdr_tab[hdr_index].mode = 0;
    pin_irq_hdr_tab[hdr_index].args = RT_NULL;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

rt_err_t gd32_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint32_t enabled)
{
    const struct pin_index *index;
    const struct pin_irq_map *irqmap;
    rt_base_t level;
    rt_int32_t hdr_index = -1;
    exti_trig_type_enum trigger_mode;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_EINVAL;
    }

    if (enabled == PIN_IRQ_ENABLE)
    {
        hdr_index = bit2bitno(index->pin);
        if (hdr_index < 0 || hdr_index >= ITEM_NUM(pin_irq_map))
        {
            return RT_EINVAL;
        }

        level = rt_hw_interrupt_disable();
        if (pin_irq_hdr_tab[hdr_index].pin == -1)
        {
            rt_hw_interrupt_enable(level);
            return RT_EINVAL;
        }

        irqmap = &pin_irq_map[hdr_index];

        switch (pin_irq_hdr_tab[hdr_index].mode)
        {
            case PIN_IRQ_MODE_RISING:
                trigger_mode = EXTI_TRIG_RISING;
                break;
            case PIN_IRQ_MODE_FALLING:
                trigger_mode = EXTI_TRIG_FALLING;
                break;
            case PIN_IRQ_MODE_RISING_FALLING:
                trigger_mode = EXTI_TRIG_BOTH;
                break;
            default:
                rt_hw_interrupt_enable(level);
                return RT_EINVAL;
        }

        /* enable and set interrupt priority */
        nvic_irq_enable(irqmap->irqno, 5U, 0U);

        /* connect EXTI line to  GPIO pin */
        syscfg_exti_line_config(index->port_src, index->pin_src);

        /* configure EXTI line */
        exti_init((exti_line_enum)(index->pin), EXTI_INTERRUPT, trigger_mode);
        exti_interrupt_flag_clear((exti_line_enum)(index->pin));

        rt_hw_interrupt_enable(level);
    }
    else if (enabled == PIN_IRQ_DISABLE)
    {
        irqmap = get_pin_irq_map(index->pin);
        if (irqmap == RT_NULL)
        {
            return RT_EINVAL;
        }
        nvic_irq_disable(irqmap->irqno);
    }
    else
    {
        return RT_EINVAL;
    }

    return RT_EOK;
}

const static struct rt_pin_ops gd32_pin_ops =
{
    gd32_pin_mode,
    gd32_pin_write,
    gd32_pin_read,
    gd32_pin_attach_irq,
    gd32_pin_detach_irq,
    gd32_pin_irq_enable,
    RT_NULL,
};

int rt_hw_pin_init(void)
{
    int result;

    result = rt_device_pin_register("pin", &gd32_pin_ops, RT_NULL);

    return result;
}
INIT_BOARD_EXPORT(rt_hw_pin_init);

rt_inline void pin_irq_hdr(int irqno)
{
    if (pin_irq_hdr_tab[irqno].hdr)
    {
        pin_irq_hdr_tab[irqno].hdr(pin_irq_hdr_tab[irqno].args);
    }
}

void GD32_GPIO_EXTI_IRQHandler(rt_int8_t exti_line)
{
    if(RESET != exti_interrupt_flag_get((exti_line_enum)(1 << exti_line)))
    {
        pin_irq_hdr(exti_line);
        exti_interrupt_flag_clear((exti_line_enum)(1 << exti_line));
    }
}
void EXTI0_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(0);
    rt_interrupt_leave();
}
void EXTI1_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(1);
    rt_interrupt_leave();
}
void EXTI2_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(2);
    rt_interrupt_leave();
}
void EXTI3_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(3);
    rt_interrupt_leave();
}
void EXTI4_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(4);
    rt_interrupt_leave();
}
void EXTI5_9_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(5);
    GD32_GPIO_EXTI_IRQHandler(6);
    GD32_GPIO_EXTI_IRQHandler(7);
    GD32_GPIO_EXTI_IRQHandler(8);
    GD32_GPIO_EXTI_IRQHandler(9);
    rt_interrupt_leave();
}
void EXTI10_15_IRQHandler(void)
{
    rt_interrupt_enter();
    GD32_GPIO_EXTI_IRQHandler(10);
    GD32_GPIO_EXTI_IRQHandler(11);
    GD32_GPIO_EXTI_IRQHandler(12);
    GD32_GPIO_EXTI_IRQHandler(13);
    GD32_GPIO_EXTI_IRQHandler(14);
    GD32_GPIO_EXTI_IRQHandler(15);
    rt_interrupt_leave();
}

#endif