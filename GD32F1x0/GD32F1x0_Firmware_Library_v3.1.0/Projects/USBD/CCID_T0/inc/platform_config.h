/*!
    \file  platform_config.h
    \brief Evaluation board specific configuration file
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/
  
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Define the GD32F1x0 hardware used from GD32150R-EVAL board */
#define SC_USART                           USART0
#define SC_USART_CLK                       RCU_USART0
#define SC_USART_APBPERIPHCLOCK            rcu_periph_clock_enable
#define SC_USART_IRQn                      USART0_IRQn
#define SC_USART_IRQHandler                USART0_IRQHandler

#define SC_USART_TX_PIN                    GPIO_PIN_9
#define SC_USART_TX_GPIO_PORT              GPIOA
#define SC_USART_TX_GPIO_CLK               RCU_GPIOA
#define SC_USART_TX_SOURCE                 GPIO_PIN_9
#define SC_USART_TX_AF                     GPIO_AF_1

#define SC_USART_CK_PIN                    GPIO_PIN_8
#define SC_USART_CK_GPIO_PORT              GPIOA
#define SC_USART_CK_GPIO_CLK               RCU_GPIOA
#define SC_USART_CK_SOURCE                 GPIO_PIN_8
#define SC_USART_CK_AF                     GPIO_AF_1

/* Smartcard Interface GPIO pins */
#define SC_3_5V_PIN                        GPIO_PIN_11 // LED2 ON
#define SC_3_5V_GPIO_PORT                  GPIOC
#define SC_3_5V_GPIO_CLK                   RCU_GPIOC

#define SC_RESET_PIN                       GPIO_PIN_1
#define SC_RESET_GPIO_PORT                 GPIOC
#define SC_RESET_GPIO_CLK                  RCU_GPIOC

#define SC_CMDVCC_PIN                      GPIO_PIN_2 //LED4 ON
#define SC_CMDVCC_GPIO_PORT                GPIOD
#define SC_CMDVCC_GPIO_CLK                 RCU_GPIOD

#define SC_OFF_PIN                         GPIO_PIN_12
#define SC_OFF_GPIO_PORT                   GPIOC
#define SC_OFF_GPIO_CLK                    RCU_GPIOC

#define SC_OFF_EXTI_LINE                   EXTI_12
#define SC_OFF_EXTI_PORT_SOURCE            EXTI_SOURCE_GPIOC
#define SC_OFF_EXTI_PIN_SOURCE             EXTI_SOURCE_PIN12

#define SC_OFF_EXTI_IRQn                   EXTI4_15_IRQn 
#define SC_OFF_EXTI_IRQHandler             EXTI4_15_IRQHandler

#endif /* __PLATFORM_CONFIG_H */
