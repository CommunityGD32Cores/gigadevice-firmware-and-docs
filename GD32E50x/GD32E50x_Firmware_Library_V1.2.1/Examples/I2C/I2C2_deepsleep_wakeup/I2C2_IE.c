/*!
    \file    I2C2_IE.c
    \brief   wakeup from deepsleep interrupt program

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x_i2c.h"
#include "I2C_IE.h"
#include "gd32e507z_eval.h"

/*!
    \brief      handle I2C2 wakeup interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C2_WakeupIRQ_Handler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_20)) {
        exti_interrupt_flag_clear(EXTI_20);
    }
}

/*!
    \brief      handle I2C2 event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C2_EventIRQ_Handler(void)
{
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_ADDSEND)){
        /* clear the ADDSEND bit */
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_ADDSEND);
    }else if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_RBNE)){
        /* if reception data register is not empty ,I2C2 will read a data from I2C2_RDATA */
        *i2c_rxbuffer++ = i2c_data_receive(I2C2);
    }else if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_STPDET)){
        /* clear STPDET interrupt flag */
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_STPDET);
        /* disable I2C2 interrupt */
        i2c2_interrupt_disable(I2C2, I2C2_INT_ERR | I2C2_INT_STPDET | I2C2_INT_ADDM | I2C2_INT_RBNE);
    }
}

/*!
    \brief      handle I2C2 error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C2_ErrorIRQ_Handler(void)
{
    /* bus error */
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_BERR)){
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_BERR);
    }

    /* arbitration lost */
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_LOSTARB)){
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_LOSTARB);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_OUERR)){
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_OUERR);
    }

    /* PEC error */
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_PECERR)){
       i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_PECERR);
    }

    /* timeout error */
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_TIMEOUT)){
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_TIMEOUT);
    }

    /* SMBus alert */
    if(i2c2_interrupt_flag_get(I2C2, I2C2_INT_FLAG_SMBALT)){
        i2c2_interrupt_flag_clear(I2C2, I2C2_INT_FLAG_SMBALT);
    }

    /* disable the I2C2 interrupt */
    i2c2_interrupt_disable(I2C2, I2C2_INT_ERR | I2C2_INT_STPDET | I2C2_INT_ADDM | I2C2_INT_RBNE);
}
