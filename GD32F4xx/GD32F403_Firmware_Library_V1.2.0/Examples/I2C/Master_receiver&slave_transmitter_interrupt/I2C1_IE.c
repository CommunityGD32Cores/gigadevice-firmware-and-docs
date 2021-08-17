/*!
    \file  I2C1_IE.c
    \brief I2C1 slave transmitter interrupt program
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.2, firmware for GD32F403
*/

#include "gd32f403_i2c.h"
#include "I2C_IE.h"

uint32_t event2;

/*!
    \brief      handle I2C1 event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C1_EventIRQ_Handler(void)
{
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_ADDSEND)){
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_ADDSEND);
    }else if((i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_TBE))&&(!i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_AERR))){
        /* send a data byte */
        i2c_data_transmit(I2C1, *i2c_txbuffer++);
    }
}

/*!
    \brief      handle I2C1 error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C1_ErrorIRQ_Handler(void)
{
    /* no acknowledge received */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_AERR)){
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_AERR);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_SMBALT)){
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_SMBALT);
    }

    /* bus timeout in SMBus mode */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_SMBTO)){
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_SMBTO);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_OUERR)){
       i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_OUERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_LOSTARB)){
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_LOSTARB);
    }

    /* bus error */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_BERR)){
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_BERR);
    }

    /* CRC value doesn't match */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_PECERR)){
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_PECERR);
    }

    /* disable the error interrupt */
    i2c_interrupt_disable(I2C1, I2C_INT_ERR | I2C_INT_BUF | I2C_INT_EV);
}
