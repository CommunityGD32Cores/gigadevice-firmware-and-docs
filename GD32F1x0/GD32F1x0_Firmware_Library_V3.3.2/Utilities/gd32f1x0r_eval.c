 /*!
    \file    gd32f1x0r_eval.c
    \brief   firmware functions to manage leds, keys, COM ports

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0r_eval.h"

/* private variables */
static uint32_t GPIO_PORT[LEDn] = {LED1_GPIO_PORT,
                                   LED2_GPIO_PORT,
                                   LED3_GPIO_PORT,
                                   LED4_GPIO_PORT};

static uint32_t GPIO_PIN[LEDn] = {LED1_PIN,
                                  LED2_PIN, 
                                  LED3_PIN, 
                                  LED4_PIN};


static rcu_periph_enum COM_CLK[COMn] = {
#ifdef GD32F130_150
                                        EVAL_COM0_CLK,
#elif defined(GD32F170_190)
                                        EVAL_COM1_CLK,
                                        EVAL_COM2_CLK
#endif
                                       };

static uint32_t COM_TX_PIN[COMn] = {
#ifdef GD32F130_150
                                    EVAL_COM0_TX_PIN,
#elif defined(GD32F170_190)
                                    EVAL_COM1_TX_PIN,
                                    EVAL_COM2_TX_PIN
#endif
                                   };

static uint32_t COM_RX_PIN[COMn] = {
#ifdef GD32F130_150
                                    EVAL_COM0_RX_PIN,
#elif defined(GD32F170_190)
                                    EVAL_COM1_RX_PIN,
                                    EVAL_COM2_RX_PIN
#endif
                                   };

static rcu_periph_enum GPIO_CLK[LEDn] = {LED1_GPIO_CLK,
                                         LED2_GPIO_CLK, 
                                         LED3_GPIO_CLK, 
                                         LED4_GPIO_CLK};

static uint32_t KEY_PORT[KEYn] = {WAKEUP_KEY_GPIO_PORT, 
                                  TAMPER_KEY_GPIO_PORT,
#ifdef GD32F130_150                            
                                  USER_KEY_GPIO_PORT
#endif
                                  };

static uint32_t KEY_PIN[KEYn] = {WAKEUP_KEY_PIN, 
                                 TAMPER_KEY_PIN,
#ifdef GD32F130_150
                                 USER_KEY_PIN
#endif
                                 };

static rcu_periph_enum KEY_CLK[KEYn] = {WAKEUP_KEY_GPIO_CLK, 
                                        TAMPER_KEY_GPIO_CLK,
#ifdef GD32F130_150
                                        USER_KEY_GPIO_CLK
#endif
                                       };

static exti_line_enum KEY_EXTI_LINE[KEYn] = {WAKEUP_KEY_EXTI_LINE,
                                             TAMPER_KEY_EXTI_LINE,
#ifdef GD32F130_150
                                             USER_KEY_EXTI_LINE
#endif
                                            };

static uint8_t KEY_PORT_SOURCE[KEYn] = {WAKEUP_KEY_EXTI_PORT_SOURCE,
                                        TAMPER_KEY_EXTI_PORT_SOURCE,
#ifdef GD32F130_150
                                        USER_KEY_EXTI_PORT_SOURCE
#endif
                                        };

static uint8_t KEY_PIN_SOURCE[KEYn] = {WAKEUP_KEY_EXTI_PIN_SOURCE,
                                       TAMPER_KEY_EXTI_PIN_SOURCE,
#ifdef GD32F130_150
                                       USER_KEY_EXTI_PIN_SOURCE
#endif
                                       };

static uint8_t KEY_IRQn[KEYn] = {WAKEUP_KEY_EXTI_IRQn, 
                                 TAMPER_KEY_EXTI_IRQn,
#ifdef GD32F130_150
                                 USER_KEY_EXTI_IRQn
#endif
                                 };

/* GD32F1x0 eval low layer private functions */
/*!
    \brief      configure led GPIO
    \param[in]  lednum: specify the Led to be configured
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void  gd_eval_led_init (led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
    /* configure led GPIO port */ 
    gpio_mode_set(GPIO_PORT[lednum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN[lednum]);
    gpio_output_options_set(GPIO_PORT[lednum], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN[lednum]);

    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn on selected led
    \param[in]  lednum: specify the Led to be turned on
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_on(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn off selected led
    \param[in]  lednum: specify the Led to be turned off
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_off(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      toggle selected led
    \param[in]  lednum: specify the Led to be toggled
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_toggle(led_typedef_enum lednum)
{
#ifdef GD32F130_150
    GPIO_OCTL(GPIO_PORT[lednum]) ^= GPIO_PIN[lednum];
#elif defined(GD32F170_190)
    GPIO_TG(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
#endif
}

/*!
    \brief      configure key
    \param[in]  keynum: specify the key to be configured
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_USER: user key
    \param[in]  key_mode: specify button mode
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum key_mode)
{
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[keynum]);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure button pin as input */
    gpio_mode_set(KEY_PORT[keynum], GPIO_MODE_INPUT, GPIO_PUPD_NONE,KEY_PIN[keynum]);

    if (key_mode == KEY_MODE_EXTI) {
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(KEY_IRQn[keynum], 2U, 0U);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);

        /* configure key EXTI line */
        exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
    }
}

/*!
    \brief      return the selected button state
    \param[in]  keynum: specify the button to be checked
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_USER: user key
    \param[out] none
    \retval     the button GPIO pin value
*/
uint8_t gd_eval_key_state_get(key_typedef_enum keynum)
{
    return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
}

/*!
    \brief      configure COM port
    \param[in]  COM: COM on the board
      \arg        EVAL_COM0: COM0 on the board for 150r eval board
      \arg        EVAL_COM1: COM1 on the board for 190r eval board
      \arg        EVAL_COM2: COM2 on the board for 190r eval board
    \param[out] none
    \retval     none
*/
void gd_eval_com_init(uint32_t COM)
{
    /* enable GPIO clock */
    uint32_t COM_ID = 0U;
#ifdef GD32F130_150
    if(EVAL_COM0==COM){
        COM_ID = 0U;
    }
#elif defined(GD32F170_190)
    if(EVAL_COM1==COM){
        COM_ID = 0U;
    }else if(EVAL_COM2==COM){
        COM_ID = 1U;
    }
#endif /* GD32F170_190 */

    rcu_periph_clock_enable( EVAL_COM_GPIO_CLK);

#if defined(GD32F130_150)
    /* enable USART clock */
    rcu_periph_clock_enable(COM_CLK[COM_ID]);
#elif defined(GD32F170_190)
    /* enable USART clock */
    rcu_periph_clock_enable(COM_CLK[COM_ID]);
#else
    #error "Please define GD32F130_150 or GD32F170_190"
#endif 

    /* connect port to USARTx_Tx */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_TX_PIN[COM_ID]);

    /* connect port to USARTx_Rx */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_RX_PIN[COM_ID]);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_TX_PIN[COM_ID]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ,COM_TX_PIN[COM_ID]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_RX_PIN[COM_ID]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ,COM_RX_PIN[COM_ID]);

    /* USART configure */
    usart_deinit(COM);
    usart_baudrate_set(COM,115200U);
    usart_transmit_config(COM, USART_TRANSMIT_ENABLE);
    usart_receive_config(COM, USART_RECEIVE_ENABLE);
    usart_enable(COM);
}
