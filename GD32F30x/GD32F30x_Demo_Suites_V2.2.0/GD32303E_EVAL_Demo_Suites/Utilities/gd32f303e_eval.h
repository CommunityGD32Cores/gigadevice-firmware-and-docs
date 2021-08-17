/*!
    \file    gd32f303e_eval.h
    \brief   definitions for GD32f303E_EVAL's leds, keys and COM ports hardware resources

    \version 2021-03-23, V2.0.0, demo for GD32F30x
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

#ifndef GD32F303E_EVAL_H
#define GD32F303E_EVAL_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f30x.h"
     
/* exported types */
typedef enum 
{
    LED2 = 0U,
    LED3 = 1U,
    LED4 = 2U,
    LED5 = 3U
}led_typedef_enum;

typedef enum 
{
    KEY_WAKEUP = 0U,
    KEY_TAMPER = 1U,
    KEY_USER1 = 2U,
    KEY_USER2 = 3U
}key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0U,
    KEY_MODE_EXTI = 1U
}keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             4U

#define LED2_PIN                         GPIO_PIN_0
#define LED2_GPIO_PORT                   GPIOF
#define LED2_GPIO_CLK                    RCU_GPIOF
  
#define LED3_PIN                         GPIO_PIN_1
#define LED3_GPIO_PORT                   GPIOF
#define LED3_GPIO_CLK                    RCU_GPIOF
  
#define LED4_PIN                         GPIO_PIN_2
#define LED4_GPIO_PORT                   GPIOF
#define LED4_GPIO_CLK                    RCU_GPIOF
  
#define LED5_PIN                         GPIO_PIN_3
#define LED5_GPIO_PORT                   GPIOF
#define LED5_GPIO_CLK                    RCU_GPIOF

#define COMn                             2U

#define EVAL_COM1                        USART0
#define EVAL_COM1_CLK                    RCU_USART0
#define EVAL_COM1_TX_PIN                 GPIO_PIN_9
#define EVAL_COM1_RX_PIN                 GPIO_PIN_10
#define EVAL_COM1_GPIO_PORT              GPIOA
#define EVAL_COM1_GPIO_CLK               RCU_GPIOA

#define EVAL_COM2                        USART1
#define EVAL_COM2_CLK                    RCU_USART1
#define EVAL_COM2_TX_PIN                 GPIO_PIN_2
#define EVAL_COM2_RX_PIN                 GPIO_PIN_3
#define EVAL_COM2_GPIO_PORT              GPIOA
#define EVAL_COM2_GPIO_CLK               RCU_GPIOA

#define KEYn                             4U

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_IRQn  

/* tamper push-button */
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_13
#define TAMPER_KEY_EXTI_IRQn             EXTI10_15_IRQn

/* user push-button */
#define USER_KEY1_PIN                    GPIO_PIN_5
#define USER_KEY1_GPIO_PORT              GPIOF
#define USER_KEY1_GPIO_CLK               RCU_GPIOF
#define USER_KEY1_EXTI_LINE              EXTI_5
#define USER_KEY1_EXTI_PORT_SOURCE       GPIO_PORT_SOURCE_GPIOF
#define USER_KEY1_EXTI_PIN_SOURCE        GPIO_PIN_SOURCE_5
#define USER_KEY1_EXTI_IRQn              EXTI5_9_IRQn

/* user push-button */
#define USER_KEY2_PIN                    GPIO_PIN_4
#define USER_KEY2_GPIO_PORT              GPIOF
#define USER_KEY2_GPIO_CLK               RCU_GPIOF
#define USER_KEY2_EXTI_LINE              EXTI_4
#define USER_KEY2_EXTI_PORT_SOURCE       GPIO_PORT_SOURCE_GPIOF
#define USER_KEY2_EXTI_PIN_SOURCE        GPIO_PIN_SOURCE_4
#define USER_KEY2_EXTI_IRQn              EXTI4_IRQn

/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum key);
/* configure COM port */
void gd_eval_com_init(uint32_t com);

#ifdef cplusplus
}
#endif

#endif /* GD32F303E_EVAL_H */
