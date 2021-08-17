/*!
    \file  gd32f190r_eval.h
    \brief definitions for GD32F190R_EVAL's leds, keys and COM ports hardware resources

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#ifndef GD32F190R_EVAL_H
#define GD32F190R_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32f1x0.h"

/* exported types */
typedef enum 
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    LED4 = 3
} led_typedef_enum;

typedef enum 
{
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* GD32190r eval low layer led */
#define LEDn                             4

#define LED1_PIN                         GPIO_PIN_11
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCU_GPIOA
  
#define LED2_PIN                         GPIO_PIN_12
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCU_GPIOA
  
#define LED3_PIN                         GPIO_PIN_6
#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK                    RCU_GPIOB
  
#define LED4_PIN                         GPIO_PIN_7
#define LED4_GPIO_PORT                   GPIOB
#define LED4_GPIO_CLK                    RCU_GPIOB

/* GD32190r eval low layer button */  
#define KEYn                             2

/* tamper push-button */ 
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN13
#define TAMPER_KEY_EXTI_IRQn             EXTI4_15_IRQn

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_1_IRQn  

/* GD32190r eval low layer COM */ 
#define COMn                             2

/* definition for COM 1, connected to USART0 */
#define EVAL_COM1                        USART0
#define EVAL_COM1_CLK                    RCU_USART0
/* definition for COM 2, connected to USART1 */
#define EVAL_COM2                        USART1
#define EVAL_COM2_CLK                    RCU_USART1

#define EVAL_COM1_TX_PIN                 GPIO_PIN_9
#define EVAL_COM1_RX_PIN                 GPIO_PIN_10
#define EVAL_COM2_TX_PIN                 GPIO_PIN_2
#define EVAL_COM2_RX_PIN                 GPIO_PIN_3

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_1

/* function declarations */
/* configures led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum key_mode);
/* return the selected button state */
uint8_t gd_eval_key_state_get(key_typedef_enum keynum);
/* configure COM port */
void gd_eval_com_init(uint32_t COM);

#ifdef cplusplus
}
#endif

#endif /* GD32F190R_EVAL_H */
