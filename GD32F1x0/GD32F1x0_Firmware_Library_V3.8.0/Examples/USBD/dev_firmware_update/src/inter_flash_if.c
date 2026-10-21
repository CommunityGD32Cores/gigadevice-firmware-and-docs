/*!
    \file    inter_flash_if.c
    \brief   USB DFU device flash interface functions

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

#include "inter_flash_if.h"

/* local function prototypes ('static') */
static uint8_t  flash_if_init(void);
static uint8_t  flash_if_deinit(void);
static uint8_t  flash_if_erase(uint32_t addr);
static uint8_t  flash_if_write(uint8_t *buf, uint32_t addr, uint32_t len);
static uint8_t *flash_if_read(uint8_t *buf, uint32_t addr, uint32_t len);
static uint8_t  flash_if_checkaddr(uint32_t addr);
static fmc_state_enum inter_flash_ready_wait(uint32_t timeout);
static fmc_state_enum inter_flash_state_get(void);

dfu_mem_prop dfu_inter_flash_cb = {
    (const uint8_t *)INTER_FLASH_IF_STR,

    flash_if_init,
    flash_if_deinit,
    flash_if_erase,
    flash_if_write,
    flash_if_read,
    flash_if_checkaddr,
    60U, /* flash erase timeout in ms */
    80U  /* flash programming timeout in ms (80us * RAM Buffer size (1024 Bytes) */
};

/*!
    \brief      program option byte
    \param[in]  Mem_Add: target address
    \param[in]  data: pointer to target data
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
fmc_state_enum option_byte_write(uint32_t mem_add, uint8_t *data)
{
    fmc_state_enum status;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_END);

    status = inter_flash_ready_wait(FMC_TIMEOUT_COUNT);

    /* authorize the small information block programming */
    ob_unlock();

    /* start erase the option byte */
    FMC_CTL |= FMC_CTL_OBER;
    FMC_CTL |= FMC_CTL_START;

    status = inter_flash_ready_wait(FMC_TIMEOUT_COUNT);

    FMC_CTL &= ~FMC_CTL_OBER;
    /* set the OBPG bit */
    FMC_CTL |= FMC_CTL_OBPG;

    /*option bytes always have 16 bytes*/
    for(uint8_t index = 0U; index < 15U; index = index + 2U) {
        *(__IO uint16_t *)mem_add = data[index] & 0xffU;

        mem_add = mem_add + 2U;

        status = inter_flash_ready_wait(FMC_TIMEOUT_COUNT);
    }

    /* if the program operation is completed, disable the OBPG Bit */
    FMC_CTL &= ~FMC_CTL_OBPG;

    fmc_lock();

    return status;
}

/*!
    \brief      flash memory interface initialization routine
    \param[in]  none
    \param[out] none
    \retval     MEM_OK if the operation is right, MEM_FAIL else
*/
static uint8_t flash_if_init(void)
{
    /* unlock the internal flash */
    fmc_unlock();

    return MEM_OK;
}

/*!
    \brief      flash memory interface deinitialization routine
    \param[in]  none
    \param[out] none
    \retval     MEM_OK if the operation is right, MEM_FAIL else
*/
static uint8_t flash_if_deinit(void)
{
    /* lock the internal flash */
    fmc_lock();

    return MEM_OK;
}

/*!
    \brief      erase flash sector
    \param[in]  addr: flash address to be erased
    \param[out] none
    \retval     MEM_OK if the operation is right, MEM_FAIL else
*/
static uint8_t flash_if_erase(uint32_t addr)
{
    /* unlock the internal flash */
    fmc_unlock();

    fmc_page_erase(addr);

    /* lock the internal flash */
    fmc_lock();

    return MEM_OK;
}

/*!
    \brief      flash memory write routine
    \param[in]  buf: data buffer pointer
    \param[in]  addr: flash address to be written
    \param[in]  len: length of data to be written (in bytes)
    \param[out] none
    \retval     MEM_OK if the operation is right, MEM_FAIL else
*/
static uint8_t flash_if_write(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint32_t idx = 0U;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* not an aligned data */
    if(len & 0x03U) {
        for(idx = len; idx < ((len & 0xFFFCU) + 4U); idx++) {
            buf[idx] = 0xFFU;
        }
    }

    /* data received are word multiple */
    for(idx = 0U; idx < len; idx += 4U) {
        fmc_word_program(addr, *(uint32_t *)(buf + idx));
        addr += 4U;
    }

    fmc_lock();

    return MEM_OK;
}

/*!
    \brief      flash memory read routine
    \param[in]  buf: data buffer pointer
    \param[in]  addr: flash address to be read from
    \param[in]  len: length of data to be read (in bytes)
    \param[out] none
    \retval     pointer to the physical address where data should be read
*/
static uint8_t *flash_if_read(uint8_t *buf, uint32_t addr, uint32_t len)
{
    return (uint8_t *)(addr);
}

/*!
    \brief      check if the address is an allowed address for this memory
    \param[in]  addr: flash address to be checked
    \param[out] none
    \retval     MEM_OK if the operation is right, MEM_FAIL else
*/
static uint8_t flash_if_checkaddr(uint32_t addr)
{
    if((addr >= FLASH_START_ADDR) && (addr < FLASH_END_ADDR)) {
        return MEM_OK;
    } else {
        return MEM_FAIL;
    }
}

/*!
    \brief      check whether FMC is ready or not
    \param[in]  timeout: timeout count
    \param[out] none
    \retval     fmc_state
*/
static fmc_state_enum inter_flash_ready_wait(uint32_t timeout)
{
    fmc_state_enum fmc_state = FMC_BUSY;

    /* wait for FMC ready */
    do {
        /* get FMC state */
        fmc_state = inter_flash_state_get();
        timeout--;
    } while((FMC_BUSY == fmc_state) && (0U != timeout));

    if(FMC_BUSY == fmc_state) {
        fmc_state = FMC_TOERR;
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      get the FMC state
    \param[in]  none
    \param[out] none
    \retval     fmc_state
*/
static fmc_state_enum inter_flash_state_get(void)
{
    fmc_state_enum fmc_state = FMC_READY;

    if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_BUSY)) {
        fmc_state = FMC_BUSY;
    } else {
        if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_WPERR)) {
            fmc_state = FMC_WPERR;
        } else {
            if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_PGERR)) {
                fmc_state = FMC_PGERR;
            }
        }
    }
    /* return the FMC state */
    return fmc_state;
}
