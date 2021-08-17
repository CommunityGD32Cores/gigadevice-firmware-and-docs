/*!
    \file  ir_decode.h
    \brief the header file of infrared decoding
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#ifndef IR_DECODE_H
#define IR_DECODE_H

#include "gd32f3x0.h"

typedef enum {NO = 0, YES = !NO} status_yes_or_no;

#if defined(GD32F350)
  #define IR_TIMER                 TIMER2                   /*!< timer used for IR decoding */
  #define TIMER_PRESCALER          71                       /*!< TIMER prescaler */
  #define IR_TIMER_CLK             RCU_TIMER2               /*!< clock of the used timer */
  #define IR_TIMER_IRQn            TIMER2_IRQn              /*!< IR TIMER IRQ */
  #define IR_TIMER_Channel         TIMER_CH_0               /*!< IR TIMER channel */

  #define IR_GPIO_PORT             GPIOC                    /*!< port which IR output is connected */
  #define IR_GPIO_PORT_CLK         RCU_GPIOC                /*!< IR pin GPIO clock port */
  #define IR_GPIO_PIN              GPIO_PIN_6               /*!< pin which IR is connected */
  #define IR_GPIO_SOURCE           GPIO_PINSOURCE6

#elif defined(GD32F330)
  #define IR_TIMER                 TIMER2                   /*!< timer used for IR decoding */
  #define TIMER_PRESCALER          71                       /*!< TIMER prescaler */
  #define IR_TIMER_CLK             RCU_TIMER2               /*!< clock of the used timer */
  #define IR_TIMER_IRQn            TIMER2_IRQn              /*!< IR TIMER IRQ */
  #define IR_TIMER_Channel         TIMER_CH_1               /*!< IR TIMER channel */

  #define IR_GPIO_PORT             GPIOA                    /*!< port which IR output is connected */
  #define IR_GPIO_PORT_CLK         RCU_GPIOA                /*!< IR pin GPIO clock port */
  #define IR_GPIO_PIN              GPIO_PIN_7               /*!< pin which IR is connected */
  #define IR_GPIO_SOURCE           GPIO_PINSOURCE7
#endif

#endif /* IR_DECODE_H */
