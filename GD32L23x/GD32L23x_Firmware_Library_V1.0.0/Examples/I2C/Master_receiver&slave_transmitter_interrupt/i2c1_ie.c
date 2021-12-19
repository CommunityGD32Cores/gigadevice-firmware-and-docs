/*!
    \file    I2C1_IE.c
    \brief   I2C1 slave transmitter interrupt program

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "i2c_ie.h"

/*!
    \brief      handle I2C1 event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c1_event_irq_handler(void)
{
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_ADDSEND)) {
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_ADDSEND);
        /* clear I2C_TDATA register */
        I2C_STAT(I2C1) |= I2C_STAT_TBE;
    } else if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_STPDET)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_STPDET);
        i2c_interrupt_disable(I2C1, I2C_INT_ERR | I2C_INT_STPDET | I2C_INT_ADDM | I2C_INT_TI);
    } else if((i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_TI))) {
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
void i2c1_error_irq_handler(void)
{
    /* bus error */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_BERR)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_BERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_LOSTARB)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_LOSTARB);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_OUERR)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_OUERR);
    }

    /* PEC error */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_PECERR)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_PECERR);
    }

    /* timeout error */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_TIMEOUT)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_TIMEOUT);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_SMBALT)) {
        i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_SMBALT);
    }

    /* disable the I2C1 interrupt */
    i2c_interrupt_disable(I2C1, I2C_INT_ERR | I2C_INT_STPDET | I2C_INT_ADDM | I2C_INT_TI);
}
