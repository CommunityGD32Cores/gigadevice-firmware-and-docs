/*!
    \file  gd32f1x0_it.c
    \brief interrupt service routines

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32f1x0_it.h"
#include "gd32f190r_eval.h"

/* array1 transmission */
uint8_t senddata1[2] = {0xA5,0x5A};
/* array2 transmission */
uint8_t senddata2[2] = {0x5A,0xA5};
/* receive array buffer  */
uint8_t rcvdata[10];
/* sent data pointer */
uint8_t *transdata;
__IO uint8_t rcvstatus = 0;
uint8_t send_inc = 0, rcv_inc = 0;
uint16_t senderrorcode = 0;
uint16_t rcverrorcode = 0;
uint8_t bytenum = 0;

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
}

/*!
    \brief      this function handles EXTI0_1 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_1_IRQHandler(void)
{
    if(RESET != exti_flag_get(WAKEUP_KEY_EXTI_LINE)){
        /* Tamper key press */
        bytenum = 2;
        /* send CEC address */
        cec_data_send(0x12);
        /* start transmission */
        transdata = senddata2;
        /* start transmission */
        cec_transmission_start();
        exti_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles EXTI4_15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI4_15_IRQHandler(void)
{
    if(RESET != exti_flag_get(TAMPER_KEY_EXTI_LINE) ){
        /* Tamper key press */
        bytenum = 2;
        /* send CEC address */
        cec_data_send(0x12);
        /* start transmission */
        transdata = senddata1;
        /* start transmission */
        cec_transmission_start();
        exti_flag_clear(TAMPER_KEY_EXTI_LINE);
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
    /* check if a reception error occured */
    if(cec_interrupt_flag_get(CEC_INTF_RO | CEC_INTF_BRE | CEC_INTF_BPSE | CEC_INTF_BPLE | CEC_INTF_RAE)){
        rcverrorcode = CEC_INTF;
        cec_interrupt_flag_clear(CEC_INTF_RO | CEC_INTF_BRE | CEC_INTF_BPSE | CEC_INTF_BPLE | CEC_INTF_RAE);
        rcvstatus  = 2;
    }
    /* receive data */
    if(cec_interrupt_flag_get(CEC_INTF_BR)){
        rcvdata[rcv_inc] = cec_data_receive();
        rcv_inc++;
        cec_interrupt_flag_clear(CEC_INTF_BR);
    }
    /* check if the byte received is the last one of the message */
    if(cec_interrupt_flag_get(CEC_INTF_REND)){
        rcvstatus = 1;
        rcv_inc = 0;
        cec_interrupt_flag_clear(CEC_INTF_REND);
    }

    /* check if a transmission error occurred */
    if(cec_interrupt_flag_get(CEC_INTF_ARBF | CEC_INTF_TU | CEC_INTF_TERR | CEC_INTF_TAERR)){
        senderrorcode = CEC_INTF;
        cec_interrupt_flag_clear(CEC_INTF_ARBF | CEC_INTF_TU | CEC_INTF_TERR | CEC_INTF_TAERR);
    }
  
    /* check if end of message bit is set in the data to be transmitted */
    if(cec_interrupt_flag_get(CEC_INTF_TEND)){
        cec_interrupt_flag_clear(CEC_INTF_TEND | CEC_INTF_TBR);
        send_inc = 0;
    }else if(cec_interrupt_flag_get(CEC_INTF_TBR)){
        /* set EOM bit if the byte to be transmitted is the last one of the senddata */
        if((bytenum - 1) == send_inc++){
            cec_transmission_end();
            cec_data_send(*transdata++);
        }else{
            /* send the byte in the transdata */
            cec_data_send(*transdata++);
        }
        cec_interrupt_flag_clear(CEC_INTF_TBR);
    }
}
