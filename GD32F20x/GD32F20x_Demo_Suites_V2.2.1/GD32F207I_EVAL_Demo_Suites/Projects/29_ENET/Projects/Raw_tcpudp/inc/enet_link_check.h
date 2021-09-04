/*!
    \file  enet_link_check.h
    \brief the header file of enet_link_check

    \version 2018-7-11, V1.0.0, plug for 
             GD32F107C/GD32F207C/GD32F207I/GD32F307C/GD32F450Z EVAL board
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#ifndef ENET_LINK_CHECK_H
#define ENET_LINK_CHECK_H
#include <stdint.h>
#include "main.h"

/* ## parameter region below, modify it according to your phy chip and circuit design ## */
#define ENET_LINK_CHANGE_LEVEL           RESET
/* for DP83848, if you use other phy chip, modify below according to chip datasheet */
#define PHY_REG_MICR                     0x11U
#define PHY_REG_MISR                     0x12U
#define PHY_INT_AND_OUTPUT_ENABLE        0x03U
#define PHY_LINK_INT_ENABLE              0x20U
/* for GD32F107C/GD32F207C/GD32F207I/GD32F307C/GD32F450Z eval board, if you use board yourself,
   modify below according to your circuit design */
#ifdef GD32F450
#define LINK_CHECK_GPIO_CLK              RCU_GPIOB
#define LINK_CHECK_GPIO_PORT             GPIOB
#define LINK_CHECK_GPIO_PIN              GPIO_PIN_15
#define LINK_CHECK_EXTI_CLK              RCU_SYSCFG
#define LINK_CHECK_EXTI_LINE             EXTI_15
#define LINK_CHECK_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOB
#define LINK_CHECK_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN15
#define LINK_CHECK_EXTI_IRQn             EXTI10_15_IRQn
#define LINK_CHECK_EXTI_SOURCE_CONFIG    syscfg_exti_line_config
#define LINK_CHECK_IRQ_HANDLE            EXTI10_15_IRQHandler
#define LINK_CHECK_GPIO_CONFIG           gpio_mode_set(LINK_CHECK_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LINK_CHECK_GPIO_PIN);
#else
#define LINK_CHECK_GPIO_CLK              RCU_GPIOB
#define LINK_CHECK_GPIO_PORT             GPIOB
#define LINK_CHECK_GPIO_PIN              GPIO_PIN_0
#define LINK_CHECK_EXTI_CLK              RCU_AF
#define LINK_CHECK_EXTI_LINE             EXTI_0
#define LINK_CHECK_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOB
#define LINK_CHECK_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_0
#define LINK_CHECK_EXTI_IRQn             EXTI0_IRQn
#define LINK_CHECK_EXTI_SOURCE_CONFIG    gpio_exti_source_select
#define LINK_CHECK_IRQ_HANDLE            EXTI0_IRQHandler
#define LINK_CHECK_GPIO_CONFIG           gpio_init(LINK_CHECK_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, LINK_CHECK_GPIO_PIN);
#endif
/* ## parameter region above, modify it according to your phy chip and circuit design ## */

typedef enum{
    ENET_LINK_CHECK_RESET,
    ENET_LINK_CHECK_INT,
    ENET_LINK_CHECK_PIN_POLL,
    ENET_LINK_CHECK_REG_POLL,
}enet_link_check_enum;

typedef enum{
    ENET_LINK_STATE_UP,
    ENET_LINK_STATE_DOWN
}enet_link_state_enum;

typedef void (*link_check_cb)(enet_link_state_enum state);

/* network cable link status check initialize */
void enet_link_check_init(link_check_cb p_func, enet_link_check_enum type);
/* poll to check the link status */
uint8_t enet_link_check_poll(void);
/* link check interrupt handler */
void enet_link_check_irq(void);

#endif /* ENET_LINK_CHECK_H */
