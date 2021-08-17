/*!
    \file    smartcard.h
    \brief   the header file of smartcard.c

    \version 2020-07-23, V3.0.0, firmware for GD32F1x0
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

#ifndef SMARTCARD_H
#define SMARTCARD_H

#include "usbd_conf.h"

/* USART communication interface */
#define SC_USART                           USART0
#define SC_USART_CLK_ENABLE()              rcu_periph_clock_enable(RCU_USART0)
#define SC_USART_CLK_DISABLE()             rcu_periph_clock_disable(RCU_USART0)
#define SC_USART_GPIO_CLK_ENABLE()         rcu_periph_clock_enable(RCU_GPIOA)

#define SC_USART_TX_PIN                    GPIO_PIN_9
#define SC_USART_TX_GPIO_PORT              GPIOA
#define SC_USART_TX_GPIO_CLK               RCC_AHBPERIPH_GPIOA
#define SC_USART_TX_SOURCE                 GPIO_PINSOURCE9
#define SC_USART_TX_AF                     GPIO_AF_1

#define SC_USART_CK_PIN                    GPIO_PIN_8
#define SC_USART_CK_GPIO_PORT              GPIOA
#define SC_USART_CK_GPIO_CLK               RCC_AHBPERIPH_GPIOA
#define SC_USART_CK_SOURCE                 GPIO_PINSOURCE8
#define SC_USART_CK_AF                     GPIO_AF_1

#define SC_USART_IRQn                      USART0_IRQn
#define SC_USART_IRQHandler                USART0_IRQHandler

/* smartcard interface GPIO pins */
#define SC_3_5V_PIN                        GPIO_PIN_11
#define SC_3_5V_GPIO_PORT                  GPIOC
#define SC_3_5V_CLK_ENABLE()               rcu_periph_clock_enable(RCU_GPIOC)

#define SC_RESET_PIN                       GPIO_PIN_1
#define SC_RESET_GPIO_PORT                 GPIOC
#define SC_RESET_CLK_ENABLE()              rcu_periph_clock_enable(RCU_GPIOC)

#define SC_CMDVCC_PIN                      GPIO_PIN_2
#define SC_CMDVCC_GPIO_PORT                GPIOD
#define SC_CMDVCC_CLK_ENABLE()             rcu_periph_clock_enable(RCU_GPIOD)

#define SC_DETECT_PIN                      GPIO_PIN_12
#define SC_DETECT_GPIO_PORT                GPIOC
#define SC_DETECT_CLK_ENABLE()             rcu_periph_clock_enable(RCU_GPIOC)

#define SC_DETECT_EXTI_LINE                EXTI_12
#define SC_DETECT_EXTI_PIN_SOURCE          EXTI_SOURCE_PIN12
#define SC_DETECT_EXTI_PORT_SOURCE         EXTI_SOURCE_GPIOC
#define SC_DETECT_EXTI_IRQn                EXTI4_15_IRQn 
#define SC_DETECT_EXTI_IRQHandler          EXTI4_15_IRQHandler

/* exported constants */
#define T0_PROTOCOL                        0x00U   /* T0 protocol */
#define DIRECT                             0x3BU   /* direct bit convention */
#define INDIRECT                           0x3FU   /* indirect bit convention */
#define SETUP_LENGTH                       20U
#define HIST_LENGTH                        20U
#define LC_MAX                             20U
#define SC_RECEIVE_TIMEOUT                 0x8000U /* direction to reader */
#define SC_TRANSMIT_TIMEOUT                200U    /* direction to transmit */

/* SC APDU command: operation code */
/* command class */
#define SC_CLA_PSAM_INTER_NOTSAFE          0x00U   /* internal no safe tranfer */
#define SC_CLA_PSAM_INTER_ADDSAFE          0x04U   /* internal add safe tranfer */
#define SC_CLA_PSAM_EXTER_NOTSAFE          0x80U   /* external add safe tranfer */
#define SC_CLA_PSAM_EXTER_ADDSAFE          0x84U   /* external no safe tranfer */

/* data area management commands */
#define SC_WRITE_KEY                       0xD4U
#define SC_READ_BINARY                     0xB0U
#define SC_UPDATE_BINARY                   0xD6U
#define SC_READ_RECORD                     0xB2U
#define SC_APPEND_RECORD                   0xE2U
#define SC_UPDATE_RECORD                   0xDCU
#define SC_SELECT_FILE                     0xA4U
#define SC_GET_RESPONCE                    0xC0U

/* administrative commands */ 
#define SC_CREATE_FILE                     0xE0U
#define SC_SELECT_FILE                     0xA4U

/* encryption algorithm commands */ 
#define SC_INT_FOR_DESCRYPT                0x1AU   /* general DES calculation initialization */
#define SC_DES_CRYPT                       0xFAU   /* general DES calculation */
#define SC_INT_SAM_FOR_PURCHASE            0x70U   /* MAC1 calculation */
#define SC_CREDIT_SAM_FOR_PURCHASE         0x72U   /* MAC2 verify */

/* safety management commands */
#define SC_UNBLOCK_PIN                     0x2CU   /* unlock */
#define SC_APPLY_BLOCK                     0x1EU   /* application lock */
#define SC_APPLY_UNBLOCK                   0x18U   /* application unlock */
#define SC_CARD_BLOCK                      0x16U   /* card lock */
#define SC_EXTERNAL_AUTH                   0x82U   /* external authentication */
#define SC_GET_CHALLENGE                   0x84U   /* generate random numbers */
#define SC_GET_RESPONSE                    0xC0U   /* get response */
#define SC_INTERNAL_AUTH                   0x88U   /* internal authentication */
#define SC_VERIFY                          0x20U   /* PIN verify */
#define SC_CHANGE                          0x24U   /* PIN unlock */
#define SC_CHANGE_PIN                      0x5EU   /* PIN change */
#define SC_RELOAD_PIN                      0x5EU   /* PIN reload */
#define SC_OUT_KEY                         0xF6U   /* export KEY */
#define SC_DISABLE                         0x26U
#define SC_ENABLE                          0x28U

/* answer to reset commands */
#define SC_GET_A2R                         0x00U

/* sc status: status code */
#define SC_EF_SELECTED                     0x9FU
#define SC_DF_SELECTED                     0x9FU
#define SC_OP_TERMINATED                   0x9000U /* correct execution of command */

/* file type */
#define SC_FILE_TYPE_BINARY                0x00U
#define SC_FILE_TYPE_FIXED_REC             0x01U
#define SC_FILE_TYPE_CHANGE_REC            0x02U
#define SC_FILE_TYPE_ROUND_REC             0x03U
#define SC_FILE_TYPE_ATR                   0x04U
#define SC_FILE_TYPE_SECRET_KEY            0x05U

/* secret key type */
#define SC_KEY_TYPE_KEY_MAC                0x00U
#define SC_KEY_TYPE_PLAIN_MAC              0x40U
#define SC_KEY_TYPE_CRYPTO                 0x80U
#define SC_KEY_TYPE_CRYPTO_MAC             0xC0U

/* smartcard voltage */
#define SC_VOLTAGE_5V                      0U
#define SC_VOLTAGE_3V                      1U

/* smartcard state type */
typedef enum {
    SC_POWER_ON = 0x00,
    SC_RESET_LOW,
    SC_RESET_HIGH,
    SC_ACTIVE,
    SC_ACTIVE_ON_T0,
    SC_POWER_OFF,
    SC_NO_INIT
} sc_state_enum;

/* ATR structure - answer To reset */
typedef struct {
  uint8_t ts;               /* bit convention */
  uint8_t t0;               /* high nibble = number of setup byte; low nibble = number of historical byte */
  uint8_t t[SETUP_LENGTH];  /* setup array */
  uint8_t h[HIST_LENGTH];   /* historical array */
  uint8_t tlength;          /* setup array dimension */
  uint8_t hlength;          /* historical array dimension */
} sc_ATR;

/* APDU-Header command structure */
typedef struct {
  uint8_t cla;    /* command class */
  uint8_t ins;    /* operation code */
  uint8_t p1;     /* selection mode */
  uint8_t p2;     /* selection option */
} sc_header;

/* APDU-Body command structure */
typedef struct {
  uint8_t lc;            /* data field length */
  uint8_t data[LC_MAX];  /* command parameters */
  uint8_t le;            /* expected length of data to be returned */
} sc_body;

/* APDU command structure */
typedef struct {
    sc_header header;
    sc_body body;
} sc_APDU_commands;

/* sc response structure */
typedef struct {
    uint8_t data[LC_MAX];  /* data returned from the card */
    uint8_t sw1;           /* command processing status */
    uint8_t sw2;           /* command processing qualification */
} sc_APDU_response;

/* global variables declarations */
extern sc_ATR sc_A2R;
extern uint8_t ATR_length;
extern uint8_t sc_ATR_table[40];
extern __IO uint32_t card_inserted;

/* function declarations */
/* handles all smartcard states and serves to send and 
receive all communication data between smartcard and reader */
void sc_handler(sc_state_enum *scstate, sc_APDU_commands *sc_APDU, sc_APDU_response *sc_response);
/* configures the communication baudrate */
void sc_PTS_config(void);
/* initializes all peripheral used for smart card */
void sc_init(void);
/* detect whether the smartcard is present */
uint8_t sc_detect(void);
/* enable or disable the power to the smartcard communication data between smartcard and reader */
void sc_power_cmd(EventStatus new_state);
/* set or clear the smartcard reset pin */
void sc_reset(FlagStatus reset_state);
/* configure the card power voltage */
void sc_voltage_config(uint32_t sc_voltage);
/* get the right value from the D_Table index */
uint32_t sc_get_DTable_value(uint8_t idx);
/* resends the byte that failed to be received(ICCard) correctly */
void sc_parity_error_handler(void);

#endif /* SMARTCARD_H */
