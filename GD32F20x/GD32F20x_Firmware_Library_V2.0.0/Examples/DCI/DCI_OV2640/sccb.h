/*!
    \file  sccb.h
    \brief definitions for sccb 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef SCCB_H
#define SCCB_H

#include "gd32f20x.h"

 /* configure I2C pins */ 
#define DCI_SCCB                       I2C1
#define DCI_SCCB_CLK                   RCU_I2C1

#define DCI_SCCB_SDA_PIN               GPIO_PIN_11
#define DCI_SCCB_SDA_GPIO_PORT         GPIOB
#define DCI_SCCB_SDA_GPIO_CLK          RCU_GPIOB

#define DCI_SCCB_SCL_PIN               GPIO_PIN_10
#define DCI_SCCB_SCL_GPIO_PORT         GPIOB
#define DCI_SCCB_SCL_GPIO_CLK          RCU_GPIOB

#define SCCB_FLAG_TIMEOUT              20000000
#define OV2640_DEVICE_WRITE_ADDRESS    0x60
#define OV2640_DEVICE_READ_ADDRESS     0x61

#define SCCB_SPEED                     100000
#define SCCB_SLAVE_ADDRESS7            0xFE

/* function declarations */
void sccb_config(void);
uint8_t dci_byte_write(uint8_t reg, uint8_t data);
uint8_t dci_byte_read(uint8_t reg, uint8_t *data);

#endif /* SCCB_H */
