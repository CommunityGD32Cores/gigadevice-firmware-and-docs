/*!
    \file  main.c
    \brief slave transmitter
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"

#define I2C_10BIT_ADDRESS      0

#define I2C0_OWN_ADDRESS7      0x82
#define I2C0_SLAVE_ADDRESS7    0x72
#define I2C0_OWN_ADDRESS10     0x0322

uint8_t i2c_transmitter[16];

void rcu_config(void);
void gpio_config(void);
void i2c_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int i;
    
    /* RCU config */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* I2C config */
    i2c_config();

    for(i=0; i<16; i++){
        i2c_transmitter[i] = i+0x80;
    }
    
#if I2C_10BIT_ADDRESS
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
#else
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
#endif
    /* wait until the transmission data register is empty */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));

    for(i=0;i<16;i++){
        /* send a data byte */
        i2c_data_transmit(I2C0, i2c_transmitter[i]);
        /* wait until the transmission data register is empty */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
    }
    /* the master doesn't acknowledge for the last byte */
    while(!i2c_flag_get(I2C0, I2C_FLAG_AERR));
    /* clear the bit of AERR */
    i2c_flag_clear(I2C0, I2C_FLAG_AERR);

    while(1){
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
}

/*!
    \brief      cofigure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
}

/*!
    \brief      cofigure the I2C0 and I2C1 interfaces
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* I2C clock configure */
    i2c_clock_config(I2C0, 400000, I2C_DTCY_2);
    /* I2C address configure */
#if I2C_10BIT_ADDRESS
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_10BITS, I2C0_OWN_ADDRESS10);
#else
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
#endif
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}
