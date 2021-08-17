/*!
    \file  gd32f207i_eval.h
    \brief definitions for GD32F207i_EVAL's leds, keys and COM ports hardware resources
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef GD32F207I_EVAL_H
#define GD32F207I_EVAL_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f20x.h"
     
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
    KEY_USER = 2,
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             4U

#define LED1_PIN                         GPIO_PIN_6
#define LED1_GPIO_PORT                   GPIOF
#define LED1_GPIO_CLK                    RCU_GPIOF
  
#define LED2_PIN                         GPIO_PIN_7
#define LED2_GPIO_PORT                   GPIOF
#define LED2_GPIO_CLK                    RCU_GPIOF
  
#define LED3_PIN                         GPIO_PIN_8
#define LED3_GPIO_PORT                   GPIOF
#define LED3_GPIO_CLK                    RCU_GPIOF
  
#define LED4_PIN                         GPIO_PIN_9
#define LED4_GPIO_PORT                   GPIOF
#define LED4_GPIO_CLK                    RCU_GPIOF

#define COMn                             1U

#define EVAL_COM1                        USART0
#define EVAL_COM1_CLK                    RCU_USART0
#define EVAL_COM1_TX_PIN                 GPIO_PIN_9
#define EVAL_COM1_RX_PIN                 GPIO_PIN_10
#define EVAL_COM1_GPIO_PORT              GPIOA
#define EVAL_COM1_GPIO_CLK               RCU_GPIOA

#define KEYn                             3U

/* tamper push-button */
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      GPIO_EVENT_PORT_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       GPIO_EVENT_PIN_13
#define TAMPER_KEY_EXTI_IRQn             EXTI10_15_IRQn

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      GPIO_EVENT_PORT_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       GPIO_EVENT_PIN_0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_IRQn  

/* user push-button */
#define USER_KEY_PIN                     GPIO_PIN_14
#define USER_KEY_GPIO_PORT               GPIOB
#define USER_KEY_GPIO_CLK                RCU_GPIOB
#define USER_KEY_EXTI_LINE               EXTI_14
#define USER_KEY_EXTI_PORT_SOURCE        GPIO_EVENT_PORT_GPIOB
#define USER_KEY_EXTI_PIN_SOURCE         GPIO_EVENT_PIN_14
#define USER_KEY_EXTI_IRQn               EXTI10_15_IRQn

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

#endif /* GD32F207I_EVAL_H */
