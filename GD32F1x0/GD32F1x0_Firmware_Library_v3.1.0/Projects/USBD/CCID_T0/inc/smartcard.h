/*!
    \file  smartcard.h
    \brief This file contains all the functions prototypes for the Smartcard firmware library.
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/
  
#ifndef __SMARTCARD_H
#define __SMARTCARD_H

#include "gd32f1x0.h"
#include "platform_config.h"
#include "sc_itf.h"

/* Exported constants */
#define T0_PROTOCOL               0x00    /* T0 protocol */
#define T1_PROTOCOL               0x01    /* T1 protocol */
#define DIRECT                    0x3B    /* Direct bit convention */
#define INDIRECT                  0x3F    /* Indirect bit convention */
#define SETUP_LENGTH              20
#define HIST_LENGTH               16
#define MAX_PROTOCOLLEVEL         7       /* Maximum levels of protocol */
#define MAX_INTERFACEBYTE         4       /* Maximum number of interface bytes per protocol */
#define LC_MAX                    22
#define SC_RECEIVE_TIMEOUT        0x8000  /* Direction to reader */

/* T = 1 protocol constants */
#define T1_I_BLOCK                0x00    /* PCB (I-block: b8 = 0)  */
#define T1_R_BLOCK                0x80    /* PCB (R-block: b8 b7 = 10) */
#define T1_S_BLOCK                0xC0    /* PCB (S-block: b8 b7 = 11) */

/* I block */
#define T1_I_SEQ_SHIFT            6       /* N(S) position (bit 7) */ 

/* R block */
#define T1_IS_ERROR(pcb)          ((pcb) & 0x0F)
#define T1_EDC_ERROR              0x01    /* [b6..b1] = 0-N(R)-0001 */
#define T1_OTHER_ERROR            0x02    /* [b6..b1] = 0-N(R)-0010 */
#define T1_R_SEQ_SHIFT            4       /* N(R) position (b5) */

/* S block */
#define T1_S_RESPONSE             0x20   /* If response: set bit b6, if request reset b6 in PCB S-Block */
#define T1_S_RESYNC               0x00   /* RESYNCH: b6->b1: 000000 of PCB S-Block */
#define T1_S_IFS                  0x01   /* IFS: b6->b1: 000001 of PCB S-Block */
#define T1_S_ABORT                0x02   /* ABORT: b6->b1: 000010 of PCB S-Block */
#define T1_S_WTX                  0x03   /* WTX: b6->b1: 000011 of PCB S-Block */

#define NAD                       0      /* NAD byte position in the block */
#define PCB                       1      /* PCB byte position in the block */
#define LEN                       2      /* LEN byte position in the block */
#define DATA                      3      /* The position of the first byte of INF field in the block */

/* Modifiable parameters */
#define SAD                       0x0    /* Source address: reader (allowed values 0 -> 7) */
#define DAD                       0x0    /* Destination address: card (allowed values 0 -> 7) */
#define IFSD_VALUE                254    /* Max length of INF field Supported by the reader */
#define SC_FILE_SIZE              0x100  /* File size */
#define SC_FILE_ID                0x0001 /* File identifier */
#define SC_CLASS                  0x00

/* Constant parameters */
#define INS_SELECT_FILE           0xA4 /* Select file instruction */
#define INS_READ_FILE             0xB0 /* Read file instruction */
#define INS_WRITE_FILE            0xD6 /* Write file instruction */
#define TRAILER_LENGTH            2    /* Trailer length (SW1 and SW2: 2 bytes) */

#define SC_T1_RECEIVE_SUCCESS     0
#define SC_T1_BWT_TIMEOUT         1
#define SC_T1_CWT_TIMEOUT         2

#define DEFAULT_FIDI_VALUE        0x11
#define PPS_REQUEST               0xFF
    
/* SC Tree Structure -----------------------------------------------------------
                              MasterFile
                           ________|___________
                          |        |           |
                        System   UserData     Note
------------------------------------------------------------------------------*/

/* SC ADPU Command: Operation Code */
#define SC_CLA_GSM11              0xA0

/* Data Area Management Commands */
#define SC_SELECT_FILE            0xA4
#define SC_GET_RESPONCE           0xC0
#define SC_STATUS                 0xF2
#define SC_UPDATE_BINARY          0xD6
#define SC_READ_BINARY            0xB0
#define SC_WRITE_BINARY           0xD0
#define SC_UPDATE_RECORD          0xDC
#define SC_READ_RECORD            0xB2

/* Administrative Commands */ 
#define SC_CREATE_FILE            0xE0

/* Safety Management Commands */
#define SC_VERIFY                 0x20
#define SC_CHANGE                 0x24
#define SC_DISABLE                0x26
#define SC_ENABLE                 0x28
#define SC_UNBLOCK                0x2C
#define SC_EXTERNAL_AUTH          0x82
#define SC_GET_CHALLENGE          0x84

/* Smartcard Interface Byte */
#define SC_INTERFACEBYTE_TA       0 /* Interface byte TA(i) */
#define SC_INTERFACEBYTE_TB       1 /* Interface byte TB(i) */
#define SC_INTERFACEBYTE_TC       2 /* Interface byte TC(i) */
#define SC_INTERFACEBYTE_TD       3 /* Interface byte TD(i) */

/* Answer to reset Commands */ 
#define SC_GET_A2R                0x00

/* SC STATUS: Status Code */
#define SC_EF_SELECTED            0x9F
#define SC_DF_SELECTED            0x9F
#define SC_OP_TERMINATED          0x9000

/* Smartcard Voltage */
#define SC_VOLTAGE_5V             0
#define SC_VOLTAGE_3V             1
#define SC_VOLTAGE_NOINIT         0xFF

typedef enum
{
    SC_POWER_ON = 0x00,
    SC_RESET_LOW,
    SC_RESET_HIGH,
    SC_ACTIVE,
    SC_ACTIVE_ON_T0,
    SC_POWER_OFF,
    SC_NO_INIT
} SC_State;

/* Interface Byte structure - TA(i), TB(i), TC(i) and TD(i) */
typedef struct
{
    uint8_t Status;     /* The Presence of the Interface byte */
    uint8_t Value;      /* The Value of the Interface byte */
} SC_InterfaceByte;

/* Protocol Level structure */
typedef struct
{
    SC_InterfaceByte InterfaceByte[MAX_INTERFACEBYTE];      /* The Values of the Interface byte TA(i), TB(i), TC(i)and TD(i) */
} SC_ProtocolLevel;

/* ATR structure - Answer To Reset */
typedef struct
{
    uint8_t TS;                                 /* Bit Convention Direct/Indirect */
    uint8_t T0;                                 /* Each bit in the high nibble = Presence of the further interface byte;
                                                 Low nibble = Number of historical byte */
    SC_ProtocolLevel T[MAX_PROTOCOLLEVEL];      /* Setup array */
    uint8_t H[HIST_LENGTH];                     /* Historical array */
    uint8_t Tlength;                            /* Setup array dimension */
    uint8_t Hlength;                            /* Historical array dimension */
    uint8_t TCK;                                /* Check character */ 
} SC_ATR;

/* ADPU-Header command structure */
typedef struct
{
    uint8_t CLA;  /* Command class */
    uint8_t INS;  /* Operation code */
    uint8_t P1;   /* Selection Mode */
    uint8_t P2;   /* Selection Option */
//	  uint8_t P3;   /* Selection Option */
} SC_Header;

/* ADPU-Body command structure */
typedef struct 
{
    uint8_t LC;           /* Data field length */
    uint8_t Data[LC_MAX]; /* Command parameters */
    uint8_t LE;           /* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure */
typedef struct
{
    SC_Header Header;
    SC_Body Body;
} SC_ADPU_Commands;

/* SC response structure */
typedef struct
{
    uint8_t Data[LC_MAX];  /* Data returned from the card */
    uint8_t SW1;           /* Command Processing status */
    uint8_t SW2;           /* Command Processing qualification */
} SC_ADPU_Response;

extern SC_ADPU_Commands SC_ADPU;
extern uint8_t SC_ATR_Table[40];
extern SC_ATR SC_A2R;

/* APPLICATION LAYER */
void SC_Handler             (SC_State *SCState, 
                             SC_ADPU_Commands *SC_ADPU, 
                             SC_ADPU_Response *SC_Response);

void SC_PowerCmd            (ControlStatus NewState);
void SC_Reset               (FlagStatus ResetState);
void SC_ParityErrorHandler  (void);
void SC_PTSConfig           (void);
void SC_VoltageConfig       (uint32_t SC_Voltage);

uint8_t SC_Detect           (void);
uint32_t SC_GetDTableValue  (uint8_t );

#endif /* __SMARTCARD_H */
