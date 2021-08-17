/*!
    \file    smartcard_itf.c
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

#include "smartcard_itf.h"

/* APDU transport structures */
sc_param_t sc_param;
sc_APDU_commands sc_adpu;
sc_APDU_response sc_response;
protocol0_data_t protocol0_data;

/* state machine for the smartcard interface */
sc_state_enum scstate = SC_POWER_OFF;

/*!
    \brief      manages the warm and cold reset and get the answer to reset from ICC 
    \param[in]  vol: voltage required by host 
    \param[out] none
    \retval     none
*/
#if defined (__ICCARM__)      /*!< IAR compiler */
    #pragma optimize = none
#endif /* __ICCARM */
void sc_answer_to_reset(uint8_t vol)
{
    scstate = SC_POWER_ON;

    sc_adpu.header.cla = 0x0U;
    sc_adpu.header.ins = SC_GET_A2R;
    sc_adpu.header.p1 = 0x0U;
    sc_adpu.header.p2 = 0x0U;
    sc_adpu.body.lc = 0x0U;

    /* power ON the card */
    sc_power_cmd(ENABLE);

    /* configure the voltage, even if IO is still not configured */
    sc_voltage_config(vol);

    while((SC_ACTIVE_ON_T0 != scstate) && (SC_NO_INIT != scstate)){
        /* if either the card has become active or become De-Active */
        sc_handler(&scstate, &sc_adpu, &sc_response);
    }

    if(SC_ACTIVE_ON_T0 == scstate){
        sc_update_params();

        /* apply the procedure type selection (PTS) */
        sc_PTS_config();

        /* save voltage for future use */
        sc_save_voltage(vol);   
    }

    /* insert delay for smartcard clock resynchronization */
    for(uint16_t i = 0U; i < 10000U; i++){
        for(uint16_t j = 0U; j < 1000U; j++);
    }
}

/*!
    \brief      provides the value of scstate variable 
    \param[in]  none
    \param[out] none
    \retval     smartcard state
*/
uint8_t sc_get_state (void)
{
    return scstate;
}

/*!
    \brief      set the value of scstate variable
    \param[in]  state: smartcard state
    \param[out] none
    \retval     none
*/
void sc_set_state (uint8_t state)
{
    scstate = (sc_state_enum)state;
}

/*!
    \brief      saves the voltage value to be saved for further usage 
    \param[in]  vol: voltage value to be saved for further usage 
    \param[out] none
    \retval     none
*/
void sc_save_voltage(uint8_t vol)
{
    sc_param.voltage = vol;
}

/*!
    \brief      save the A2R parameters for further usage 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sc_update_params(void)
{
    /* FI, the reference to a clock rate conversion factor over the bits b8 to b5
       DI, the reference to a baud rate adjustment factor over the bits b4 to bl */
    sc_param.SC_A2R_FiDi = sc_A2R.t[0];
    sc_param.SC_hostFiDi = sc_A2R.t[0];

    protocol0_data.bmFindexDindex= sc_A2R.t[0];
}

/*!
    \brief      initialize the parameters structures
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sc_init_params(void)
{
    /* - FI, the reference to a clock rate conversion factor over the bits b8 to b5
       - DI, the reference to a baud rate adjustment factor over the bits b4 to bl */
    sc_param.SC_A2R_FiDi = DEFAULT_FIDI;
    sc_param.SC_hostFiDi = DEFAULT_FIDI;

    protocol0_data.bmFindexDindex = DEFAULT_FIDI;

    /* place holder, ignored */
    /* 0 = Direct, first byte of the ICC's ATR data. */
    protocol0_data.bmTCCKST0 = DEFAULT_T01CONVCHECKSUM;  

    /* extra GuardTime = 0 ETU */
    protocol0_data.bGuardTimeT0 = DEFAULT_EXTRA_GUARDTIME; 
    protocol0_data.bWaitingIntegerT0 = DEFAULT_WAITINGINTEGER;

    /* stopping the clock is not allowed */
    protocol0_data.bClockStop = 0U; 
}

/*!
    \brief      set the parameters for CCID/USART interface 
    \param[in]  pPtr: pointer to buffer containing the parameters to be set in USART
    \param[out] none
    \retval     status value
*/
uint8_t sc_set_params(protocol0_data_t* pPtr)
{
    uint32_t fi_new;
    uint32_t di_new;
    uint16_t guardTime;

    guardTime = pPtr->bGuardTimeT0;

    if(guardTime > MAX_EXTRA_GUARD_TIME){
        return SLOTERROR_BAD_GUARDTIME;
    }

    fi_new = pPtr->bmFindexDindex;
    di_new = pPtr->bmFindexDindex;

    /* check for the FIDI value set by host */
    di_new &= (uint8_t)0x0F;

    if(0U == sc_get_DTable_value(di_new)){
        return SLOTERROR_BAD_FIDI;
    }

    fi_new >>= 4U;
    fi_new &= (uint8_t)0x0FU; 

    if(0U == sc_get_DTable_value(fi_new)){
        return SLOTERROR_BAD_FIDI;
    }

    /* put total GuardTime in USART settings */
    usart_guard_time_config(SC_USART, (uint8_t)(guardTime + DEFAULT_EXTRA_GUARDTIME));

    /* save extra GuardTime value */
    protocol0_data.bGuardTimeT0 = guardTime; 

    protocol0_data.bmTCCKST0 = pPtr->bmTCCKST0;
    protocol0_data.bClockStop = pPtr->bClockStop;

    /* save new bmFindexDindex */
    sc_param.SC_hostFiDi = pPtr->bmFindexDindex;
    sc_PTS_config();

    protocol0_data.bmFindexDindex = pPtr->bmFindexDindex;

    return SLOT_NO_ERROR;
}

/*!
    \brief      sc_execute_escape function from the host. This is user implementable.
    \param[in]  ptrEscape: pointer to buffer containing the Escape data
    \param[in]  escapeLen: length of escaped data
    \param[in]  responseBuff: pointer containing escape buffer response
    \param[in]  responseLen: length of escape response buffer
    \param[out] none
    \retval     status value
*/
uint8_t sc_execute_escape(uint8_t* ptrEscape, uint32_t escapeLen, uint8_t* responseBuff, uint16_t* responseLen)
{
    uint32_t idx;

    /* manufacturer specific implementation */

    /* do a loop back for example */
    /* to be modified for end application */
    for(idx =0U; idx < escapeLen; idx++){
        *responseBuff++ = *ptrEscape++;
    }

    *responseLen = escapeLen;   /* To be Modified for end application */

    return SLOT_NO_ERROR;
}

/*!
    \brief      this function to define clock status request from the host. 
    \param[in]  bClockCommand: clock status from the host 
    \param[out] none
    \retval     status value
*/
uint8_t sc_set_clock(uint8_t bClockCommand) 
{
    /* bClockCommand: 00h restarts Clock
                      01h stops Clock in the state shown in the bClockStop 
                      field of the PC_to_RDR_SetParameters command 
                      and RDR_to_PC_Parameters message. */
    if(0U == bClockCommand){
        /* 00h restarts clock : since clock is always running, pass this command */
        return SLOT_NO_ERROR;
    }else if(1U == bClockCommand){
        return SLOTERROR_BAD_CLOCKCOMMAND;
    }

    return SLOTERROR_CMD_NOT_SUPPORTED;
}

/*!
    \brief      this function from the host. This is user implementable
    \param[in]  udev: pointer to USB device instance
    \param[in]  ptrBlock: pointer containing the data from host 
    \param[in]  blockLen: length of block data for the data transfer 
    \param[in]  expectedLen: expected length of data transfer 
    \param[out] none
    \retval     status value
*/
uint8_t sc_xfer_block (usb_dev *udev, uint8_t* ptrBlock, uint32_t blockLen, uint16_t expectedLen)
{
    uint32_t index;
    uint32_t cmdType;

    usb_ccid_handler *ccid = (usb_ccid_handler *)udev->class_data;

    sc_adpu.header.cla = *ptrBlock++;
    sc_adpu.header.ins = *ptrBlock++;
    sc_adpu.header.p1 = *ptrBlock++;
    sc_adpu.header.p2 = *ptrBlock++;

    /* check for the command types from the host */
    if((SC_SELECT_FILE == sc_adpu.header.ins) || \
        (SC_ENABLE == sc_adpu.header.ins) || \
        (SC_VERIFY == sc_adpu.header.ins)){
        /* set the flag as 1 for commands which issue data to the card */
        cmdType = XFER_BLK_SEND_DATA;
    }else if((SC_GET_RESPONCE == sc_adpu.header.ins) || \
            (SC_READ_BINARY == sc_adpu.header.ins)){
        cmdType = XFER_BLK_RECEIVE_DATA;
    }else{
        cmdType = XFER_BLK_NO_DATA;
    }

    if(0x04U == blockLen){
        /* case: no data to exchange with ICC, only header sent by Host= 4Bytes
           Command TPDU = CLA INS P1 P2, the CCID is responsible to add P3 =00h */
        sc_adpu.body.lc = 0U;
        sc_adpu.body.le = 0U;
    }

    /* process the commands based on their types from the host */
    if(XFER_BLK_RECEIVE_DATA == cmdType){
        /* form 2, data expected from ICC:
          Command TPDU = CLA INS P1 P2 Le, Le=P3 from 00h to FFh (00h means 100h)
          Response TPDU = Data(Le) SW1 SW2, Data(Le) is for the Le data received from 
          the ICC or empty if ICC rejects the command. */
        sc_adpu.body.le = *ptrBlock++;

        /* To be compatible with lower layer, reset to 0 */
        sc_adpu.body.lc = 0x0U;  

        sc_handler(&scstate, &sc_adpu, &sc_response);

        for(index = 0U; index < (sc_adpu.body.le); index++){
            /* copy the ATR to the response buffer */
            ccid->CCID_bulkin_data.abData[index] = sc_response.data[index];
        }

        ccid->CCID_bulkin_data.abData[index] = sc_response.sw1;
        index++;
        ccid->CCID_bulkin_data.abData[index] = sc_response.sw2;

        /* response len = expected len + sw1, sw2*/
        ccid->CCID_bulkin_data.dwLength= (sc_adpu.body.le) + 2;
    }else{
        /* cmdType == XFER_BLK_SEND_DATA, XFER_BLK_NO_DATA*/
        /* form 3, data are to be sent to the ICC:
           Command TPDU = CLA INS P1 P2 Lc Data(Lc), Lc=P3 from 01h to FFh and Data(Lc) 
           for the Lc data to send to the ICC.
           Response TPDU = SW1 SW2 */
        sc_adpu.body.lc = *ptrBlock++;

        /* To be compatible with Lower Layer, reset to 0 */
        sc_adpu.body.le = 0U;  

        for(index = 0U; index < sc_adpu.body.lc; index++){
            sc_adpu.body.data[index] = *ptrBlock++;
        }

        while(index < LC_MAX) {
            sc_adpu.body.data[index++] = 0U;
        }

        /* call the API from lower layer to process the command */
        sc_handler(&scstate, &sc_adpu, &sc_response);
        ccid->CCID_bulkin_data.abData[0] = sc_response.sw1;
        ccid->CCID_bulkin_data.abData[1] = sc_response.sw2;
        ccid->CCID_bulkin_data.dwLength = 2U;
    }

    return SLOT_NO_ERROR;
}

/*!
    \brief      Class specific request from the host to provide supported clock freq. 
                This is Optional function & user implementable.
    \param[in]  pbuf: pointer to be updated by function which points to the supported clock frequencies 
    \param[in]  len: length of data to be transferred to host
    \param[out] none
    \retval     status value
*/
uint8_t sc_request_get_clock_freq(uint8_t* pbuf, uint16_t* len)
{
    /* user have to fill the pbuf with the GetClockFrequency data buffer */
    *len = 0U;

    return 0U;
}

/*!
    \brief      Class specific request from the host to provide supported data rates
                This is Optional function & user implementable 
    \param[in]  pbuf: pointer to be updated by function which points to the supported clock frequencies 
    \param[in]  len: length of data to be transferred to host
    \param[out] none
    \retval     status value
*/
uint8_t sc_request_get_data_rates(uint8_t* pbuf, uint16_t* len)
{
    /* user have to fill the pbuf with the GetDataRates data buffer */
    *len = 0U;

    return 0U;
}

/*!
    \brief      Class specific request from the host to provide supported data rates.
                This is Optional function & user implementable.
    \param[in]  bmChanges: value specifying which parameter is valid in command 
                among next bClassGetResponse, bClassEnvelope
    \param[in]  bClassGetResponse: Value to force the class byte of the header in 
                a Get Response command.
    \param[in]  bClassEnvelope: Value to force the class byte of the header in a Envelope command.
    \param[out] none
    \retval     status value
*/
uint8_t sc_T0_APDU(uint8_t bmChanges, uint8_t bClassGetResponse, uint8_t bClassEnvelope)
{
    /* user have to fill the pbuf with the GetDataRates data buffer */
    if(0U == bmChanges){
        /* bit cleared indicates that the associated field is not significant and 
           that default behaviour defined in CCID class descriptor is selected */
        return SLOT_NO_ERROR;
    }

    return SLOTERROR_CMD_NOT_SUPPORTED;
}

/*!
    \brief      mechanical function being requested by host 
    \param[in]  bFunction: value corresponds to the mechanical function being requested by host
    \param[out] none
    \retval     status value
*/
uint8_t sc_mechanical(uint8_t bFunction)
{
    return SLOTERROR_CMD_NOT_SUPPORTED;
}

/*!
    \brief      set the clock and data rate of the interface
                This is optional function & user implementable
    \param[in]  dwClockFrequency: the value of clock frequency
    \param[in]  dwDataRate: data rate
    \param[out] none
    \retval     status value
*/
uint8_t sc_set_DataRate_and_ClockFreq(uint32_t dwClockFrequency, uint32_t dwDataRate)
{
    /* user have to fill the pbuf with the GetDataRates data buffer */
    if((DEFAULT_DATA_RATE == dwDataRate) && (DEFAULT_CLOCK_FREQ == dwClockFrequency)){
        return SLOT_NO_ERROR;
    }

    return SLOTERROR_CMD_NOT_SUPPORTED;
}

/*!
    \brief      process the secure command
                This is optional function & user implementable
    \param[in]  dwLength: length of data from the host 
    \param[in]  bBWI: Block Waiting Timeout sent by host
    \param[in]  wLevelParameter: Parameters sent by host
    \param[in]  pbuf: buffer containing the data
    \param[out] returnLen: Length of data expected to return
    \retval     status value
*/
uint8_t sc_secure(uint32_t dwLength, uint8_t bBWI, uint16_t wLevelParameter, 
                  uint8_t* pbuf, uint32_t* returnLen)
{
    *returnLen = 0U;

    return SLOTERROR_CMD_NOT_SUPPORTED;
}
