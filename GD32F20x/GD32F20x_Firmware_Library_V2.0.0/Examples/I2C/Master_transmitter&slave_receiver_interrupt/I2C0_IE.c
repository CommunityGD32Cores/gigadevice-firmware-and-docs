/*!
    \file  I2C0_IE.c
    \brief I2C0 master transmitter interrupt program
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x_i2c.h"
#include "I2C_IE.h"

uint32_t event1;

/*!
    \brief      handle I2C0 event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_EventIRQ_Handler(void)
{
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SBSEND)){
        /* send slave address */
        i2c_master_addressing(I2C0, I2C1_SLAVE_ADDRESS7, I2C_TRANSMITTER);
    }else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)){
        /*clear ADDSEND bit */
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
    }else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_TBE)){
        if(I2C_nBytes>0){
            /* the master sends a data byte */
            i2c_data_transmit(I2C0, *i2c_txbuffer++);
            I2C_nBytes--;
        }else{
            /* the master sends a stop condition to I2C bus */
            i2c_stop_on_bus(I2C0);
            /* disable the I2C0 interrupt */
            i2c_interrupt_disable(I2C0, I2C_CTL1_ERRIE | I2C_CTL1_BUFIE | I2C_CTL1_EVIE);
        }
    }
}

/*!
    \brief      handle I2C0 error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_ErrorIRQ_Handler(void)
{
    /* no acknowledge received */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_AERR)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBALT)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBALT);
    }

    /* bus timeout in SMBus mode */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBTO)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBTO);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_OUERR)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_OUERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_LOSTARB)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_LOSTARB);
    }

    /* bus error */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BERR)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
    }

    /* CRC value doesn't match */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_PECERR)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_PECERR);
    }

    /* disable the error interrupt */
    i2c_interrupt_disable(I2C0,I2C_CTL1_ERRIE | I2C_CTL1_BUFIE | I2C_CTL1_EVIE);
}
