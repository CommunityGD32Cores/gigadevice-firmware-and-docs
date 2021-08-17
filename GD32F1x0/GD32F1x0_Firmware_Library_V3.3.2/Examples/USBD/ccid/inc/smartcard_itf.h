/*!
    \file    smartcard_itf.h
    \brief   smartcard interface driver

    \version 2020-07-23, V3.0.0, firmware for GD32F1x0
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

#ifndef __SMARTCARD_ITF_H
#define __SMARTCARD_ITF_H

#include "ccid_cmd.h"

typedef struct
{
    uint8_t voltage;  /* voltage for the card already selected */
    uint8_t USART_GuardTime;
    uint8_t SC_A2R_FiDi;
    uint8_t SC_hostFiDi;
    uint8_t USART_DefaultGuardTime;
    uint32_t USART_BaudRate;
}sc_param_t;

#pragma pack(1)
typedef struct _Protocol0_DataStructure_t
{
    uint8_t bmFindexDindex;
    uint8_t bmTCCKST0;
    uint8_t bGuardTimeT0;
    uint8_t bWaitingIntegerT0;
    uint8_t bClockStop;
}protocol0_data_t;
#pragma pack()

extern sc_param_t sc_param;
extern protocol0_data_t protocol0_data;

#define MAX_EXTRA_GUARD_TIME         (0xFFU - DEFAULT_EXTRA_GUARDTIME)

/* following macros are used for SC_XferBlock command */
#define XFER_BLK_SEND_DATA           1U /* command is for issuing the data */
#define XFER_BLK_RECEIVE_DATA        2U /* command is for receiving the data */
#define XFER_BLK_NO_DATA             3U /* command type is no data exchange */

/* function declarations */
/* manages the warm and cold reset and get the answer to reset from ICC */
void sc_answer_to_reset(uint8_t vol);
/* provides the value of scstate variable */
uint8_t sc_get_state (void);
/* set the value of scstate variable */
void sc_set_state(uint8_t scState);
/* saves the voltage value to be saved for further usage */
void sc_save_voltage(uint8_t vol);
/* save the A2R parameters for further usage */
void sc_update_params(void);
/* initialize the parameters structures */
void sc_init_params(void);
/* set the parameters for CCID/USART interface */
uint8_t sc_set_params(protocol0_data_t* pPtr);
/* sc_execute_escape function from the host */
uint8_t sc_execute_escape(uint8_t* escapePtr, uint32_t escapeLen, uint8_t* responseBuff, uint16_t* responseLen);
/* define clock status request from the host */
uint8_t sc_set_clock(uint8_t bClockCommand);
/* define transfer block configuration */
uint8_t sc_xfer_block(usb_dev *udev, uint8_t* ptrBlock, uint32_t blockLen, uint16_t expectedLen);
/* Class specific request from the host to provide supported clock freq */
uint8_t sc_request_get_clock_freq(uint8_t* pbuf, uint16_t* len);
/*  Class specific request from the host to provide supported data rates */
uint8_t sc_request_get_data_rates(uint8_t* pbuf, uint16_t* len);
/* Class specific request from the host to provide supported data rates */
uint8_t sc_T0_APDU(uint8_t bmChanges, uint8_t bClassGetResponse, uint8_t bClassEnvelope);
/* mechanical function being requested by host */
uint8_t sc_mechanical(uint8_t bFunction);
/* set the clock and data rate of the interface */
uint8_t sc_set_DataRate_and_ClockFreq(uint32_t dwClockFrequency, uint32_t dwDataRate);
/* process the secure command */
uint8_t sc_secure(uint32_t dwLength, uint8_t bBWI, uint16_t wLevelParameter, uint8_t* pbuf, uint32_t* returnLen);

#endif /* __SMARTCARD_ITF_H */
