/*!
    \file    gd32f3x0_it.c
    \brief   interrupt service routines
    
    \version 2017-06-28, V1.0.0, demo for GD32F3x0
    \version 2019-06-01, V2.0.0, demo for GD32F3x0
    \version 2021-05-31, V2.1.0, demo for GD32F3x0
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

#include "gd32f3x0_it.h"
#include "systick.h"

/* array1 transmission */
uint8_t senddata1[2] = {0xA5,0x5A};
/* array2 transmission */
uint8_t senddata2[2] = {0x5A,0xA5};
/* receive array buffer  */
uint8_t rcvdata[10];
/* sent data pointer */
uint8_t *transdata;
__IO uint8_t rcvstatus = 0;
__IO uint8_t send_inc = 0, rcv_inc = 0;
uint16_t senderrorcode = 0;
uint16_t rcverrorcode = 0;
__IO uint8_t bytenum = 0;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

/*!
    \brief      this function handles EXTI4_15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI4_15_IRQHandler(void)
{
    if(exti_flag_get(EXTI_7) != RESET){
        /* tamper key press */
        bytenum = 2;
        /* send CEC address */
        cec_data_send(0x12);
        transdata = senddata1;
        /* start transmission */
        cec_transmission_start();
        exti_flag_clear(EXTI_7);
    }else if(exti_flag_get(EXTI_13) != RESET){
        /* Tamper key press */
        bytenum = 2;
        /* send CEC address */
        cec_data_send(0x12);
        /* start transmission */
        transdata = senddata2;
        /* start transmission */
        cec_transmission_start();
        exti_flag_clear(EXTI_13);
    }
}

/*!
    \brief      this function handles CEC interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CEC_IRQHandler(void)
{
    uint8_t tmp_bytenum;
    tmp_bytenum = bytenum;
    /********************** Reception *******************************************/
    /* check if a reception error occured */
    if(cec_interrupt_flag_get(CEC_INT_FLAG_RO | CEC_INT_FLAG_BRE | CEC_INT_FLAG_BPSE | CEC_INT_FLAG_BPLE | CEC_INT_FLAG_RAE)){
        rcverrorcode = CEC_INTF;
        cec_interrupt_flag_clear(CEC_INT_FLAG_RO | CEC_INT_FLAG_BRE | CEC_INT_FLAG_BPSE | CEC_INT_FLAG_BPLE | CEC_INT_FLAG_RAE);
        rcvstatus  = 2;
    }
    /* receive data */
    if(cec_interrupt_flag_get(CEC_INT_FLAG_BR)){
        rcvdata[rcv_inc] = cec_data_receive();
        rcv_inc++;
        cec_interrupt_flag_clear(CEC_INT_FLAG_BR);
    }
    /* check if the byte received is the last one of the message */
    if(cec_interrupt_flag_get(CEC_INT_FLAG_REND)){
        rcvstatus = 1;
        rcv_inc = 0;
        cec_interrupt_flag_clear(CEC_INT_FLAG_REND);
    }

    /********************** Transmission ****************************************/
    /* check if a transmission error occurred */
    if(cec_interrupt_flag_get(CEC_INT_FLAG_ARBF | CEC_INT_FLAG_TU | CEC_INT_FLAG_TERR | CEC_INT_FLAG_TAERR)){
        senderrorcode = CEC_INTF;
        cec_interrupt_flag_clear(CEC_INT_FLAG_ARBF | CEC_INT_FLAG_TU | CEC_INT_FLAG_TERR | CEC_INT_FLAG_TAERR);
    }
  
    /* check if end of message bit is set in the data to be transmitted */
    if(cec_interrupt_flag_get(CEC_INT_FLAG_TEND)){
        cec_interrupt_flag_clear(CEC_INT_FLAG_TEND| CEC_INT_FLAG_TBR);
        send_inc = 0;
    }else if(cec_interrupt_flag_get(CEC_INT_FLAG_TBR)){
        /* set EOM bit if the byte to be transmitted is the last one of the senddata */
        if((tmp_bytenum - 1) == send_inc++){
            cec_transmission_end();
            cec_data_send(*transdata++);
        }else{
            /* send the byte in the transdata */
            cec_data_send(*transdata++);
        }
        cec_interrupt_flag_clear(CEC_INT_FLAG_TBR);
    }
}
