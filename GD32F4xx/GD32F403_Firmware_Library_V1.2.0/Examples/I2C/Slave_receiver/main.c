/*!
    \file  main.c
    \brief slave receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"

#define I2C0_OWN_ADDRESS7      0x82

uint8_t i2c_receiver[16];

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

    i=0;
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    for(i=0; i<16; i++){
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));

        /* read a data byte from I2C_DATA */
        i2c_receiver[i] = i2c_data_receive(I2C0);
    }
    /* wait until the STPDET bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c_enable(I2C0);

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
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}
