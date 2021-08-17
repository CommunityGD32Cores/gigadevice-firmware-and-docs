/*!
    \file  sccb.c
    \brief sccb driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "sccb.h"

/*!
    \brief      configure I2C gpio and I2C parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sccb_config(void)
{
    /* enable GPIOB and I2C clock */
    rcu_periph_clock_enable(DCI_SCCB_SCL_GPIO_CLK);
    rcu_periph_clock_enable(DCI_SCCB_CLK);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure I2C1_SCL(PB10), I2C1_SDA(PB11) */ 
    gpio_init(DCI_SCCB_SCL_GPIO_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_2MHZ, DCI_SCCB_SCL_PIN);
    gpio_init(DCI_SCCB_SCL_GPIO_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_2MHZ, DCI_SCCB_SDA_PIN);

    /* configure I2C parameter */
    i2c_deinit(DCI_SCCB);
    i2c_clock_config(DCI_SCCB,SCCB_SPEED,I2C_DTCY_2);
    i2c_mode_addr_config(DCI_SCCB,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,SCCB_SLAVE_ADDRESS7);
    i2c_enable(DCI_SCCB);

    i2c_ack_config(DCI_SCCB,I2C_ACK_ENABLE);
}

/*!
    \brief      write a byte at a specific camera register
    \param[in]  reg:  camera register address
    \param[in]  data: data to be written to the specific register
    \param[out] none
    \retval     0x00 if write operation is OK. 
                0xFF if timeout condition occured (device not connected or bus error).
*/
uint8_t dci_byte_write(uint8_t reg, uint8_t data)
{
    volatile uint32_t timeout = SCCB_FLAG_TIMEOUT;

    /* the software must wait until i2c bus is idle */
    while(i2c_flag_get(DCI_SCCB,I2C_FLAG_I2CBSY));  
    /* generate the start condition */
    i2c_start_on_bus(DCI_SCCB);
    
    /* test on i2c I2C_SBSEND and clear it */
    timeout = SCCB_FLAG_TIMEOUT;
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_SBSEND)){
        if ((timeout--) == 0)
            return 0xFF;
    }
    
    /* send DCI selected device slave address for write */
    i2c_master_addressing(DCI_SCCB, OV2640_DEVICE_WRITE_ADDRESS,I2C_TRANSMITTER);
    
    /* test on I2C I2C_ADDSEND and clear it */
    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_ADDSEND)){
        if ((timeout--) == 0) 
            return 0xFF;
    }
    i2c_flag_clear(DCI_SCCB,I2C_FLAG_ADDSEND);
    
    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB,I2C_FLAG_TBE)){
         if ((timeout--) == 0) 
             return 0xFF;
    }
    
    /* send register address */
    i2c_data_transmit(DCI_SCCB, (uint8_t)(reg));

    /* test on I2C I2C_BTC and clear it */
    timeout = SCCB_FLAG_TIMEOUT;
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_BTC)){
        if ((timeout--) == 0) 
            return 0xFF;
    }
    /* send data */
    i2c_data_transmit(DCI_SCCB, data);    

    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_TBE)){
        if ((timeout--) == 0)
            return 0xFF;
    }  
    /* send I2C stop condition */
    i2c_stop_on_bus(DCI_SCCB);

    /* if operation is ok, return 0 */
    return 0;
}

/*!
    \brief      read a byte from a specific camera register 
    \param[in]  reg:  camera register address
    \param[out] data: read from the specific register
    \retval     0x00 if write operation is ok. 
                0xFF if timeout condition occured (device not connected or bus error).
*/
uint8_t dci_byte_read(uint8_t reg, uint8_t *data)
{
    volatile uint32_t timeout = SCCB_FLAG_TIMEOUT;

    I2C_STAT0(DCI_SCCB) |= (uint16_t)0x0400;

    /* generate the start condition */
    i2c_start_on_bus(DCI_SCCB);

    /* test on I2C I2C_SBSEND and clear it */
    timeout = SCCB_FLAG_TIMEOUT;
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_SBSEND)){
        if ((timeout--) == 0) 
            return 0xFF;
    } 
    /* send DCI selcted device slave address for read */
    i2c_master_addressing(DCI_SCCB, OV2640_DEVICE_READ_ADDRESS,I2C_TRANSMITTER);

    /* test on I2C I2C_ADDSEND and clear it */
    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_ADDSEND)){
        if ((timeout--) == 0) 
            return 0xFF;
    }
    i2c_flag_clear(DCI_SCCB,I2C_FLAG_ADDSEND);
    
    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB,I2C_FLAG_TBE)){
        if ((timeout--) == 0) 
            return 0xFF;
    }
    
    /* send register address */
    i2c_data_transmit(DCI_SCCB, (uint8_t)(reg));
          
    /* test on I2C I2C_BTC and clear it */
    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_BTC)){
        if ((timeout--) == 0) 
            return 0xFF;
    } 

    /* clear AF flag if arised */
    I2C_STAT0(DCI_SCCB) |= (uint16_t)0x0400;
        
    /* prepare stop after receiving data */
    i2c_stop_on_bus(DCI_SCCB);
    
    /* generate the start condition */
    i2c_start_on_bus(DCI_SCCB);

    /* test on I2C I2C_SBSEND and clear it */
    timeout = SCCB_FLAG_TIMEOUT; 
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_SBSEND)){
        if ((timeout--) == 0) 
            return 0xFF;
    } 
    
    /* send DCI selcted device slave address for write */
    i2c_master_addressing(DCI_SCCB, OV2640_DEVICE_WRITE_ADDRESS,I2C_RECEIVER);
    
    /* test on I2C I2C_ADDSEND and clear it */
    timeout = SCCB_FLAG_TIMEOUT;
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_ADDSEND)){
        if ((timeout--) == 0) 
            return 0xFF;
    }
    i2c_flag_clear(DCI_SCCB,I2C_FLAG_ADDSEND); 
    
    /* prepare an nack for the next data received */
    i2c_ack_config(DCI_SCCB, I2C_ACK_DISABLE);

    /* test on I2C_RBNE and clear it */
    timeout = SCCB_FLAG_TIMEOUT;
    while(!i2c_flag_get(DCI_SCCB, I2C_FLAG_RBNE)){
        if ((timeout--) == 0) 
            return 0xFF;
    } 
    
    /* prepare stop after receiving data */
    i2c_stop_on_bus(DCI_SCCB); 

    /* receive the data */
    *data = i2c_data_receive(DCI_SCCB);
    /* clear AF flag if arised */
    I2C_STAT0(DCI_SCCB) |= (uint16_t)0x0400;

    return 0;
}
