/*!
    \file    wear_levelling_flash.c
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

#include "wear_levelling_flash.h"
#include "gd32w51x_crc.h"
#include "debug_print.h"
#include "stdio.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif

#define ERR_CHECK(func) {      \
            result = func; \
            if (result)        \
                return result; \
        }

wl_flash_t wl_config_global;

static uint32_t wl_calculate_crc(uint8_t *pbuf, uint32_t buffer_size);
static uint32_t wl_flash_calc_addr(wl_flash_t *wl_flash, uint32_t addr);
static int wl_flash_erase_raw(wl_flash_t *wl_flash, uint32_t start_address, uint32_t size);
static int wl_flash_recover_pos(wl_flash_t *wl_flash);
static int wl_flash_init_sections(wl_flash_t *wl_flash);
static int wl_flash_erase_sector(wl_flash_t *wl_flash, uint32_t sector);
static int wl_flash_updatewl(wl_flash_t *wl_flash);

/*!
    \brief      from virtual address calculating physical address
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[in]  address: virtual flash address
    \param[out] none
    \retval     physical flash address
*/
static uint32_t wl_flash_calc_addr(wl_flash_t *wl_flash, uint32_t addr)
{
    uint32_t pa = 0, dummy_addr = 0;

    pa = (wl_flash->flash_size - wl_flash->state.move_count * wl_flash->cfg.page_size + addr) % wl_flash->flash_size;
    dummy_addr = wl_flash->state.pos * wl_flash->cfg.page_size;
    if (pa >= dummy_addr) {
        pa += wl_flash->cfg.page_size;
    }

    return pa;
}

/*!
    \brief      initialize wear levelling related parameter
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
static int wl_flash_init_sections(wl_flash_t *wl_flash)
{
    int result = 0;

    wl_flash->state.pos = 0;
    wl_flash->state.move_count = 0;
    wl_flash->state.version = wl_flash->cfg.version;
    wl_flash->state.block_size = wl_flash->cfg.page_size;
    wl_flash->state.max_pos = 1 + wl_flash->flash_size / wl_flash->cfg.page_size;
    wl_flash->state.crc = wl_calculate_crc((uint8_t *)&wl_flash->state, sizeof(wl_state_t) - sizeof(uint32_t));
    ERR_CHECK(wl_flash_erase_raw(wl_flash, wl_flash->addr_state1, wl_flash->state_size))
    ERR_CHECK(mbl_flash_write(wl_flash->addr_state1, (uint8_t *)&wl_flash->state, sizeof(wl_state_t)))
    ERR_CHECK(wl_flash_erase_raw(wl_flash, wl_flash->addr_state2, wl_flash->state_size))
    ERR_CHECK(mbl_flash_write(wl_flash->addr_state2, (uint8_t *)&wl_flash->state, sizeof(wl_state_t)))
    ERR_CHECK(wl_flash_erase_raw(wl_flash, wl_flash->addr_cfg, wl_flash->cfg_size))
    ERR_CHECK(mbl_flash_write(wl_flash->addr_cfg, (uint8_t *)&wl_flash->cfg, sizeof(wl_config_t)))

    return result;
}

/*!
    \brief      recover pos that in use
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
static int wl_flash_recover_pos(wl_flash_t *wl_flash)
{
    uint8_t pos_bits = 0;
    int result = 0;

    for (uint32_t i = 0; i < wl_flash->state.max_pos; i++) {
        ERR_CHECK(mbl_flash_indirect_read(wl_flash->addr_state1 + sizeof(wl_state_t) + i * wl_flash->cfg.wr_size / 8, &pos_bits, 1))
        if (pos_bits == 0xff) {
            wl_flash->state.pos = i;
            break;
        }
    }
    if (wl_flash->state.pos == wl_flash->state.max_pos) {
        wl_flash->state.pos--;
    }
    return result;
}

/*!
    \brief      erase flash sector by physical address
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[in]  start_address: start erase address
    \param[in]  size: size of flash to erase
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
static int wl_flash_erase_raw(wl_flash_t *wl_flash, uint32_t start_address, uint32_t size)
{
    uint32_t i = 0, sector_count = 0;
    int result = 0;

    sector_count = size / wl_flash->cfg.sector_size;
    for (i = 0; i < sector_count; i++) {
        ERR_CHECK(mbl_flash_erase((start_address + (i * wl_flash->cfg.sector_size)), wl_flash->cfg.sector_size))
    }
    return result;
}

/*!
    \brief      update wear levelling table
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
static int wl_flash_updatewl(wl_flash_t *wl_flash)
{
    const uint8_t used_bits = 1;
    int result = 0;
    uint32_t byte_pos = 0, copy_count = 0, i = 0;
    uint32_t data_addr = wl_flash->state.pos + 1;

    if (data_addr >= wl_flash->state.max_pos) {
        data_addr = 0;
    }

    copy_count = wl_flash->cfg.page_size / wl_flash->cfg.temp_buff_size;
    data_addr = wl_flash->cfg.start_addr + data_addr * wl_flash->cfg.page_size;
    wl_flash->dummy_addr = wl_flash->cfg.start_addr + wl_flash->state.pos * wl_flash->cfg.page_size;
    ERR_CHECK(wl_flash_erase_raw(wl_flash, wl_flash->dummy_addr, wl_flash->cfg.page_size))
    for (i = 0; i < copy_count; i++) {
        ERR_CHECK(mbl_flash_indirect_read(data_addr + i * wl_flash->cfg.temp_buff_size, wl_flash->temp_buff, wl_flash->cfg.temp_buff_size))
        ERR_CHECK(mbl_flash_write(wl_flash->dummy_addr + i * wl_flash->cfg.temp_buff_size, wl_flash->temp_buff, wl_flash->cfg.temp_buff_size))
    }

    byte_pos = wl_flash->state.pos * wl_flash->cfg.wr_size / 8;
    ERR_CHECK(mbl_flash_write(wl_flash->addr_state1 + sizeof(wl_state_t) + byte_pos, (uint8_t *)&used_bits, 1))
    ERR_CHECK(mbl_flash_write(wl_flash->addr_state2 + sizeof(wl_state_t) + byte_pos, (uint8_t *)&used_bits, 1))
    wl_flash->state.pos++;
    if (wl_flash->state.pos >= wl_flash->state.max_pos) {
        wl_flash->state.pos = 0;
        wl_flash->state.move_count++;
        if (wl_flash->state.move_count >= (wl_flash->state.max_pos - 1)) {
            wl_flash->state.move_count = 0;
        }
        wl_flash->state.crc = wl_calculate_crc((uint8_t *)&wl_flash->state, sizeof(wl_state_t) - sizeof(uint32_t));
        ERR_CHECK(wl_flash_erase_raw(wl_flash, wl_flash->addr_state1, wl_flash->state_size))
        ERR_CHECK(mbl_flash_write(wl_flash->addr_state1, (uint8_t *)&wl_flash->state, sizeof(wl_state_t)))
        ERR_CHECK(wl_flash_erase_raw(wl_flash, wl_flash->addr_state2, wl_flash->state_size))
        ERR_CHECK(mbl_flash_write(wl_flash->addr_state2, (uint8_t *)&wl_flash->state, sizeof(wl_state_t)))
    }

    return result;
}

/*!
    \brief      erase flash sector by virtual address
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[in]  sector: sector number to erase
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
static int wl_flash_erase_sector(wl_flash_t *wl_flash, uint32_t sector)
{
    uint32_t virt_addr;
    int result = 0;

    ERR_CHECK(wl_flash_updatewl(wl_flash))
    virt_addr = wl_flash_calc_addr(wl_flash, sector * wl_flash->cfg.sector_size);
    ERR_CHECK(mbl_flash_erase(wl_flash->cfg.start_addr + virt_addr, wl_flash->cfg.sector_size))

    return result;
}

/*!
    \brief      erase a range of flash
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[in]  size: size of flash to erase
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
int wl_flash_erase_range(wl_flash_t *wl_flash, uint32_t start_address, uint32_t size)
{
    uint32_t i = 0, erase_count = 0, start_sector = 0;
    int result = 0;

    erase_count = (size + wl_flash->cfg.sector_size - 1) / wl_flash->cfg.sector_size;
    start_sector = start_address / wl_flash->cfg.sector_size;
    for (i = 0; i < erase_count; i++) {
        ERR_CHECK(wl_flash_erase_sector(wl_flash, start_sector + i))
    }
    return result;
}

/*!
    \brief      recover or initialize wear levelling table
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
int wl_flash_config(wl_flash_t *wl_flash)
{
    wl_state_t sa_copy;
    uint32_t check_size = 0;
    uint32_t i = 0;
    uint32_t crc1;
    uint32_t crc2;
    int result = -1;

    wl_flash->cfg.crc = wl_calculate_crc((uint8_t *)(&wl_flash->cfg), sizeof(wl_config_t) - sizeof(wl_flash->cfg.crc));
    if (((&wl_flash->cfg)->sector_size % (&wl_flash->cfg)->temp_buff_size) != 0) {
        DEBUGPRINT("ERROR_FATFS: wl_flash->cfg->sector_size remainder wl_flash->cfg->temp_buff_size is 0\r\n");
        goto error;
    }
    if (((&wl_flash->cfg)->page_size < (&wl_flash->cfg)->sector_size)) {
        DEBUGPRINT("ERROR_FATFS: wl_flash->cfg->page_size < wl_flash->cfg->sector_size\r\n");
        goto error;
    }

    wl_flash->temp_buff = (uint8_t *)sys_malloc(wl_flash->cfg.temp_buff_size);
    if(wl_flash->temp_buff == NULL) {
        DEBUGPRINT("ERROR_FATFS: wl_flash_config, malloc wl_flash->temp_buff failed\r\n");
        goto error;
    }

    wl_flash->state_size = wl_flash->cfg.sector_size;
    if (wl_flash->state_size < (sizeof(wl_state_t) + (wl_flash->cfg.full_mem_size / wl_flash->cfg.sector_size) * wl_flash->cfg.wr_size / 8)) {
        /* calculate the size of wl state with one byte for each sector*/
        wl_flash->state_size = ((sizeof(wl_state_t) + (wl_flash->cfg.full_mem_size / wl_flash->cfg.sector_size) * wl_flash->cfg.wr_size / 8) + wl_flash->cfg.sector_size - 1) / wl_flash->cfg.sector_size;
        /* sector number to use = ((sizeof wl_state_t + sector number * write size(1 byte) + 1 sector size - 1) / sector size) */
        wl_flash->state_size = wl_flash->state_size * wl_flash->cfg.sector_size;
        /* multiply sector size to get total size */
    }

    wl_flash->cfg_size = (sizeof(wl_config_t) + wl_flash->cfg.sector_size - 1) / wl_flash->cfg.sector_size;
    wl_flash->cfg_size = wl_flash->cfg_size * wl_flash->cfg.sector_size;
    /* calculate wl cfg size with same way above */
    wl_flash->addr_cfg = wl_flash->cfg.start_addr + wl_flash->cfg.full_mem_size - wl_flash->cfg_size;
    wl_flash->addr_state1 = wl_flash->cfg.start_addr + wl_flash->cfg.full_mem_size - wl_flash->state_size * 2 - wl_flash->cfg_size;
    wl_flash->addr_state2 = wl_flash->cfg.start_addr + wl_flash->cfg.full_mem_size - wl_flash->state_size * 1 - wl_flash->cfg_size;
    /* calculate start address of wl state1、state2 and cfg */
    wl_flash->flash_size = ((wl_flash->cfg.full_mem_size - wl_flash->state_size * 2 - wl_flash->cfg_size) / wl_flash->cfg.page_size - 1) * wl_flash->cfg.page_size;
    /* flash size remained to use */

    result = mbl_flash_indirect_read(wl_flash->addr_state1, (uint8_t *)&wl_flash->state, sizeof(wl_state_t));
    if (result) goto error;

    result = mbl_flash_indirect_read(wl_flash->addr_state2, (uint8_t *)&sa_copy, sizeof(wl_state_t));
    if (result) goto error;

    check_size = sizeof(wl_state_t) - sizeof(uint32_t);
    crc1 = wl_calculate_crc((uint8_t *)&wl_flash->state, check_size);
    crc2 = wl_calculate_crc((uint8_t *)&sa_copy, check_size);

    if ((crc1 == wl_flash->state.crc) && (crc2 == sa_copy.crc)) {
        if (wl_flash->state.version != wl_flash->cfg.version) {
            result = wl_flash_init_sections(wl_flash);
            if (result) goto error;

            result = wl_flash_recover_pos(wl_flash);
            if (result) goto error;

        } else {
            if (crc1 != crc2) {
                result = wl_flash_erase_raw(wl_flash, wl_flash->addr_state2, wl_flash->state_size);
                if (result) goto error;

                result = mbl_flash_write(wl_flash->addr_state2, (uint8_t *)&wl_flash->state, sizeof(wl_state_t));
                if (result) goto error;

                for (i = 0; i < ((wl_flash->cfg.full_mem_size / wl_flash->cfg.sector_size)*wl_flash->cfg.wr_size / 8); i++) {
                    uint8_t pos_bits = 0;
                    result = mbl_flash_indirect_read(wl_flash->addr_state1 + sizeof(wl_state_t) + i, &pos_bits, 1);
                    if (result) goto error;

                    if (pos_bits != 0xff) {
                        result = mbl_flash_write(wl_flash->addr_state2 + sizeof(wl_state_t) + i, &pos_bits, 1);
                        if (result) goto error;
                    }
                }
            }
            result = wl_flash_recover_pos(wl_flash);
        }
    } else if ((crc1 != wl_flash->state.crc) && (crc2 != sa_copy.crc)) {
        result = wl_flash_init_sections(wl_flash);
        if (result) goto error;

        result = wl_flash_recover_pos(wl_flash);
        if (result) goto error;

    } else {
        if (crc1 == wl_flash->state.crc) {
            result = wl_flash_erase_raw(wl_flash, wl_flash->addr_state2, wl_flash->state_size);
            if (result) goto error;

            result = mbl_flash_write(wl_flash->addr_state2, (uint8_t *)&wl_flash->state, sizeof(wl_state_t));
            if (result) goto error;

            for (i = 0; i < ((wl_flash->cfg.full_mem_size / wl_flash->cfg.sector_size) * wl_flash->cfg.wr_size / 8); i++) {
                uint8_t pos_bits = 0;
                result = mbl_flash_indirect_read(wl_flash->addr_state1 + sizeof(wl_state_t) + i, &pos_bits, 1);
                if (result) goto error;

                if (pos_bits != 0xff)
                {
                    result = mbl_flash_write(wl_flash->addr_state2 + sizeof(wl_state_t) + i, &pos_bits, 1);
                    if (result) goto error;
                }
            }
            result = mbl_flash_indirect_read(wl_flash->addr_state2, (uint8_t *)&wl_flash->state, sizeof(wl_state_t));
            if (result) goto error;

        } else {
            result = wl_flash_erase_raw(wl_flash, wl_flash->addr_state1, wl_flash->state_size);
            if (result) goto error;

            result = mbl_flash_write(wl_flash->addr_state1, (uint8_t *)&sa_copy, sizeof(wl_state_t));
            if (result) goto error;

            for (i = 0; i < ((wl_flash->cfg.full_mem_size / wl_flash->cfg.sector_size) * wl_flash->cfg.wr_size / 8); i++) {
                uint8_t pos_bits = 0;
                result = mbl_flash_indirect_read(wl_flash->addr_state2 + sizeof(wl_state_t) + i, &pos_bits, 1);
                if (result) goto error;

                if (pos_bits != 0xff) {
                    result = mbl_flash_write(wl_flash->addr_state1 + sizeof(wl_state_t) + i, &pos_bits, 1);
                    if (result) goto error;
                }
            }
            result = mbl_flash_indirect_read(wl_flash->addr_state1, (uint8_t *)&wl_flash->state, sizeof(wl_state_t));
            if (result) goto error;

            wl_flash->state.pos = wl_flash->state.max_pos - 1;
        }
        if (wl_flash->state.version != wl_flash->cfg.version) {
            result = wl_flash_init_sections(wl_flash);
            if (result) goto error;
        }
    }
    return 0;
error:
    if (wl_flash->temp_buff != NULL)
        sys_mfree(wl_flash->temp_buff);
    return result;
}

/*!
    \brief      write data to flash
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[in]  dest_addr: destination address of flash to write
    \param[in]  src: pointer to data write to flash
    \param[in]  size: size of data write to flash
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
int wl_flash_write(wl_flash_t *wl_flash, uint32_t dest_addr, const uint8_t *src, size_t size)
{
    uint32_t count = 0, virt_addr = 0, virt_addr_last = 0;
    int result = 0;

    count = (size - 1) / wl_flash->cfg.page_size;

    for (uint32_t i = 0; i < count; i++) {
        virt_addr = wl_flash_calc_addr(wl_flash, dest_addr + i * wl_flash->cfg.page_size);
        ERR_CHECK(mbl_flash_write(wl_flash->cfg.start_addr + virt_addr, &((uint8_t *)src)[i * wl_flash->cfg.page_size], wl_flash->cfg.page_size))
    }

    virt_addr_last = wl_flash_calc_addr(wl_flash, dest_addr + count * wl_flash->cfg.page_size);
    ERR_CHECK(mbl_flash_write(wl_flash->cfg.start_addr + virt_addr_last, &((uint8_t *)src)[count * wl_flash->cfg.page_size], size - count * wl_flash->cfg.page_size))
    return result;
}

/*!
    \brief      read data from flash
    \param[in]  wl_flash: pointer to wl_flash_t
    \param[in]  dest_addr: source address of flash to read
    \param[in]  src: pointer to buffer to store data read from flash
    \param[in]  size: size of data to read from flash
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
int wl_flash_read(wl_flash_t *wl_flash, uint32_t src_addr, uint8_t *dest, size_t size)
{
    uint32_t count = 0;
    int result = 0;

    if (size >= 1){
        count = (size - 1) / wl_flash->cfg.page_size;
    } else {
        DEBUGPRINT("wl_flash_read: flash read len is 0\r\n");
        return -1;
    }

    for (uint32_t i = 0; i < count; i++) {
        uint32_t virt_addr = wl_flash_calc_addr(wl_flash, src_addr + i * wl_flash->cfg.page_size);
        ERR_CHECK(mbl_flash_indirect_read(wl_flash->cfg.start_addr + virt_addr, &((uint8_t *)dest)[i * wl_flash->cfg.page_size],  wl_flash->cfg.page_size))
    }

    uint32_t virt_addr_last = wl_flash_calc_addr(wl_flash, src_addr + count * wl_flash->cfg.page_size);
    ERR_CHECK(mbl_flash_indirect_read(wl_flash->cfg.start_addr + virt_addr_last, &((uint8_t *)dest)[count * wl_flash->cfg.page_size], size - count * wl_flash->cfg.page_size))

    return result;
}

/*!
    \brief      calculate crc
    \param[in]  pbuf: pointer to data need to be calculated crc
    \param[in]  buffersize: data length
    \param[out] none
    \retval     crc calculated result
*/
static uint32_t wl_calculate_crc(uint8_t *pbuf, uint32_t buffer_size)
{
    /* reset the CRC data register and calculate the CRC of the value */
    crc_data_register_reset();
    return crc_block_data_calculate((uint32_t *)pbuf, buffer_size / 4);
}
