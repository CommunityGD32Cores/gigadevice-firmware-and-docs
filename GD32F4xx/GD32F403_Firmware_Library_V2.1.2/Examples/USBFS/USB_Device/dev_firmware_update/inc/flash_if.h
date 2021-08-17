/*!
    \file    flash_if.h
    \brief   USB DFU device flash interface header file

    \version 2020-08-01, V3.0.0, firmware for GD32F403
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

#ifndef __FLASH_IF_H
#define __FLASH_IF_H

#include "dfu_mal.h"

#define FLASH_START_ADDR        0x08000000

#define OB_RDPT0                0x1FFFC000
#define OB_RDPT1                0x1FFEC000

#define OB_RDPT                 0x1ffff800

#define MAL_MASK_OB             0xFFFFFF00

#define FLASH_END_ADDR          0x08080000

#define FLASH_IF_STRING         "@Internal Flash   /0x08000000/16*002Ka,112*002Kg"

extern dfu_mal_prop DFU_Flash_cb;

/* function declarations */
/* program option byte */
fmc_state_enum Option_Byte_Write(uint32_t Mem_Add,uint8_t* data);

#endif /* __FLASH_IF_H */
