/*!
    \file    gd32vf103_it.c
    \brief   interrupt service routines
    
    \version 2019-06-05, V1.0.0, firmware for GD32VF103
    \version 2020-08-04, V1.1.0, firmware for GD32VF103
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

#include "gd32vf103_it.h"

extern ErrStatus test_flag_interrupt;

/*!
    \brief      this function handles CAN0 RX1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CAN0_RX1_IRQHandler(void)
{
    can_receive_message_struct receive_message;
    /* initialize receive message */
    receive_message.rx_sfid = 0x00;
    receive_message.rx_efid = 0x00;
    receive_message.rx_ff = 0;
    receive_message.rx_dlen = 0;
    receive_message.rx_fi = 0;
    receive_message.rx_data[0] = 0x00;
    receive_message.rx_data[1] = 0x00;
    
    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO1, &receive_message);
    
    if((0x1234 == receive_message.rx_efid) && (CAN_FF_EXTENDED == receive_message.rx_ff)
        && (2 == receive_message.rx_dlen) && (0xCADE == (receive_message.rx_data[1]<<8|receive_message.rx_data[0]))){
        test_flag_interrupt = SUCCESS; 
    }else{
        test_flag_interrupt = ERROR; 
    }
}

/*!
    \brief      this function handles CAN1 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CAN1_RX1_IRQHandler(void)
{
    can_receive_message_struct receive_message;
    /* initialize receive message */
    receive_message.rx_sfid = 0x00;
    receive_message.rx_efid = 0x00;
    receive_message.rx_ff = 0;
    receive_message.rx_dlen = 0;
    receive_message.rx_fi = 0;
    receive_message.rx_data[0] = 0x00;
    receive_message.rx_data[1] = 0x00;

    /* check the receive message */
    can_message_receive(CAN1, CAN_FIFO1, &receive_message);

    if((0x1234 == receive_message.rx_efid) && (CAN_FF_EXTENDED == receive_message.rx_ff)
        && (2 == receive_message.rx_dlen) && (0xCADE == (receive_message.rx_data[1]<<8|receive_message.rx_data[0]))){
        test_flag_interrupt = SUCCESS; 
    }else{
        test_flag_interrupt = ERROR; 
    }
}
