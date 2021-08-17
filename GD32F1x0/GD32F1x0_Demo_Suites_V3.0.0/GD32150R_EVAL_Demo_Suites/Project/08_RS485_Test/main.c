/*!
    \file  main.c
    \brief USART in RS485 mode
    
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

#include "gd32f1x0.h"
#include "gd32f150r_eval.h"
#include "systick.h"
#include <stdio.h>

#define COUNTOF(a)   (sizeof(a) / sizeof(*(a)))
#define BUFFER_SIZE  (COUNTOF(tx_buffer)-1)
#define NO_KEY       3
typedef enum
{
    IDLE = 0,
    RX_MODE,
    TX_MODE
}rs485_mode_enum;

/* transmit buffer and receive buffer */
uint8_t tx_buffer[] = "GD32MCU";
uint8_t rx_buffer[BUFFER_SIZE];
/* counter of transmit buffer and receive buffer */
__IO uint16_t tx_count = 0, rx_count = 0;
/* size of transmit buffer and receive buffer */
uint32_t rx_buffer_size = BUFFER_SIZE, tx_buffer_size = BUFFER_SIZE;
rs485_mode_enum rs485_mode = IDLE;

void led_init(void);
void key_init(void);
void usart_gpio_config(void);
void nvic_config(void);
void led_flash(int times);
uint8_t key_pressed_check(void);
void rs485_mode_set(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t i = 0;
    /* initialize the LEDs and KEYs */
    led_init();
    key_init();
    
    /* configure USART and GPIO */
    usart_gpio_config();
    gd_eval_com_init(EVAL_COM0);
    
    /* configure NVIC and systick */
    nvic_config();
    systick_config();
    
    /* flash the LEDs for 1 time */
    led_flash(1);
    gd_eval_led_on(LED1);
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED3);
    
    /* display information */
    printf("\n\r----------------------------------------------------------------------");
    printf("\n\r    GD32F150R_EVAL RS485_Test");
    printf("\n\r--> Press down KEY_WAKEUP to set GD32F150R_EVAL as RS485 transmitter");
    printf("\n\r--> Press down KEY_TAMPER to set GD32F150R_EVAL as RS485 receiver");
    printf("\n\r----------------------------------------------------------------------");
    
    /* wait until the mode is transmit mode or receive mode */
    while(IDLE == rs485_mode){
        rs485_mode_set();
    }
    
    while (1){
        if (RX_MODE == rs485_mode){
            /* receive data in RS485 mode */
            usart_interrupt_enable(USART1, USART_INT_RBNE);
            while (rx_count < rx_buffer_size);
            printf("\n\r The received data: ");
            printf("[%s]", rx_buffer);
            rx_count = 0;
        } else if (TX_MODE == rs485_mode){
            /* transmit data in RS485 mode */
            usart_interrupt_enable(USART1, USART_INT_TBE);
            while (tx_count < tx_buffer_size);
            printf("\n\r Data is being transmitted: ");
            /* print out the transmit buffer */
            for(i=0; i<tx_buffer_size; i++){
                printf("%c", tx_buffer[i]);
                if(i == tx_buffer_size){
                    printf("\n\r");
                }
            }
            tx_count = 0;
            delay_1ms(3000);
        } else {
            rs485_mode_set();
        }
    }
}

/*!
    \brief      initialize the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/*!
    \brief      initialize the keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_init(void)
{
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);
}

/*!
    \brief      configure the USART and GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_gpio_config(void)
{
    /* enable the USART clock and GPIO clock */
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* configure USART0 GPIO ports(PA.9, PA.10) */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
    
    /* configure USART0 */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200);
    /* enable USART transmission and reception */
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_enable(USART0);
    
    /* configure USART1 GPIO ports(PA.1, PA.2, PA.3) */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    
    /* configure USART1 in RS485 mode */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200);
    
    /* enable USART RS485 driver */
    usart_rs485_driver_enable(USART1);
    /* configure the driver enable polarity mode */
    usart_depolarity_config(USART1, USART_DEP_HIGH);
    /* set the RS485 driver assertion and de-assertion time */
    usart_driver_assertime_config(USART1, 4);
    usart_driver_deassertime_config(USART1, 2);
    /* enable USART transmission and reception */
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_enable(USART1);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
    nvic_irq_enable(USART1_IRQn, 0, 2);
}

/*!
    \brief      flash the LEDs for test
    \param[in]  times: times to flash the LEDs
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for (i = 0;i < times;i++){
        /* delay 400 ms */
        delay_1ms(400);

        /* turn on LEDs */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* delay 400 ms */
        delay_1ms(400);

        /* turn off LEDs */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}

/*!
    \brief      check the state of key and return which key is pressed
    \param[in]  none
    \param[out] none
    \retval     the key is pressed
*/
uint8_t key_pressed_check(void)
{
    /* wakeup key is pressed */
    if(RESET == gd_eval_key_state_get(KEY_WAKEUP)){
        /* wait until the key is up */
        while (RESET == gd_eval_key_state_get(KEY_WAKEUP));
        return KEY_WAKEUP; 
    }
    
    /* tamper key is pressed */
    if(RESET == gd_eval_key_state_get(KEY_TAMPER)){
        /* wait until the key is up */
        while (RESET == gd_eval_key_state_get(KEY_TAMPER));
        return KEY_TAMPER; 
    }
    
    /* user key is pressed */
    if(RESET == gd_eval_key_state_get(KEY_USER)){
        /* wait until the key is up */
        while (RESET == gd_eval_key_state_get(KEY_USER));
        return KEY_USER; 
    }else{
        /* no key is pressed */
        return NO_KEY;
    }
}

/*!
    \brief      set mode according to which key is pressed
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rs485_mode_set(void)
{
    switch (key_pressed_check()){
    case KEY_WAKEUP:
        /* transmit data in RS485 mode */
        rs485_mode = TX_MODE;
        printf("\n\r RS485 transmitter is enabled");
        gd_eval_led_on(LED2);
        gd_eval_led_off(LED4);
        break;
    case KEY_TAMPER:
        /* receive data in RS485 mode */
        rs485_mode = RX_MODE;
        printf("\n\r RS485 receiver is enabled");
        printf("\n\r Waiting for received data");
        gd_eval_led_on(LED3);
        gd_eval_led_off(LED4);
        break;
    case KEY_USER:
        /* no transmit or receive data is chosen */
        rs485_mode = IDLE;
        printf("\n\r Please set the work mode");
        printf("\n\r KEY_WAKEUP: set GD32F150R_EVAL as RS485 transmitter");
        printf("\n\r KEY_TAMPER: set GD32F150R_EVAL as RS485 receiver");
        gd_eval_led_on(LED4);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        break;
    case NO_KEY:
        rs485_mode = IDLE;
        break;
    default:
        rs485_mode = IDLE;
        break;
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
