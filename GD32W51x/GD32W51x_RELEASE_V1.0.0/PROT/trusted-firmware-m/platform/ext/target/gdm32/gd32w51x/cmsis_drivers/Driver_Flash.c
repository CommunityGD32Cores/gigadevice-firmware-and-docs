/*
 * Copyright (c) 2013-2018 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdint.h>
#include "cmsis.h"
#include "Driver_Flash.h"
#include "platform_retarget.h"
#include "RTE_Device.h"
#include "rom_export.h"
#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION      ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)
#define ARM_FLASH_DRV_ERASE_VALUE  0xFF


/*
 * ARM FLASH device structure
 *
 * There is no real flash memory for code on MPS2 board. Instead a code SRAM is
 * used for code storage: ZBT SSRAM1. This driver just emulates a flash
 * interface and behaviour on top of the SRAM memory.
 */
struct arm_flash_dev_t {
    const uint32_t memory_base;   /*!< FLASH memory base address */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0, /* event_ready */
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

static int32_t is_range_valid(struct arm_flash_dev_t *flash_dev,
                              uint32_t offset)
{
    uint32_t flash_limit = 0;
    int32_t rc = 0;

    flash_limit = (flash_dev->data->sector_count * flash_dev->data->sector_size)
                   - 1;

    if (offset > flash_limit) {
        rc = -1;
    }
    return rc;
}

static int32_t is_write_aligned(struct arm_flash_dev_t *flash_dev,
                                uint32_t param)
{
    int32_t rc = 0;

    if ((param % flash_dev->data->program_unit) != 0) {
        rc = -1;
    }
    return rc;
}

static int32_t is_sector_aligned(struct arm_flash_dev_t *flash_dev,
                                 uint32_t offset)
{
    int32_t rc = 0;

    if ((offset % flash_dev->data->sector_size) != 0) {
        rc = -1;
    }
    return rc;
}

static int32_t is_flash_ready_to_write(const uint8_t *start_addr, uint32_t cnt)
{
    int32_t rc = 0;
    uint32_t i;

    for (i = 0; i < cnt; i++) {
        if(start_addr[i] != ARM_FLASH_DRV_ERASE_VALUE) {
            rc = -1;
            break;
        }
    }

    return rc;
}

#if (RTE_FLASH_INTERNAL)
/* Delay definition */
#define EraseTimeout          ((uint32_t)0xffffffff)        // 0x000B0000
#define ProgramTimeout        ((uint32_t)0xffffffff)        // 0x00002000

static ARM_FLASH_INFO ARM_FLASH_INTERNAL_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH_INTERNAL_SIZE / FLASH_INTERNAL_SECTOR_SIZE,
    .sector_size  = FLASH_INTERNAL_SECTOR_SIZE,
    .page_size    = FLASH_INTERNAL_PAGE_SIZE,
    .program_unit = FLASH_INTERNAL_PROGRAM_UNIT,
    .erased_value = ARM_FLASH_DRV_ERASE_VALUE};

static struct arm_flash_dev_t ARM_FLASH_INTERNAL_DEV = {
#if (__DOMAIN_NS == 1)
    .memory_base = FLASH_INTERNAL_BASE_NS,
#else
    .memory_base = FLASH_INTERNAL_BASE_S,
#endif /* __DOMAIN_NS == 1 */
    .data        = &(ARM_FLASH_INTERNAL_DEV_DATA)};

struct arm_flash_dev_t *FLASH_INTERNAL_DEV = &ARM_FLASH_INTERNAL_DEV;

/*
 * Functions
 */

/*
 * \brief Translates error codes from native API to CMSIS API.
 *
 * \param[in] err  Error code to translate (\ref FLASH_Status).
 *
 * \return Returns CMSIS error code.
 */
static int32_t error_trans(fmc_state_enum err)
{
    switch(err) {
    case FMC_BUSY:
        return ARM_DRIVER_ERROR_BUSY;
    case FMC_WPERR:
    case FMC_OBERR:
    case FMC_SECERR:
        return ARM_DRIVER_ERROR;
    case FMC_READY:
        return ARM_DRIVER_OK;
    case FMC_TOERR:
        return ARM_DRIVER_ERROR_TIMEOUT;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all the enumeration values are
     *           covered in the switch.
     */
    }
}

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
        break;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t start_addr = FLASH_INTERNAL_DEV->memory_base + addr;
    int32_t rc = 0;
    int cache_enable = 0;

    /* Check flash memory boundaries */
    rc = is_range_valid(FLASH_INTERNAL_DEV, addr + cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (ICACHE_CTL & ICACHE_CTL_EN) {
        icache_disable();
        cache_enable = 1;
    }

    memcpy(data, (void *)start_addr, cnt);
    if (cache_enable)
        icache_enable();
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    uint32_t base_addr = FLASH_INTERNAL_DEV->memory_base;
    uint8_t *data_u8 = (uint8_t *)data;
    int32_t rc = 0;
    uint32_t offset_align, val32;
    int vb, act_len, i;
    uint8_t val[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid(FLASH_INTERNAL_DEV, addr + cnt);
    rc |= is_write_aligned(FLASH_INTERNAL_DEV, addr);
    rc |= is_write_aligned(FLASH_INTERNAL_DEV, cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Check if the flash area to write the data was erased previously */
    rc = is_flash_ready_to_write((const uint8_t*)base_addr + addr, cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR;
    }
    rc = rom_flash_write(addr, data, cnt);
        if (rc != 0) {
        return ARM_DRIVER_ERROR;
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    uint32_t start_addr = FLASH_INTERNAL_DEV->memory_base + addr;
    uint32_t rc = 0;

    rc  = is_range_valid(FLASH_INTERNAL_DEV, addr);
    rc |= is_sector_aligned(FLASH_INTERNAL_DEV, addr);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }
    rc = rom_flash_erase(addr, FLASH_INTERNAL_SECTOR_SIZE);
    if (rc != 0) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseChip(void)
{
    int32_t rc = ARM_DRIVER_ERROR_UNSUPPORTED;

    /* Check driver capability erase_chip bit */
    if (DriverCapabilities.erase_chip == 1) {
        /* unlock the flash program erase controller */
        rom_fmc_unlock();

        /* clear pending flag */
        rom_fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_SECERR | FMC_FLAG_WPERR);

        rc = rom_fmc_mass_erase();

        /* lock the flash program erase controller */
        rom_fmc_lock();

        rc = error_trans(rc);
    }
    return rc;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return FLASH_INTERNAL_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH_INTERNAL = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};
#endif /* RTE_FLASH_INTERNAL */

#if (RTE_FLASH_EXTERNAL)

#define WRITE_ENABLE_CMD               0x06
#define WRITE_DISABLE_CMD              0x04
#define READ_STATUS_REG1_CMD           0x05
#define WRITE_STATUS_REG1_CMD          0x01
#define READ_STATUS_REG_CMD            0x35
#define WRITE_STATUS_REG_CMD           0x31
#define QUAD_IN_FAST_PROG_CMD          0x32
#define QUAD_OUT_FAST_READ_CMD         0x6B
#define SECTOR_ERASE_CMD               0x20 // 0xD8
#define HIGH_PERFORMANCE_CMD           0xA3
#define CHIP_ERASE_CMD                 0xC7

static ARM_FLASH_INFO ARM_FLASH_EXTERNAL_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH_EXTERNAL_SIZE / FLASH_EXTERNAL_SECTOR_SIZE,
    .sector_size  = FLASH_EXTERNAL_SECTOR_SIZE,
    .page_size    = FLASH_EXTERNAL_PAGE_SIZE,
    .program_unit = FLASH_EXTERNAL_PROGRAM_UNIT,
    .erased_value = ARM_FLASH_DRV_ERASE_VALUE};

static struct arm_flash_dev_t ARM_FLASH_EXTERNAL_DEV = {
#if (__DOMAIN_NS == 1)
    .memory_base = FLASH_EXTERNAL_BASE_NS,
#else
    .memory_base = FLASH_EXTERNAL_BASE_S,
#endif /* __DOMAIN_NS == 1 */
    .data        = &(ARM_FLASH_EXTERNAL_DEV_DATA)};

struct arm_flash_dev_t *FLASH_EXTERNAL_DEV = &ARM_FLASH_EXTERNAL_DEV;

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    __disable_irq();
    rom_qspi_flash_config(3);
    __enable_irq();

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
        break;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t start_addr = FLASH_EXTERNAL_DEV->memory_base + addr;
    int32_t rc = 0;

    if (cnt == 0) {
        return ARM_DRIVER_OK;
    }

    /* Check flash memory boundaries */
    rc = is_range_valid(FLASH_EXTERNAL_DEV, addr + cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }
    __disable_irq();
    rom_qspi_flash_read(start_addr, data, cnt);
    __enable_irq();
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    uint32_t start_addr = FLASH_EXTERNAL_DEV->memory_base + addr;
    int32_t rc = 0;
    uint32_t page_size = FLASH_EXTERNAL_DEV->data->page_size;
    uint32_t page_offset = addr & (page_size - 1);
    uint32_t size_to_program;
    uint8_t *src = (uint8_t *)data;

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid(FLASH_EXTERNAL_DEV, addr + cnt);
    rc |= is_write_aligned(FLASH_EXTERNAL_DEV, addr);
    rc |= is_write_aligned(FLASH_EXTERNAL_DEV, cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (page_offset != 0) {
        size_to_program = (cnt > page_size - page_offset)? (page_size - page_offset) : cnt;
        __disable_irq();
        rom_qspi_flash_program_page(start_addr, src, size_to_program);
        __enable_irq();
        addr += size_to_program;
        src += size_to_program;
        cnt -= size_to_program;
    }

    while (cnt >= page_size) {
        start_addr = FLASH_EXTERNAL_DEV->memory_base + addr;
        __disable_irq();
        rom_qspi_flash_program_page(start_addr, src, page_size);
        __enable_irq();
        addr += page_size;
        src += page_size;
        cnt -= page_size;
    }

    if (cnt > 0) {
        start_addr = FLASH_EXTERNAL_DEV->memory_base + addr;
        __disable_irq();
        rom_qspi_flash_program_page(start_addr, src, cnt);
        __enable_irq();
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    uint32_t start_addr = FLASH_EXTERNAL_DEV->memory_base + addr;
    uint32_t rc = 0;
    rc  = is_range_valid(FLASH_EXTERNAL_DEV, addr);
    rc |= is_sector_aligned(FLASH_EXTERNAL_DEV, addr);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }
    __disable_irq();
    rom_qspi_flash_erase_sector(start_addr);
    __enable_irq();
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseChip(void)
{
    uint32_t i;
    uint32_t addr = FLASH_EXTERNAL_DEV->memory_base;
    int32_t rc = ARM_DRIVER_ERROR_UNSUPPORTED;

    /* Check driver capability erase_chip bit */
    if (DriverCapabilities.erase_chip == 1) {
        for (i = 0; i < FLASH_EXTERNAL_DEV->data->sector_count; i++) {
            __disable_irq();
            rom_qspi_flash_erase_sector(addr);
            __enable_irq();
            addr += FLASH_EXTERNAL_DEV->data->sector_size;
            rc = ARM_DRIVER_OK;
        }
    }
    return rc;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return FLASH_EXTERNAL_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH_EXTERNAL = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};
#endif /* RTE_FLASH_EXTERNAL */