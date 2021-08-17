/*!
    \file  main.c
    \brief USART synchronous
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "gd32f30x.h"
#include "gd32f30x_eval.h"
#include <stdio.h>

#define txbuffer_size1   (countof(txbuffer1) - 1)
#define txbuffer_size2   (countof(txbuffer2) - 1)
#define DYMMY_BYTE       0x00
#define countof(a)       (sizeof(a) / sizeof(*(a)))

uint8_t txbuffer1[] = "USART synchronous example: USART0 -> SPI0 using TXE and RXNE Flags";
uint8_t txbuffer2[] = "USART synchronous example: SPI0 -> USART0 using TXE and RXNE Flags";
uint8_t rxbuffer1[txbuffer_size2];
uint8_t rxbuffer2[txbuffer_size1];
__IO uint8_t data_read1 = txbuffer_size2;
__IO uint8_t data_read2 = txbuffer_size1;
__IO uint8_t tx_counter1 = 0, rx_counter1 = 0;
__IO uint8_t tx_counter2 = 0, rx_counter2 = 0;
ErrStatus state1 = ERROR;
ErrStatus state2 = ERROR;

void usart_config(void);
void spi_config(void);
void led_init(void);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize leds */
    led_init();
    /* turn off LED1~4 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);

    /* configure USART */
    usart_config();

    /* configure SPI */
    spi_config();
    
    while(data_read2--){
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE)){
        }
        /* write one byte in the USART0 data register */
        usart_data_transmit(USART0, txbuffer1[tx_counter1++]);
        /* wait until end of transmit */
        while(RESET == usart_flag_get(USART0, USART_FLAG_TC)){
        }
        /* wait the byte is entirely received by SPI0 */  
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)){
        }
        /* store the received byte in the rxbuffer2 */
        rxbuffer2[rx_counter2++] = spi_i2s_data_receive(SPI0);
    }

    /* clear the USART0 data register */
    usart_data_receive(USART0);

    while(data_read1--){
        /* wait until end of transmit */
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){
        }
        /* write one byte in the SPI0 transmit data register */
        spi_i2s_data_transmit(SPI0, txbuffer2[tx_counter2++]);

        /* send a dummy byte to generate clock to slave */ 
        usart_data_transmit(USART0, DYMMY_BYTE);
        /* wait until end of transmit */
        while(RESET == usart_flag_get(USART0, USART_FLAG_TC)){
        }
        /* wait the byte is entirely received by USART0 */
        while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE)){
        }
        /* store the received byte in the rxbuffer1 */
        rxbuffer1[rx_counter1++] = usart_data_receive(USART0);
    }
  
    /* check the received data with the send ones */
    state1 = memory_compare(txbuffer1, rxbuffer2, txbuffer_size1);
    state2 = memory_compare(txbuffer2, rxbuffer1, txbuffer_size2);
    
    if(SUCCESS == state1){
        /* if the data transmitted from USART0 and received by SPI0 are the same */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED3);
    }else{
        /* if the data transmitted from USART0 and received by SPI0 are not the same */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED3);
    }
    if(SUCCESS == state2){
        /* if the data transmitted from SPI0 and received by USART0 are the same */
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED4);
    }else{
        /* if the data transmitted from SPI0 and received by USART0 are not the same */
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED4);
    }
    while(1){
    }
}
/*!
    \brief      configure USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_AF);
    /* configure USART Tx as alternate function push-pull */
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9|GPIO_PIN_8);
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
    /* configure USART synchronous mode */
    usart_synchronous_clock_enable(USART0);
    usart_synchronous_clock_config(USART0, USART_CLEN_EN, USART_CPH_2CK, USART_CPL_HIGH);
    
    usart_baudrate_set(USART0, 115200);
    /* configure USART transmitter */
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    /* configure USART receiver */
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    /* enable USART */
    usart_enable(USART0);
}

/*!
    \brief      configure SPI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_parameter;
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_AF);
    
    spi_i2s_deinit(SPI0);
    
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_5 | GPIO_PIN_6);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, GPIO_PIN_7);
    /* configure SPI0 */
    spi_init_parameter.device_mode = SPI_SLAVE;
    spi_init_parameter.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_parameter.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_parameter.nss = SPI_NSS_SOFT;
    spi_init_parameter.endian = SPI_ENDIAN_LSB;
    spi_init_parameter.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_parameter.prescale = SPI_PSC_32;
    spi_init(SPI0, &spi_init_parameter);

    /* SPI0 enable */
    spi_enable(SPI0);
}

/*!
    \brief      initialize leds
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
    \brief      memory compare function
    \param[in]  src: source data
    \param[in]  dst: destination data
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
