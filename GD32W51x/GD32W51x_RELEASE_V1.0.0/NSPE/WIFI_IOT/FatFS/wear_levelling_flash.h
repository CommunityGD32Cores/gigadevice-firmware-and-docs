/*!
    \file    wear_levelling_flash.h
    \brief   Wear levelling flash file for GD32W51x WiFi driver

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef _WL_Flash_H_
#define _WL_Flash_H_

#include "ff.h"

typedef struct wl_state_s
{
    uint32_t pos;
    uint32_t max_pos;
    uint32_t move_count;
    uint32_t block_size;
    uint32_t version;
    uint32_t crc;
} wl_state_t;

typedef struct wl_config_s
{
    uint32_t start_addr;
    uint32_t full_mem_size;
    uint32_t page_size;
    uint32_t sector_size;
    uint32_t wr_size;
    uint32_t version;
    uint32_t temp_buff_size;
    uint32_t crc;
} wl_config_t;

typedef struct wl_flash
{
    wl_state_t state;
    wl_config_t cfg;
    uint32_t addr_cfg;
    uint32_t addr_state1;
    uint32_t addr_state2;
    uint32_t flash_size;
    uint32_t state_size;
    uint32_t cfg_size;
    uint8_t *temp_buff;
    uint32_t dummy_addr;
} wl_flash_t;

extern wl_flash_t wl_config_global;

int wl_flash_config(wl_flash_t *wl_flash);
int wl_flash_erase_range(wl_flash_t *wl_flash, uint32_t start_address, uint32_t size);
int wl_flash_write(wl_flash_t *wl_flash, uint32_t dest_addr, const uint8_t *src, size_t size);
int wl_flash_read(wl_flash_t *wl_flash, uint32_t src_addr, uint8_t *dest, size_t size);
#endif
