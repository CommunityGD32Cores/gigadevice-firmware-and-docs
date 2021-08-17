/*!
    \file  I2C_IE.h
    \brief the header file of I2C0 and I2C1 interrupt
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.2, firmware for GD32F403
*/

#ifndef I2C_IE_H
#define I2C_IE_H

#include "gd32f403.h"

#define I2C0_SLAVE_ADDRESS7    0x82
#define I2C1_SLAVE_ADDRESS7    0x72

extern  volatile ErrStatus    status;
extern  volatile uint8_t*     i2c_txbuffer;
extern  volatile uint8_t*     i2c_rxbuffer;
extern  volatile uint16_t     I2C_nBytes;

/* function declarations */
/* handle I2C0 event interrupt request */
void I2C0_EventIRQ_Handler(void);
/* handle I2C0 error interrupt request */
void I2C0_ErrorIRQ_Handler(void);
/* handle I2C1 event interrupt request */
void I2C1_EventIRQ_Handler(void);
/* handle I2C1 error interrupt request */
void I2C1_ErrorIRQ_Handler(void);

#endif /* I2C_IE_H */
