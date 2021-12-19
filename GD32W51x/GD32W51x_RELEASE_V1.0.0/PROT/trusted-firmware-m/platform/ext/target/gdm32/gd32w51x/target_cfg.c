/*
 * Copyright (c) 2017-2019 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis.h"
#include "target_cfg.h"
#include "Driver_MPC.h"
#include "platform_retarget_dev.h"
#include "region_defs.h"
#include "tfm_plat_defs.h"
#include "region.h"
#include "bl2/include/tfm_boot_status.h"
#include "gd32w51x.h"
#include "string.h"
#include "rom_export.h"
#include "log/tfm_assert.h"
#include "log/tfm_log.h"
#ifdef PSA_API_TEST_IPC
#define PSA_FF_TEST_SECURE_UART2
#endif

struct rom_api_s_t *p_rom_api_s = (struct rom_api_s_t *)ROM_API_ARRAY_BASE;

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

/* Macros to pick linker symbols */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Limit);
#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

const struct memory_region_limits memory_regions = {
    .non_secure_code_start =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        BL2_HEADER_SIZE,

    .non_secure_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base),

    .non_secure_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        NS_PARTITION_SIZE - 1,

    .veneer_base = (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Base),
    .veneer_limit = (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Limit),

#ifdef BL2
    .secondary_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base),

    .secondary_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base) +
        SECONDARY_PARTITION_SIZE - 1,
#endif /* BL2 */
};

/* Allows software, via SAU, to define the code region as a NSC */
#define NSCCFG_CODENSC  1

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_SRAM1_MPC, Driver_SRAM2_MPC;

/* Define Peripherals NS address range for the platform */
#define PERIPHERALS_BASE_NS_START (0x40000000)
#define PERIPHERALS_BASE_NS_END   (0x4FFFFFFF)

/* Enable system reset request for CPU 0 */
#define ENABLE_CPU0_SYSTEM_RESET_REQUEST (1U << 4U)

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

/* Debug configuration flags */
#define SPNIDEN_SEL_STATUS (0x01u << 7)
#define SPNIDEN_STATUS     (0x01u << 6)
#define SPIDEN_SEL_STATUS  (0x01u << 5)
#define SPIDEN_STATUS      (0x01u << 4)
#define NIDEN_SEL_STATUS   (0x01u << 3)
#define NIDEN_STATUS       (0x01u << 2)
#define DBGEN_SEL_STATUS   (0x01u << 1)
#define DBGEN_STATUS       (0x01u << 0)

#define All_SEL_STATUS (SPNIDEN_SEL_STATUS | SPIDEN_SEL_STATUS | \
                        NIDEN_SEL_STATUS | DBGEN_SEL_STATUS)

struct tfm_spm_partition_platform_data_t tfm_peripheral_std_uart = {
        UART0_BASE_NS,
        UART0_BASE_NS + 0xFFF,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

// struct tfm_spm_partition_platform_data_t tfm_peripheral_uart1 = {
//         UART1_BASE_S,
//         UART1_BASE_S + 0xFFF,
//         PPC_SP_APB_PPC_EXP1,
//         CMSDK_UART1_APB_PPC_POS
// };
//
// struct tfm_spm_partition_platform_data_t tfm_peripheral_fpga_io = {
//         MPS2_IO_FPGAIO_BASE_S,
//         MPS2_IO_FPGAIO_BASE_S + 0xFFF,
//         PPC_SP_APB_PPC_EXP2,
//         CMSDK_FPGA_IO_PPC_POS
// };
//
// struct tfm_spm_partition_platform_data_t tfm_peripheral_timer0 = {
//         CMSDK_TIMER0_BASE_S,
//         CMSDK_TIMER1_BASE_S - 1,
//         PPC_SP_APB_PPC0,
//         CMSDK_TIMER0_APB_PPC_POS
// };

#ifdef PSA_API_TEST_IPC

/* Below data structure are only used for PSA FF tests, and this pattern is
 * definitely not to be followed for real life use cases, as it can break
 * security.
 */

struct tfm_spm_partition_platform_data_t
    tfm_peripheral_FF_TEST_UART_REGION = {
        UART2_BASE_S,
        UART2_BASE_S + 0xFFF,
        PPC_SP_APB_PPC_EXP2,
        CMSDK_UART2_APB_PPC_POS
};

struct tfm_spm_partition_platform_data_t
    tfm_peripheral_FF_TEST_WATCHDOG_REGION = {
        APB_WATCHDOG_BASE_S,
        APB_WATCHDOG_BASE_S + 0xFFF,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

#define FF_TEST_NVMEM_REGION_START            0x102FFC00
#define FF_TEST_NVMEM_REGION_END              0x102FFFFF
#define FF_TEST_SERVER_PARTITION_MMIO_START   0x3801FC00
#define FF_TEST_SERVER_PARTITION_MMIO_END     0x3801FD00
#define FF_TEST_DRIVER_PARTITION_MMIO_START   0x3801FE00
#define FF_TEST_DRIVER_PARTITION_MMIO_END     0x3801FF00

struct tfm_spm_partition_platform_data_t
    tfm_peripheral_FF_TEST_NVMEM_REGION = {
        FF_TEST_NVMEM_REGION_START,
        FF_TEST_NVMEM_REGION_END,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

struct tfm_spm_partition_platform_data_t
    tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO = {
        FF_TEST_SERVER_PARTITION_MMIO_START,
        FF_TEST_SERVER_PARTITION_MMIO_END,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

struct tfm_spm_partition_platform_data_t
    tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO = {
        FF_TEST_DRIVER_PARTITION_MMIO_START,
        FF_TEST_DRIVER_PARTITION_MMIO_END,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};
#endif

static int get_boot_data(uint16_t tlv_type, uint8_t* buf_start, uint8_t buf_size)
{
    uint8_t *ptr;
    struct tfm_boot_data *boot_data;
    struct shared_data_tlv_entry tlv_entry;
    uintptr_t tlv_end, offset;

    boot_data = (struct tfm_boot_data *)BOOT_TFM_SHARED_DATA_BASE;

    if (boot_data->header.tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return -1;
    }

    /* Check whether caller has access right to given tlv_major_type */
    if (GET_MAJOR(tlv_type) != TLV_MAJOR_CORE) {
        return -2;
    }

    /* Get the boundaries of TLV section */
    tlv_end = BOOT_TFM_SHARED_DATA_BASE + boot_data->header.tlv_tot_len;
    offset  = BOOT_TFM_SHARED_DATA_BASE + SHARED_DATA_HEADER_SIZE;

    /* Iterates over the TLV section and copy TLVs with requested major
     * type to the provided buffer.
     */
    for (; offset < tlv_end; offset += tlv_entry.tlv_len) {
        /* Create local copy to avoid unaligned access */
        memcpy(&tlv_entry,(const void *)offset,SHARED_DATA_ENTRY_HEADER_SIZE);
        if (tlv_entry.tlv_type == tlv_type) {
            /* Check buffer overflow */
            if (tlv_entry.tlv_len - SHARED_DATA_ENTRY_HEADER_SIZE > buf_size) {
                return -4;
            }
            memcpy(buf_start,
                    (const void *)(offset + SHARED_DATA_ENTRY_HEADER_SIZE),
                    tlv_entry.tlv_len - SHARED_DATA_ENTRY_HEADER_SIZE);
            break;
        }
    }

    if (offset >= tlv_end)
        return -5;

    return 0;
}
enum tfm_plat_err_t enable_fault_handlers(void)
{
#if 0
    /* Explicitly set secure fault priority to the highest */
    NVIC_SetPriority(SecureFault_IRQn, 0);

    /* Enables BUS, MEM, USG and Secure faults */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
                  | SCB_SHCSR_BUSFAULTENA_Msk
                  | SCB_SHCSR_MEMFAULTENA_Msk
                  | SCB_SHCSR_SECUREFAULTENA_Msk;
#endif
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t system_reset_cfg(void)
{
    // struct sysctrl_t *sysctrl = (struct sysctrl_t *)CMSDK_SYSCTRL_BASE_S;
    uint32_t reg_value = SCB->AIRCR;

    /* Enable system reset request for CPU 0, to be triggered via
     * NVIC_SystemReset function.
     */
    // sysctrl->resetmask |= ENABLE_CPU0_SYSTEM_RESET_REQUEST;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Enable system reset request only to the secure world */
    //reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK | SCB_AIRCR_SYSRESETREQS_Msk);
    reg_value &= ~(uint32_t)(SCB_AIRCR_SYSRESETREQS_Msk);
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK);

    SCB->AIRCR = reg_value;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t init_debug(void)
{
    // volatile struct sysctrl_t *sys_ctrl = (struct sysctrl_t *)CMSDK_SYSCTRL_BASE_S;

#if defined(DAUTH_NONE)
    /* Set all the debug enable selector bits to 1 */
    sys_ctrl->secdbgset = All_SEL_STATUS;
    /* Set all the debug enable bits to 0 */
    sys_ctrl->secdbgclr =
                   DBGEN_STATUS | NIDEN_STATUS | SPIDEN_STATUS | SPNIDEN_STATUS;
#elif defined(DAUTH_NS_ONLY)
    /* Set all the debug enable selector bits to 1 */
    sys_ctrl->secdbgset = All_SEL_STATUS;
    /* Set the debug enable bits to 1 for NS, and 0 for S mode */
    sys_ctrl->secdbgset = DBGEN_STATUS | NIDEN_STATUS;
    sys_ctrl->secdbgclr = SPIDEN_STATUS | SPNIDEN_STATUS;
#elif defined(DAUTH_FULL)
    /* Set all the debug enable selector bits to 1 */
    sys_ctrl->secdbgset = All_SEL_STATUS;
    /* Set all the debug enable bits to 1 */
    sys_ctrl->secdbgset =
                   DBGEN_STATUS | NIDEN_STATUS | SPIDEN_STATUS | SPNIDEN_STATUS;
#else

#if !defined(DAUTH_CHIP_DEFAULT)
#error "No debug authentication setting is provided."
#endif

    /* Set all the debug enable selector bits to 0 */
    // sys_ctrl->secdbgclr = All_SEL_STATUS;

    /* No need to set any enable bits because the value depends on
     * input signals.
     */
#endif
    return TFM_PLAT_ERR_SUCCESS;
}

/*----------------- NVIC interrupt target state to NS configuration ----------*/
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void)
{
    /* Target every interrupt to NS; unimplemented interrupts will be WI */
    for (uint8_t i = 0; i < ARRAY_SIZE(NVIC->ITNS); i++) {
        NVIC->ITNS[i] = 0xFFFFFFFF;
    }

#if 0
    /* Make sure that MPC and PPC are targeted to S state */
    NVIC_ClearTargetState(MPC_IRQn);
    NVIC_ClearTargetState(PPC_IRQn);
#endif

#ifdef SECURE_UART1
    /* UART1 is a secure peripheral, so its IRQs have to target S state */
    NVIC_ClearTargetState(UARTRX1_IRQn);
    NVIC_ClearTargetState(UARTTX1_IRQn);
    NVIC_ClearTargetState(UART1_IRQn);
#endif

    return TFM_PLAT_ERR_SUCCESS;
}

/*----------------- NVIC interrupt enabling for S peripherals ----------------*/
enum tfm_plat_err_t nvic_interrupt_enable(void)
{
    // struct spctrl_def* spctrl = CMSDK_SPCTRL;
    int32_t ret = ARM_DRIVER_OK;

    /* MPC interrupt enabling */
    ret = Driver_SRAM1_MPC.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    ret = Driver_SRAM2_MPC.EnableInterrupt();
    if (ret != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    // NVIC_EnableIRQ(MPC_IRQn);

    /* PPC interrupt enabling */
    /* Clear pending PPC interrupts */
    /* In the PPC configuration function, we have used the Non-Secure
     * Privilege Control Block to grant unprivilged NS access to some
     * peripherals used by NS. That triggers a PPC0 exception as that
     * register is meant for NS privileged access only. Clear it here
     */
    // spctrl->secppcintclr = CMSDK_APB_PPC0_INT_POS_MASK;

    /* Enable PPC interrupts for APB PPC */
    // spctrl->secppcinten |= CMSDK_APB_PPC0_INT_POS_MASK |
    //                        CMSDK_APB_PPC1_INT_POS_MASK |
    //                        CMSDK_APB_PPCEXP0_INT_POS_MASK |
    //                        CMSDK_APB_PPCEXP1_INT_POS_MASK |
    //                        CMSDK_APB_PPCEXP2_INT_POS_MASK |
    //                        CMSDK_APB_PPCEXP3_INT_POS_MASK;

    // NVIC_EnableIRQ(PPC_IRQn);

    return TFM_PLAT_ERR_SUCCESS;
}

/*------------------- SAU/IDAU configuration functions -----------------------*/
struct sau_cfg_t {
    uint32_t RBAR;
    uint32_t RLAR;
    bool nsc;
};

const struct sau_cfg_t sau_cfg[] = {
    {
        ((uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base)),
        ((uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        NS_PARTITION_SIZE - 1),
        false,
    },
    {
        NS_DATA_START,
        NS_DATA_LIMIT,
        false,
    },
    {
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Base),
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Limit),
        true,
    },
    {
        PERIPHERALS_BASE_NS_START,
#ifdef SECURE_UART1
        (UART1_BASE_NS - 1),
		false,
    },
    {
        UART2_BASE_NS,
#endif
        PERIPHERALS_BASE_NS_END,
		false,
    }
#ifdef BL2
    ,
    {
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base),
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base) +
        SECONDARY_PARTITION_SIZE - 1,
		false,
    }
#endif
};

void sau_and_idau_cfg(void)
{
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    int32_t i;
    int32_t rc;
    uint32_t mbl_start, mbl_end;

    /* Enables SAU */
    TZ_SAU_Enable();

    for (i = 0; i < ARRAY_SIZE(sau_cfg); i++) {
        SAU->RNR = i;
        SAU->RBAR = sau_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
		SAU->RLAR = (sau_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                    (sau_cfg[i].nsc ? SAU_RLAR_NSC_Msk : 0U) |
                    SAU_RLAR_ENABLE_Msk;
    }
    rc = get_boot_data((uint16_t)SET_TLV_TYPE(TLV_MAJOR_CORE, SET_CORE_MINOR(SYS_SAU, SAU_ENTRY_START)),
                        (uint8_t *)&mbl_start, sizeof(mbl_start));
    rc |= get_boot_data((uint16_t)SET_TLV_TYPE(TLV_MAJOR_CORE, SET_CORE_MINOR(SYS_SAU, SAU_ENTRY_END)),
                        (uint8_t *)&mbl_end, sizeof(mbl_end));
    if (rc) {
        LOG_MSG("Failed to get mbl sau setting");
    }else{
        SAU->RNR = i;
        SAU->RBAR = mbl_start & SAU_RBAR_BADDR_Msk;
        SAU->RLAR = mbl_end | SAU_RLAR_ENABLE_Msk |
                    SAU_RLAR_NSC_Msk;
    }
    /* Allows SAU to define the code region as a NSC */
    spctrl->nsccfg |= NSCCFG_CODENSC;
}

/*------------------- Memory configuration functions -------------------------*/

int32_t mpc_init_cfg(void)
{
#if 0
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_SRAM1_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = Driver_SRAM1_MPC.ConfigRegion(
                                      memory_regions.non_secure_partition_base,
                                      memory_regions.non_secure_partition_limit,
                                      ARM_MPC_ATTR_NONSECURE);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

#ifdef BL2
    /* Secondary image region */
    ret = Driver_SRAM1_MPC.ConfigRegion(memory_regions.secondary_partition_base,
                                  memory_regions.secondary_partition_limit,
                                  ARM_MPC_ATTR_NONSECURE);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }
#endif /* BL2 */

    ret = Driver_SRAM2_MPC.Initialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = Driver_SRAM2_MPC.ConfigRegion(NS_DATA_START, NS_DATA_LIMIT,
                                        ARM_MPC_ATTR_NONSECURE);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Lock down the MPC configuration */
    ret = Driver_SRAM1_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = Driver_SRAM2_MPC.LockDown();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Add barriers to assure the MPC configuration is done before continue
     * the execution.
     */
    __DSB();
    __ISB();

#endif
    return ARM_DRIVER_OK;
}

/*---------------------- PPC configuration functions -------------------------*/

void ppc_init_cfg(void)
{
#if 0
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    struct nspctrl_def* nspctrl = CMSDK_NSPCTRL;

    /* Grant non-secure access to peripherals in the PPC0
     * (timer0 and 1, dualtimer, watchdog, mhu 0 and 1)
     */
     spctrl->apbnsppc0 |= (1U << CMSDK_TIMER0_APB_PPC_POS) |
                          (1U << CMSDK_TIMER1_APB_PPC_POS) |
                          (1U << CMSDK_DTIMER_APB_PPC_POS) |
                          (1U << CMSDK_MHU0_APB_PPC_POS) |
                          (1U << CMSDK_MHU1_APB_PPC_POS);
    /* Grant non-secure access to S32K Timer in PPC1*/
    spctrl->apbnsppc1 |= (1U << CMSDK_S32K_TIMER_PPC_POS);
    /* Grant non-secure access for APB peripherals on EXP1 */
    spctrl->apbnsppcexp1 |= (1U << CMSDK_SPI0_APB_PPC_POS) |
                            (1U << CMSDK_SPI1_APB_PPC_POS) |
                            (1U << CMSDK_SPI2_APB_PPC_POS) |
                            (1U << CMSDK_SPI3_APB_PPC_POS) |
                            (1U << CMSDK_SPI4_APB_PPC_POS) |
                            (1U << CMSDK_UART0_APB_PPC_POS) |
#ifdef SECURE_UART1
    /* To statically configure a peripheral as secure, skip PPC NS peripheral
     * configuration for the given device.
     */
#else
                            (1U << CMSDK_UART1_APB_PPC_POS) |
#endif
                            (1U << CMSDK_UART2_APB_PPC_POS) |
                            (1U << CMSDK_UART3_APB_PPC_POS) |
                            (1U << CMSDK_UART4_APB_PPC_POS) |
                            (1U << CMSDK_I2C0_APB_PPC_POS) |
                            (1U << CMSDK_I2C1_APB_PPC_POS) |
                            (1U << CMSDK_I2C2_APB_PPC_POS) |
                            (1U << CMSDK_I2C3_APB_PPC_POS);
    /* Grant non-secure access for APB peripherals on EXP2 */
    spctrl->apbnsppcexp2 |= (1U << CMSDK_FPGA_SCC_PPC_POS) |
                            (1U << CMSDK_FPGA_AUDIO_PPC_POS) |
                            (1U << CMSDK_FPGA_IO_PPC_POS);

    /* Grant non-secure access to all peripherals on AHB EXP:
     * Make sure that all possible peripherals are enabled by default
     */
    spctrl->ahbnsppcexp0 |= (1U << CMSDK_VGA_PPC_POS) |
                            (1U << CMSDK_GPIO0_PPC_POS) |
                            (1U << CMSDK_GPIO1_PPC_POS) |
                            (1U << CMSDK_GPIO2_PPC_POS) |
                            (1U << CMSDK_GPIO3_PPC_POS) |
                            (1U << MPS2_ETHERNET_PPC_POS);

    spctrl->ahbnsppcexp1 |= (1U << CMSDK_DMA0_PPC_POS) |
                            (1U << CMSDK_DMA1_PPC_POS) |
                            (1U << CMSDK_DMA2_PPC_POS) |
                            (1U << CMSDK_DMA3_PPC_POS);

    /* in NS, grant un-privileged for UART0 */
    nspctrl->apbnspppcexp1 |= (1U << CMSDK_UART0_APB_PPC_POS);

    /* in NS, grant un-privileged access for LEDs */
    nspctrl->apbnspppcexp2 |= (1U << CMSDK_FPGA_SCC_PPC_POS) |
                              (1U << CMSDK_FPGA_IO_PPC_POS);

    /* Configure the response to a security violation as a
     * bus error instead of RAZ/WI
     */
    spctrl->secrespcfg |= 1U;
#endif
}

void ppc_configure_to_non_secure(enum ppc_bank_e bank, uint16_t pos)
{
#if 0
    /* Setting NS flag for peripheral to enable NS access */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbnsppc0))[bank] |= (1U << pos);
#endif
}

void ppc_configure_to_secure(enum ppc_bank_e bank, uint16_t pos)
{
#if 0
    /* Clear NS flag for peripheral to prevent NS access */
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbnsppc0))[bank] &= ~(1U << pos);
#endif
}

void ppc_en_secure_unpriv(enum ppc_bank_e bank, uint16_t pos)
{
#if 0
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbspppc0))[bank] |= (1U << pos);
#endif
}

void ppc_clr_secure_unpriv(enum ppc_bank_e bank, uint16_t pos)
{
#if 0
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    ((uint32_t*)&(spctrl->ahbspppc0))[bank] &= ~(1U << pos);
#endif
}

void ppc_clear_irq(void)
{
#if 0
    struct spctrl_def* spctrl = CMSDK_SPCTRL;
    /* Clear APB PPC EXP2 IRQ */
    spctrl->secppcintclr = CMSDK_APB_PPCEXP2_INT_POS_MASK;
#endif
}

void flash_wm_config(uint32_t wmidx, uint32_t start_page, uint32_t end_page)
{
	/* unlock the flash program erase controller */
	fmc_unlock();
	/* unlock the option byte operation (include SECWM/HDP/WRP/NODEC/OFRG/OFVR) */
	ob_unlock();
	/* clear pending flags */
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_SECERR | FMC_FLAG_WPERR);

	ob_secmark_config(start_page, end_page, wmidx);

	/* lock the option byte operation */
	ob_lock();
	/* lock the flash program erase controller */
	fmc_lock();
}
void flash_nodec_config(uint32_t nd_idx, uint32_t start_page, uint32_t end_page)
{
	if (OBSTAT_NQSPI() == SET) {
		/* unlock the flash program erase controller */
		fmc_unlock();
		/* unlock the option byte operation (include SECWM/HDP/WRP/NODEC/OFRG/OFVR) */
		ob_unlock();
		/* clear pending flags */
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_SECERR | FMC_FLAG_WPERR);

		/* set no OTFDEC region for sip flash */
		fmc_no_rtdec_config(start_page, end_page, nd_idx);

		/* lock the option byte operation */
		ob_lock();
		/* lock the flash program erase controller */
		fmc_lock();
	}
}
int flash_secure_config(void)
{
    uint32_t running_idx;
    tzpcu_non_secure_mark_struct wm;
    int ret = get_boot_data((uint16_t)SET_TLV_TYPE(TLV_MAJOR_CORE, SET_CORE_MINOR(SYS_STATUS, STATUS_RUNNING_IMG)),
                        (uint8_t *)&running_idx, sizeof(running_idx));


    if (ret) {
        LOG_MSG("Failed to get running image");
        return ARM_DRIVER_ERROR;
    }
    if (running_idx == IMAGE_1) {
        if (OBSTAT_NQSPI() == SET) {
            if (OBSTAT_FMCOB()) {
	            flash_wm_config(2, 0, (RE_IMG_1_NSPE_OFFSET >> 12) - 1);
	            flash_wm_config(3, RE_SST_OFFSET >> 12, (rom_flash_total_size() >> 12) - 1);
            } else {
	            flash_wm_config(0, 0, (RE_IMG_1_NSPE_OFFSET >> 12) - 1);
	            flash_wm_config(1, RE_SST_OFFSET >> 12, (rom_flash_total_size() >> 12) - 1);
            }
        } else {
            wm.memory_type = QSPIFMEM;
            wm.region_number = NON_SECURE_MARK_REGION0;
            wm.start_address = RE_IMG_1_NSPE_OFFSET;
            wm.length = RE_SST_OFFSET - RE_IMG_1_NSPE_OFFSET;
            tzpcu_tzspc_emnsm_config(&wm);
        }
    } else {
        if (OBSTAT_NQSPI() == SET) {
            if (OBSTAT_FMCOB()) {
	            flash_wm_config(2, 0, (RE_IMG_0_NSPE_OFFSET >> 12) - 1);
	            flash_wm_config(3, RE_IMG_1_PROT_OFFSET >> 12, (rom_flash_total_size() >> 12) - 1);
            } else {
	            flash_wm_config(0, 0, (RE_IMG_0_NSPE_OFFSET >> 12) - 1);
	            flash_wm_config(1, RE_IMG_1_PROT_OFFSET >> 12, (rom_flash_total_size() >> 12) - 1);
            }
		} else {
            wm.memory_type = QSPIFMEM;
            wm.region_number = NON_SECURE_MARK_REGION0;
            wm.start_address = RE_IMG_0_NSPE_OFFSET;
            wm.length = RE_IMG_1_PROT_OFFSET - RE_IMG_0_NSPE_OFFSET;
            tzpcu_tzspc_emnsm_config(&wm);
        }
    }
    flash_nodec_config(1, RE_SST_OFFSET >> 12, (rom_flash_total_size() >> 12) - 1);
    return ARM_DRIVER_OK;
}

void sram_mpcbb_config(void)
{
    static const uint32_t groups[] = {TZBMPC0, TZBMPC1, TZBMPC2, TZBMPC3};
    static const uint32_t sizes[] = {0, 0x10000, 0x20000, 0x40000, 0x70000};
    const uint32_t address = RE_NSPE_DATA_START;
    uint32_t start_block;
    uint32_t end_block;

    TFM_ASSERT(address < sizes[ARRAY_SIZE(groups) - 1]);
    TFM_ASSERT((address & 0xFF) == 0);

    for (int32_t i = ARRAY_SIZE(groups) - 1; i >= 0; i--) {
        if (address >= sizes[i + 1]) {
            continue;
        }
        start_block = address > sizes[i] ? (address - sizes[i]) >> 8 : 0;
        end_block = (sizes[i + 1] - sizes[i]) >> 8;
        for (uint32_t j = start_block; j < end_block; j++) {
            tzpcu_tzbmpc_block_secure_access_mode_config(groups[i], j, BLOCK_SECURE_ACCESS_MODE_NSEC);
        }
    }
}

void gpio_nonsecure_config(void)
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
