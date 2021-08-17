/*!
    \file  flash_if.h
    \brief USB DFU device flash interface header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_IF_H
#define FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "dfu_mal.h"

#define FLASH_START_ADDR        0x08000000

#define OB_RDPT                 0x1FFFF800

#define FLASH_END_ADDR          0x08300000
#define FLASH_IF_STRING         "@Internal Flash   /0x08000000/16*002Ka,112*002Kg"

extern DFU_MAL_Property_TypeDef DFU_Flash_cb;

#endif /* FLASH_IF_H */

