/*!
    \file    ccid_itf.h
    \brief   functions prototypes for USB CCID

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

#ifndef CCID_ITF_H
#define CCID_ITF_H

#include "ccid_core.h"

/* bulk-only command block wrapper */
#define ABDATA_SIZE                             261U           /*!< abData size */
#define CCID_CMD_HEADER_SIZE                    10U            /*!< CCID command header size */
#define CCID_RESPONSE_HEADER_SIZE               10U            /*!< CCID response header size */

#define CCID_INT_BUFF_SIZ                       2U             /*!< CCID INT buff size */

#define CARD_SLOT_FITTED                        1U             /*!< card slot fitted */
#define CARD_SLOT_REMOVED                       0U             /*!< card slot removed */

#define BULK_MAX_PACKET_SIZE                    64U            /*!< bulk maximum packet size */
#define CCID_IN_EP_SIZE                         64U            /*!< CCID IN endpoint size */
#define INT_MAX_PACKET_SIZE                     8U             /*!< INT maximum packet size */
#define CCID_MESSAGE_HEADER_SIZE                10U            /*!< CCID message header size */
#define CCID_NUMBER_OF_SLOTS                    1U             /*!< CCID number of slots */

/* following parameters used in PC_to_RDR_IccPowerOn */
#define VOLTAGE_SELECTION_AUTOMATIC             0x00U          /*!< voltage selection automatic */
#define VOLTAGE_SELECTION_3V                    0x02U          /*!< voltage selection 3V */
#define VOLTAGE_SELECTION_5V                    0x01U          /*!< voltage selection 5V */
#define VOLTAGE_SELECTION_1V8                   0x03U          /*!< voltage selection 1V8 */

#define PC_TO_RDR_ICCPOWERON                    0x62U          /*!< PC to RDR ICC power ON */
#define PC_TO_RDR_ICCPOWEROFF                   0x63U          /*!< PC to RDR ICC power OFF */
#define PC_TO_RDR_GETSLOTSTATUS                 0x65U          /*!< PC to RDR get slot status */
#define PC_TO_RDR_XFRBLOCK                      0x6FU          /*!< PC to RDR transfer block */
#define PC_TO_RDR_GETPARAMETERS                 0x6CU          /*!< PC to RDR get parameters */
#define PC_TO_RDR_RESETPARAMETERS               0x6DU          /*!< PC to RDR reset parameters */
#define PC_TO_RDR_SETPARAMETERS                 0x61U          /*!< PC to RDR set parameters */
#define PC_TO_RDR_ESCAPE                        0x6BU          /*!< PC to RDR escape */
#define PC_TO_RDR_ICCCLOCK                      0x6EU          /*!< PC to RDR ICC clock */
#define PC_TO_RDR_T0APDU                        0x6AU          /*!< PC to RDR T0 APDU */
#define PC_TO_RDR_SECURE                        0x69U          /*!< PC to RDR secure */
#define PC_TO_RDR_MECHANICAL                    0x71U          /*!< PC to RDR mechanical */
#define PC_TO_RDR_ABORT                         0x72U          /*!< PC to RDR abort */
#define PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY  0x73U          /*!< PC to RDR set data rate and clock frequency */

#define RDR_TO_PC_DATABLOCK                     0x80U          /*!< RDR to PC set data block */
#define RDR_TO_PC_SLOTSTATUS                    0x81U          /*!< RDR to PC slot status */
#define RDR_TO_PC_PARAMETERS                    0x82U          /*!< RDR to PC parameters */
#define RDR_TO_PC_ESCAPE                        0x83U          /*!< RDR to PC escape */
#define RDR_TO_PC_DATARATEANDCLOCKFREQUENCY     0x84U          /*!< RDR to PC data rate and clock frequency */

#define RDR_TO_PC_NOTIFYSLOTCHANGE              0x50U          /*!< RDR to PC notify slot change */
#define RDR_TO_PC_HARDWAREERROR                 0x51U          /*!< RDR to PC HW error */

#define OFFSET_INT_BMESSAGETYPE                 0U             /*!< offset INT message type */
#define OFFSET_INT_BMSLOTICCSTATE               1U             /*!< offset INT slotic state */

/* LSb: (0b = no ICC present, 1b = ICC present) */
#define SLOT_ICC_PRESENT                        0x01U

/* MSb : (0b = no change, 1b = change) */
#define SLOT_ICC_CHANGE                         0x02U  

/* CCID bulk transfer state machine */
#define CCID_STATE_IDLE                         0U            /*!< idle state */
#define CCID_STATE_DATA_OUT                     1U            /*!< data out state */
#define CCID_STATE_RECEIVE_DATA                 2U            /*!< receive data */
#define CCID_STATE_SEND_RESP                    3U            /*!< send response */
#define CCID_STATE_DATAIN                       4U            /*!< data IN */
#define CCID_STATE_UNCORRECT_LENGTH             5U            /*!< uncorrect length */

#define DIR_IN                                  0U            /*!< direction IN */
#define DIR_OUT                                 1U            /*!< direction OUT */
#define BOTH_DIR                                2U            /*!< both direction */

#pragma pack(1)
typedef struct {
    uint8_t bMessageType;                       /*!< offset = 0*/
    uint32_t dwLength;                          /*!< offset = 1, the length field (dwLength) is the length
                                                   of the message not including the 10-byte header.*/
    uint8_t bSlot;                              /*!< offset = 5*/
    uint8_t bSeq;                               /*!< offset = 6*/
    uint8_t bSpecific_0;                        /*!< offset = 7*/
    uint8_t bSpecific_1;                        /*!< offset = 8*/
    uint8_t bSpecific_2;                        /*!< offset = 9*/
    uint8_t abData [ABDATA_SIZE];               /*!< offset = 10, for reference, the absolute 
                                                   maximum block size for a TPDU T=0 block is 260 bytes 
                                                   (5 bytes command; 255 bytes data), or for a TPDU T=1 
                                                   block is 259 bytes, or for a short APDU T=1 block is 
                                                   261 bytes, or for an extended APDU T=1 block is 65544 bytes.*/
} ccid_bulkout_data_t; 

typedef struct {
    uint8_t bMessageType;                       /*!< offset = 0*/
    uint32_t dwLength;                          /*!< offset = 1*/
    uint8_t bSlot;                              /*!< offset = 5, same as bulk-out message */
    uint8_t bSeq;                               /*!< offset = 6, same as bulk-out message */
    uint8_t bStatus;                            /*!< offset = 7, slot status */
    uint8_t bError;                             /*!< offset = 8, slot error */
    uint8_t bSpecific;                          /*!< offset = 9*/
    uint8_t abData[ABDATA_SIZE];                /*!< offset = 10*/
    uint16_t u16SizeToSend;                     /*!< size to send */
}ccid_bulkin_data_t; 

#pragma pack()

typedef struct {
    __IO uint8_t SlotStatus;                    /*!< slot status */
    __IO uint8_t SlotStatusChange;              /*!< slot status change */
}ccid_slot_status_t; 

typedef struct {
    __IO uint8_t bAbortRequestFlag;            /*!< abort request flag */
    __IO uint8_t bSeq;                         /*!< CCID sequence */
    __IO uint8_t bSlot;                        /*!< CCID slot */
}usb_ccid_param_t;

typedef struct {
    uint8_t CCID_bulk_state;                                          /*!< CCID bulk state */
    usb_ccid_param_t usb_ccid_param;                                  /*!< CCID parameters struct */
    __IO uint8_t pre_xfer_compl_INT_IN;                               /*!< previous transfer complete INT IN */
    uint32_t usb_message_len;                                         /*!< USB message length */
    uint8_t *pointer_message_buff;                                    /*!< message buff pointer */
    ccid_slot_status_t CCID_slot_status;                              /*!< CCID CCID slot status struct */
    ccid_bulkin_data_t CCID_bulkin_data;                              /*!< CCID bulk IN data struct */
    ccid_bulkout_data_t CCID_bulkout_data;                            /*!< CCID bulk OUT data struct */
    uint8_t bulkout_data_buff[BULK_MAX_PACKET_SIZE];                  /*!< CCID bulk OUT data buff */
    uint8_t interrupt_message_buff[INT_MAX_PACKET_SIZE];              /*!< CCID interrupt message buff */
} usb_ccid_handler;

/* function declarations */
/* initialize the CCID USB layer */
void CCID_init(usb_dev *udev);
/* deinitialize the CCID machine */
void CCID_deinit(usb_dev *udev);
/* handle bulk IN & INT IN data stage */
void CCID_bulk_message_in(usb_dev *udev, uint8_t epnum);
/* process CCID OUT data */
void CCID_bulk_message_out(usb_dev *udev, uint8_t epnum);
/* parse the commands and process command */
void CCID_cmd_decode(usb_dev *udev);
/* prepare the request response to be sent to the host */
void transfer_data_request(usb_dev *udev, uint8_t *buffer, uint16_t len);
/* send the data on BULK-IN EP */
void CCID_response_send_data(usb_dev *udev, uint8_t *buffer, uint16_t len);
/* send the INT-IN data to the host */
void CCID_INT_message(usb_dev *udev);
/* receive the data from USB BULK-OUT buffer to pointer */
void CCID_receive_cmd_header(usb_dev *udev, uint8_t *pDst, uint8_t len);
/* provides the status of previous interrupt transfer status */
uint8_t CCID_IS_INT_transfer_complete(usb_dev *udev);
/* provides the status of previous interrupt transfer status */
void CCID_set_INT_transfer_status (usb_dev *udev, uint8_t xfer_status);

#endif /* CCID_ITF_H */
