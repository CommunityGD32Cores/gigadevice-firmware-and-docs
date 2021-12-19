/*!
    \file    mbl_target_config.c
    \brief   MBL target configuration for GD32W51x WiFi SDK

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

#include "mbl_includes.h"
#include "rom_flash.h"

#if defined PLATFORM_GDM32
#if 0
enum {
    NS_REGION_CODE_1 = 0,
    NS_REGION_CODE_2,
    NS_REGION_DATA,
    NS_REGION_CMSE_1,  /* NSC */
    NS_REGION_CMSE_2,  /* NSC */
    NS_REGION_PERIPH,
    NS_REGION_USER,
};
/*!
    \brief      configure sau and idau
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void sau_and_idau_cfg(void)
{
  SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
               ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

  FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
               ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos) & FPU_FPCCR_TS_Msk) |
               ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
               ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos) & FPU_FPCCR_CLRONRET_Msk);

  /* Disable SAU */
  TZ_SAU_Disable();

#ifdef CODE_FROM_FLASH
  /* Configures SAU regions to be non-secure */
  /* CODE */
  SAU->RNR  = NS_REGION_CODE_1;
  SAU->RBAR = ((FLASH_BASE_NS + FLASH_AREA_1_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((FLASH_BASE_NS + FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk;

  SAU->RNR  = NS_REGION_CODE_2;
  SAU->RBAR = ((FLASH_BASE_NS + FLASH_AREA_3_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((FLASH_BASE_NS + FLASH_AREA_3_OFFSET + FLASH_AREA_3_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk;

  /* DATA */
  SAU->RNR  = NS_REGION_DATA;
  SAU->RBAR = ((RAM_BASE_NS + RAM_BANK3_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((RAM_BASE_NS + RAM_BANK3_OFFSET + RAM_BANK3_SIZE + RAM_BANK4_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk;

  /* Configures veneers region to be non-secure callable */
  SAU->RNR  = NS_REGION_CMSE_1;
  SAU->RBAR = ((FLASH_BASE_S + FLASH_AREA_0_CMSE_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((FLASH_BASE_S + FLASH_AREA_0_CMSE_OFFSET + FLASH_AREA_0_CMSE_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk
              | SAU_RLAR_NSC_Msk;

  SAU->RNR  = NS_REGION_CMSE_2;
  SAU->RBAR = ((FLASH_BASE_S + FLASH_AREA_2_CMSE_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((FLASH_BASE_S + FLASH_AREA_2_CMSE_OFFSET + FLASH_AREA_2_CMSE_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk
              | SAU_RLAR_NSC_Msk;
#elif defined (CODE_FROM_SRAM)
  /* Configures SAU regions to be non-secure */
  /* CODE */
  SAU->RNR  = NS_REGION_CODE_1;
  SAU->RBAR = ((RAM_BASE_NS + RAM_BANK4_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((RAM_BASE_NS + RAM_BANK4_OFFSET + RAM_BANK4_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk;

  /* DATA */
  SAU->RNR  = NS_REGION_DATA;
  SAU->RBAR = ((RAM_BASE_NS + RAM_BANK2_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((RAM_BASE_NS + RAM_BANK2_OFFSET + RAM_BANK2_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk;

  /* Configures veneers region to be non-secure callable */
  SAU->RNR  = NS_REGION_CMSE_1;
  SAU->RBAR = ((RAM_BASE_S + RAM_BANK1_CMSE_OFFSET)
               & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((RAM_BASE_S + RAM_BANK1_CMSE_OFFSET + RAM_BANK1_CMSE_SIZE - 1)
               & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk
              | SAU_RLAR_NSC_Msk;
#endif

  /* Configure the peripherals space */
  SAU->RNR  = NS_REGION_PERIPH;
  SAU->RBAR = (PERIPH_BASE_NS & SAU_RBAR_BADDR_Msk);
  SAU->RLAR = ((PERIPH_BASE_NS + 0xFFFFFFF) & SAU_RLAR_LADDR_Msk)
              | SAU_RLAR_ENABLE_Msk;

  /* Force memory writes before continuing */
  __DSB();
  /* Flush and refill pipeline with updated permissions */
  __ISB();

  /* Enable SAU */
  TZ_SAU_Enable();

  /* Lock SAU config */
  //FIXME
  //__HAL_RCC_SYSCFG_CLK_ENABLE();
  //SYSCFG->CSLCKR |= SYSCFG_CSLCKR_LOCKSAU;
}
#endif
/* NVIC interrupt target state configuration */
/*!
    \brief      configure nvic interrupts
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_nvic_its_config(void)
{
    uint32_t sz;
    int i;

    sz = sizeof(NVIC->ITNS)/sizeof(NVIC->ITNS[0]);
    for (i = 0; i < sz; i++) {
        NVIC->ITNS[i] = 0xFFFFFFFF;
    }
}
/*!
    \brief      configure gpio attribute
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_gpio_attr_config(void)
{
    /* RFIO pins */
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_9);
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_10);
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_11);
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_12);

    /* USART pins */
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_8);
    gpio_bit_reset_sec_cfg(GPIOB, GPIO_PIN_15);
}
/*!
    \brief      configure sram block secure access mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_sram_mpcbb_config(void)
{
#ifndef ROM_SELF_TEST
    static const uint32_t groups[] = {TZBMPC0, TZBMPC1, TZBMPC2, TZBMPC3};
    static const uint32_t sizes[] = {0, 0x10000, 0x20000, 0x40000, 0x70000}; /* Start addresses of 4 SRAMs */
    const uint32_t address = RE_NSPE_DATA_START;
    uint32_t start_block, end_block, array_sz;
    int32_t i;

    array_sz = (sizeof(groups)/sizeof(groups[0]));
    for (i = array_sz - 1; i >= 0; i--) {
        if (address >= sizes[i + 1]) {
            continue;
        }
        start_block = address > sizes[i] ? (address - sizes[i]) >> 8 : 0;
        end_block = (sizes[i + 1] - sizes[i]) >> 8;
        for (uint32_t j = start_block; j < end_block; j++) {
            tzpcu_tzbmpc_block_secure_access_mode_config(groups[i], j, BLOCK_SECURE_ACCESS_MODE_NSEC);
        }
    }
#else
    int32_t i;
    uint32_t blk_num = (RE_NSPE_DATA_START >> 8);
    for (i = blk_num; i < 768; i++) {  /* SRAM4: 192K */
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC3, i, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
    tzgpc_tzbmpc_secure_illegal_access_disable_config(TZBMPC3, SECURE_ILLEGAL_ACCESS_DISABLE);
#endif
}

/*!
    \brief      initialize flash water mark secure pages
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_flash_wm_init(void)
{
    if (IS_SIP_FLASH()) {
        if (OBSTAT_FMCOB()) {
            /* unlock the flash program erase controller */
            fmc_unlock();
            ob_unlock();

            /* clear pending flags */
            fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_SECERR | FMC_FLAG_WPERR);

            /* Set MBL region as secure */
            ob_secmark_config(0, 0x3FF, SECM_INDEX2);

            /* lock the flash program erase controller */
            ob_lock();
            fmc_lock();
        }
    }
}

/*!
    \brief      configure flash water secure pages
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_flash_wm_config(uint8_t boot_idx)
{
    if (IS_SIP_FLASH()) {
        uint32_t ota_spage, ota_epage;
        if (boot_idx == IMAGE_0) {
            ota_spage = (RE_IMG_1_PROT_OFFSET >> 12);
            ota_epage = (RE_IMG_1_END_OFFSET >> 12) - 1;
        } else {
            ota_spage = (RE_IMG_0_PROT_OFFSET >> 12);
            ota_epage = (RE_IMG_1_PROT_OFFSET >> 12) - 1;
        }

        /* unlock the flash program erase controller */
        fmc_unlock();
        ob_unlock();

        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_SECERR | FMC_FLAG_WPERR);

        if (OBSTAT_FMCOB()) {
            /* Set MBL region as secure */
            ob_secmark_config(0, ((RE_IMG_0_NSPE_OFFSET >> 12) - 1), SECM_INDEX2);
            /* Set IMAGE-not-running region as secure */
            ob_secmark_config(ota_spage, ota_epage, SECM_INDEX3);
        } else {
            /* Set MBL region as secure */
            ob_secmark_config(0, ((RE_IMG_0_NSPE_OFFSET >> 12) - 1), SECM_INDEX0);
            /* Set IMAGE-not-running region as secure */
            ob_secmark_config(ota_spage, ota_epage, SECM_INDEX1);
            ob_secmark_config(0x3FF, 0, SECM_INDEX2);
            ob_secmark_config(0x3FF, 0, SECM_INDEX3);
        }
        /* lock the flash program erase controller */
        ob_lock();
        fmc_lock();
    } else {
        tzpcu_non_secure_mark_struct wm;

        wm.memory_type = QSPIFMEM;
        wm.region_number = NON_SECURE_MARK_REGION0;
        if (boot_idx == IMAGE_0) {
            wm.start_address = RE_IMG_0_NSPE_OFFSET;
            wm.length = RE_IMG_1_PROT_OFFSET - RE_IMG_0_NSPE_OFFSET;
        } else {
            wm.start_address = RE_IMG_1_NSPE_OFFSET;
            wm.length = RE_IMG_1_END_OFFSET - RE_IMG_1_NSPE_OFFSET;
        }
        tzpcu_tzspc_emnsm_config(&wm);
    }
}

/*!
    \brief      configure system reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_system_reset_config(void)
{
    uint32_t reg_value;

    //struct sysctrl_t *sysctrl = (struct sysctrl_t *)CMSDK_SYSCTRL_BASE_S;
    reg_value = SCB->AIRCR;

    /* Enable system reset request for CPU 0, to be triggered via
     * NVIC_SystemReset function.
     */
    //sysctrl->resetmask |= ENABLE_CPU0_SYSTEM_RESET_REQUEST;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Enable system reset request only to the secure world */
    //reg_value |= (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQS_Msk | SCB_AIRCR_PRIS_Msk);
    reg_value &= ~(uint32_t)(SCB_AIRCR_SYSRESETREQS_Msk);
    reg_value |= (uint32_t)(0x5FAUL << SCB_AIRCR_VECTKEY_Pos);

    SCB->AIRCR = reg_value;
}

/*!
    \brief      setup secure for non-secure processing environment
    \param[in]  boot_idx: boot index
    \param[out] none
    \retval     result of setup secure for nspe
*/
int mbl_sec_for_nspe_setup(uint8_t boot_idx)
{
    /* System reset options */
    mbl_system_reset_config();

    //__enable_irq();

    /* GPIO: set gpios used by WIFI to non-secure */
    mbl_gpio_attr_config();

    /* Flash: only first ten pages(40KB) are secure */
    mbl_flash_wm_config(boot_idx);

    /* SRAM: only first ten blocks(0xA00 Bytes) are secure */
    mbl_sram_mpcbb_config();

    /* Target every interrupt to NS; unimplemented interrupts will be WI */
    mbl_nvic_its_config();

    rom_trace_ex(ROM_ALWAYS, "Setup security settings for NSPE OK. \r\n");

    return 0;
}

/*!
    \brief      mapping flash offset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void flash_offset_mapping(void)
{
    fmc_unlock();
    ob_unlock();
    fmc_offset_region_config(RE_IMG_0_PROT_OFFSET >> 12, (RE_IMG_1_PROT_OFFSET >> 12) - 1);
    fmc_offset_value_config((RE_IMG_1_PROT_OFFSET - RE_IMG_0_PROT_OFFSET) >> 12);
    ob_lock();
    fmc_lock();
}

#else
int mbl_sec_for_nspe_setup(void)
{
}
/*!
    \brief      mapping flash offset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void flash_offset_mapping(void)
{
}
#endif
