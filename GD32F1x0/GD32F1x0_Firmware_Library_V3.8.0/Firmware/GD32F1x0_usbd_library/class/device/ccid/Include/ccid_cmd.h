/*!
    \file    ccid_cmd.h
    \brief   the header of CCID commands handler

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

#ifndef CCID_CMD_H
#define CCID_CMD_H

#include "ccid_core.h"

/* error codes for usb bulk in: bError */
#define SLOT_NO_ERROR                            0x81U            /*!< slot no error */
#define SLOTERROR_UNKNOWN                        0x82U            /*!< slot error unknown */

/* update CCID command status */
#define CCID_current_cmd_status(cmd_status, icc_status) ccid->CCID_bulkin_data.bStatus = ((cmd_status) | (icc_status))

/* Index of not supported / incorrect message parameter : 7Fh to 01h */
/* These Values are used for Return Types between Firmware Layers */
/* Failure of a command. The CCID cannot parse one parameter or the ICC is 
   not supporting one parameter. Then the Slot Error register contains the 
   index of the first bad parameter as a positive number (1-127). For instance,
   if the CCID receives an ICC command to an unimplemented slot, then the Slot 
   Error register shall be set to (index of bSlot field). */
#define SLOTERROR_BAD_LENTGH                    0x01U             /*!< slot error BAD length */
#define SLOTERROR_BAD_SLOT                      0x05U             /*!< slot error BAD slot */
#define SLOTERROR_BAD_POWERSELECT               0x07U             /*!< slot error BAD powerselect */
#define SLOTERROR_BAD_PROTOCOLNUM               0x07U             /*!< slot error BAD protocol number */
#define SLOTERROR_BAD_CLOCKCOMMAND              0x07U             /*!< slot error BAD clock command */
#define SLOTERROR_BAD_ABRFU_3B                  0x07U             /*!< slot error BAD ABRFU 3B */
#define SLOTERROR_BAD_BMCHANGES                 0x07U             /*!< slot error BAD changes */
#define SLOTERROR_BAD_BFUNCTION_MECHANICAL      0x07U             /*!< slot error BAD function mechanical */
#define SLOTERROR_BAD_ABRFU_2B                  0x08U             /*!< slot error BAD ABRFU 2B */
#define SLOTERROR_BAD_LEVELPARAMETER            0x08U             /*!< slot error BAD level parameter */
#define SLOTERROR_BAD_FIDI                      0x0AU             /*!< slot error BAD FIDI */
#define SLOTERROR_BAD_T01CONVCHECKSUM           0x0BU             /*!< slot error BAD convert checksum */
#define SLOTERROR_BAD_GUARDTIME                 0x0CU             /*!< slot error BAD guardtime */
#define SLOTERROR_BAD_WAITINGINTEGER            0x0DU             /*!< slot error BAD waiting integer */
#define SLOTERROR_BAD_CLOCKSTOP                 0x0EU             /*!< slot error BAD clock stop */
#define SLOTERROR_BAD_IFSC                      0x0FU             /*!< slot error BAD IFSC */
#define SLOTERROR_BAD_NAD                       0x10U             /*!< slot error BAD NAD */
#define SLOTERROR_BAD_DWLENGTH                  0x08U             /*!< slot error BAD length */

/* Table 6.2-2 Slot error register when bmCommandStatus = 1 */
#define SLOTERROR_CMD_ABORTED                   0xFFU             /*!< slot error command aborted */
#define SLOTERROR_ICC_MUTE                      0xFEU             /*!< slot error ICC mute */
#define SLOTERROR_XFR_PARITY_ERROR              0xFDU             /*!< slot error transfer parity error */
#define SLOTERROR_XFR_OVERRUN                   0xFCU             /*!< slot error transfer overrun */
#define SLOTERROR_HW_ERROR                      0xFBU             /*!< slot error HW error */
#define SLOTERROR_BAD_ATR_TS                    0xF8U             /*!< slot error BAD attribute TS */
#define SLOTERROR_BAD_ATR_TCK                   0xF7U             /*!< slot error BAD attribute TCK */
#define SLOTERROR_ICC_PROTOCOL_NOT_SUPPORTED    0xF6U             /*!< slot error ICC protocol not supported */
#define SLOTERROR_ICC_CLASS_NOT_SUPPORTED       0xF5U             /*!< slot error ICC class not supported */
#define SLOTERROR_PROCEDURE_BYTE_CONFLICT       0xF4U             /*!< slot error procedure byte conflict */
#define SLOTERROR_DEACTIVATED_PROTOCOL          0xF3U             /*!< slot error deactivated protocol */
#define SLOTERROR_BUSY_WITH_AUTO_SEQUENCE       0xF2U             /*!< slot error busy wait with auto sequence */
#define SLOTERROR_PIN_TIMEOUT                   0xF0U             /*!< slot error PIN timeout */
#define SLOTERROR_PIN_CANCELLED                 0xEFU             /*!< slot error PIN cancelled */
#define SLOTERROR_CMD_SLOT_BUSY                 0xE0U             /*!< slot error command slot busy */
#define SLOTERROR_CMD_NOT_SUPPORTED             0x00U             /*!< slot error command not supported */

#define DEFAULT_FIDI                            0x11U             /*!< default FIDI */
#define DEFAULT_T01CONVCHECKSUM                 0x00U             /*!< default convert checksum */
#define DEFAULT_EXTRA_GUARDTIME                 0x00U             /*!< default extra guardtime */
#define DEFAULT_WAITINGINTEGER                  0x0AU             /*!< default waiting integer */
#define DEFAULT_CLOCKSTOP                       0x00U             /*!< default clock stop */
#define DEFAULT_IFSC                            0x20U             /*!< default IFSC */
#define DEFAULT_NAD                             0x00U             /*!< default NAD */
#define DEFAULT_DATA_RATE                       0x000025CDU       /*!< default data rate */
#define DEFAULT_CLOCK_FREQ                      0x00000E10U       /*!< default clock frequency */

/* Offset=0 bmICCStatus 2 bit  0, 1, 2
    0 - An ICC is present and active (power is on and stable, RST is inactive)
    1 - An ICC is present and inactive (not activated or shut down by hardware error)
    2 - No ICC is present
    3 - RFU
Offset=0 bmRFU 4 bits 0 RFU
Offset=6 bmCommandStatus 2 bits 0, 1, 2
    0 - Processed without error
    1 - Failed (error code provided by the error register)
    2 - Time Extension is requested
    3 - RFU */
#define BM_ICC_CURRENT_ACTIVE                   0x00U            /*!< BM ICC current active */
#define BM_ICC_CURRENT_INACTIVE                 0x01U            /*!< BM ICC current inactive */
#define BM_ICC_NO_ICC_CURRENT                   0x02U            /*!< BM ICC no ICC current */

#define BM_CMD_STATUS_OFFSET                    0x06U            /*!< BM command status offset */
#define BM_CMD_STATUS_NO_ERROR                  0x00U            /*!< BM command status no error */
#define BM_CMD_STATUS_FAILED                    (0x01U << BM_CMD_STATUS_OFFSET)           /*!< BM command status failed */
#define BM_CMD_STATUS_TIME_EXTN                 (0x02U << BM_CMD_STATUS_OFFSET)           /*!< BM command status time extern */

/* defines for the CCID_CMD Layers */
#define LEN_RDR_TO_PC_SLOTSTATUS                10U              /*!< RDR to PC slot status length */
#define LEN_PROTOCOL_STRUCT_T0                  5U               /*!< protocol struct T0 length */

#define BPROTOCOL_NUM_T0                        0U               /*!< protocol number T0 */
#define BPROTOCOL_NUM_T1                        1U               /*!< protocol number T1 */

/* error codes for RDR_TO_PC_HARDWAREERROR message: bHardwareErrorCode */
#define HARDWAREERRORCODE_OVERCURRENT           0x01U            /*!< HW error code over current */
#define HARDWAREERRORCODE_VOLTAGEERROR          0x02U            /*!< HW error code voltage error */
#define HARDWAREERRORCODE_OVERCURRENT_IT        0x04U            /*!< HW error code over current IT */
#define HARDWAREERRORCODE_VOLTAGEERROR_IT       0x08U            /*!< HW error code voltage error IT */

typedef enum {
    CHECK_PARAMETER_SLOT = 1U,                                   /*!< check parameter slot */
    CHECK_PARAMETER_DWLENGTH = (1U << 1U),                       /*!< check parameter down length */
    CHECK_PARAMETER_abRFU2 = (1U << 2U),                         /*!< check parameter ABRFU2 */
    CHECK_PARAMETER_abRFU3 = (1U << 3U),                         /*!< check parameter ABRFU3 */
    CHECK_PARAMETER_CARD_PRESENT = (1U << 4U),                   /*!< check parameter card present */
    CHECK_PARAMETER_ABORT = (1U << 5U),                          /*!< check parameter abort */
    CHECK_ACTIVE_STATE = (1U << 6U)                              /*!< check active state */
} ChkParam_t;

/* function declarations */
/* PC_TO_RDR_ICCPOWERON message execution, apply voltage and get ATR */
uint8_t PC_to_RDR_ICC_poweron(usb_dev *udev);
/* ICC VCC is switched off */
uint8_t PC_to_RDR_ICC_poweroff(usb_dev *udev);
/* provides the slot status to the host */
uint8_t PC_to_RDR_get_slot_status(usb_dev *udev);
/* handles the block transfer from host */
uint8_t PC_to_RDR_xfr_block(usb_dev *udev);
/* provides the ICC parameters to the host */
uint8_t PC_to_RDR_get_parameters(usb_dev *udev);
/* set the ICC parameters to the default */
uint8_t PC_to_RDR_reset_parameters(usb_dev *udev);
/* set the ICC parameters to the host defined parameters */
uint8_t PC_to_RDR_set_parameters(usb_dev *udev);
/* execute the escape command */
uint8_t PC_to_RDR_escape(usb_dev *udev);
/* execute the clock specific command from host */
uint8_t PC_to_RDR_ICC_clock(usb_dev *udev);
/* execute the abort command from host */
uint8_t PC_to_RDR_abort(usb_dev *udev);
/* execute the PC_TO_RDR_T0APDU command from host */
uint8_t PC_to_RDR_T0_APDU(usb_dev *udev);
/* execute the PC_TO_RDR_MECHANICAL command from host */
uint8_t PC_to_RDR_mechanical(usb_dev *udev);
/* set the required card frequency and data rate from the host */
uint8_t PC_to_RDR_Set_DataRate_and_ClockFreq(usb_dev *udev);
/* execute the secure command from the host */
uint8_t PC_TO_RDR_secure(usb_dev *udev);
/* execute the abort command from bulk endpoint or from control endpoint */
uint8_t CCID_cmd_abort(usb_dev *udev, uint8_t slot, uint8_t seq);
/* provide the data block response to the host */
void RDR_to_PC_data_block(usb_dev *udev, uint8_t errorCode);
/* provide the slot status response to the host */
void RDR_to_PC_slot_status(usb_dev *udev, uint8_t errorCode);
/* provide the data block response to the host */
void RDR_to_PC_parameters(usb_dev *udev, uint8_t errorCode);
/* provide the escaped data block response to the host */
void RDR_to_PC_escape(usb_dev *udev, uint8_t errorCode);
/* provide the clock and data rate information to host */
void RDR_to_PC_DataRate_and_ClockFreq(usb_dev *udev, uint8_t errorCode);
/* interrupt message to be sent to the host, checks the card presence status and update the buffer accordingly */
void RDR_to_PC_notify_slot_change(usb_dev *udev);
/* updates the variable for the slot status */
void CCID_update_slot_status(usb_dev *udev, uint8_t slot_status);
/* updates the variable for the slot change status */
void CCID_update_slot_change(usb_dev *udev, uint8_t change_status);
/* provides the value of the variable for the slot change status */
uint8_t CCID_IS_slot_status_change(usb_dev *udev);
/* checks the specific parameters requested by the function and update status accordingly */
uint8_t CCID_cmd_params_check(usb_dev *udev, uint32_t param_type);

#endif /* CCID_CMD_H */
