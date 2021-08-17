/*!
    \file    gd32e103t_start.h
    \brief   definitions for GD32E103T_START's led, key hardware resources
    
    \version 2018-03-26, V1.0.0, demo for GD32E103
    \version 2020-09-30, V1.1.0, demo for GD32E103
    \version 2020-12-31, V1.2.0, demo for GD32E103
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#ifndef GD32E10X_START_H
#define GD32E10X_START_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32e10x.h"
     
/* exported types */
typedef enum 
{
    LED2 = 0,
} led_typedef_enum;

typedef enum 
{
    KEY_USER = 0
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             1U

#define LED2_PIN                         GPIO_PIN_8
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCU_GPIOA

#define KEYn                             1U

/* user push-button */
#define USER_KEY_PIN                     GPIO_PIN_0
#define USER_KEY_GPIO_PORT               GPIOA
#define USER_KEY_GPIO_CLK                RCU_GPIOA
#define USER_KEY_EXTI_LINE               EXTI_0
#define USER_KEY_EXTI_PORT_SOURCE        GPIO_PORT_SOURCE_GPIOA
#define USER_KEY_EXTI_PIN_SOURCE         GPIO_PIN_SOURCE_0
#define USER_KEY_EXTI_IRQn               EXTI0_IRQn

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

#endif /* GD32E10X_START_H */
