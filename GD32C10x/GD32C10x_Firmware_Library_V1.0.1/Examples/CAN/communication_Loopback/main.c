/*!
    \file    main.c
    \brief   CAN loopback communication in normal mode
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#include "gd32c10x.h"
#include <stdio.h>
#include "gd32c10x_eval.h"

/* select CAN */
#define DEV_CAN0_USED
//#define DEV_CAN1_USED

#ifdef DEV_CAN0_USED
#define CANX    CAN0
#elif defined DEV_CAN1_USED
#define CANX    CAN1
#else
    #error ("DEV_CANx_USED should be defined");
#endif /* DEV_CAN0_USED */

#define DEV_CAN_LOOPBACK_ID          0x0000
#define DEV_CAN_LOOPBACK_MASK        0x0000
/* config CAN baud rate to 500K Hz (range from 1Hz to 1MHz)*/
#define DEV_CAN_BAUD_RATE    125000

volatile ErrStatus g_test_flag;

void bsp_board_config(void)
{
    /* configure leds */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      initialize CAN and filter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_loopback_init(void)
{
    can_parameter_struct can_parameter;
    uint32_t temp;
    
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    
    /* initialize CAN register */
    can_deinit(CANX);
    
    /* initialize CAN */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_LOOPBACK_MODE;
    can_init(CANX, &can_parameter);
    
    /* configure baudrate to 125kbps */
    can_frequency_set(CANX, DEV_CAN_BAUD_RATE);
    /* initialize filter */
#ifdef  DEV_CAN0_USED
    /* CAN0 filter number */
    temp = 0;
#else
    /* CAN1 filter number */
    temp = 15;
#endif /* DEV_CAN0_USED */

    /* initialize filter */   
    can1_filter_start_bank(14);
    can_filter_mask_mode_init(DEV_CAN_LOOPBACK_ID, DEV_CAN_LOOPBACK_MASK, CAN_STANDARD_FIFO1, temp);
}

/*!
    \brief      function for CAN loopback communication
    \param[in]  none
    \param[out] none
    \retval     ErrStatus
*/
void can_loopback_test(void)
{
    can_trasnmit_message_struct transmit_message;
    can_receive_message_struct  receive_message;
    uint32_t timeout = 0xFFFF;
    uint8_t transmit_mailbox = 0;

    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);
    /* initialize transmit message */
    transmit_message.tx_sfid = DEV_CAN_LOOPBACK_ID;
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
    transmit_message.tx_dlen = 2;
    transmit_message.tx_data[0] = 0xAB;
    transmit_message.tx_data[1] = 0xCD;
    
    /* transmit message */
    transmit_mailbox = can_message_transmit(CANX, &transmit_message);
    /* waiting for transmit completed */
    while((CAN_TRANSMIT_OK != can_transmit_states(CANX, transmit_mailbox)) && (0 != timeout)){
        timeout--;
    }
    timeout = 0xFFFF;
    /* waiting for receive completed */
    while((can_receive_message_length_get(CANX, CAN_FIFO1) < 1) && (0 != timeout)){
        timeout--; 
    }
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &receive_message);
    can_message_receive(CANX, CAN_FIFO1, &receive_message);
    
    /* check the receive message */
    if((DEV_CAN_LOOPBACK_ID == receive_message.rx_sfid) && (CAN_FF_STANDARD == receive_message.rx_ff)
       && (2 == receive_message.rx_dlen) && (0xCDAB == (receive_message.rx_data[1]<<8|receive_message.rx_data[0]))){
        /* loopback test is success */
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
    }else{
        /* loopback test is failed */
        gd_eval_led_on(LED4);
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    bsp_board_config();
    /* initialize CAN */
    can_loopback_init();
    /* loopback of polling */
    can_loopback_test();

    while (1);
}
