/***************************************************************************//**
* \file system_psoc6_cm0plus.c
* \version 2.60
*
* The device system-source file.
*
********************************************************************************
* \copyright
* Copyright 2016-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
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
*******************************************************************************/

#include <stdbool.h>
#include "system_psoc6.h"
#include "cycfg.h"
#include "cy_device.h"
#include "cy_device_headers.h"
#include "cy_syslib.h"
#include "cy_wdt.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

#if !defined(CY_IPC_DEFAULT_CFG_DISABLE)
    #include "cy_ipc_sema.h"
    #include "cy_ipc_pipe.h"
    #include "cy_ipc_drv.h"

    #if defined(CY_DEVICE_PSOC6ABLE2)
        #include "cy_flash.h"
    #endif /* defined(CY_DEVICE_PSOC6ABLE2) */

#endif /* !defined(CY_IPC_DEFAULT_CFG_DISABLE) */


/*******************************************************************************
* SystemCoreClockUpdate()
*******************************************************************************/

/** Default HFClk frequency in Hz */
#define CY_CLK_HFCLK0_FREQ_HZ_DEFAULT       (8000000UL)

/** Default PeriClk frequency in Hz */
#define CY_CLK_PERICLK_FREQ_HZ_DEFAULT      (4000000UL)

/** Default SlowClk system core frequency in Hz */
#define CY_CLK_SYSTEM_FREQ_HZ_DEFAULT       (4000000UL)

/** IMO frequency in Hz */
#define CY_CLK_IMO_FREQ_HZ                  (8000000UL)

/** HVILO frequency in Hz */
#define CY_CLK_HVILO_FREQ_HZ                (32000UL)

/** PILO frequency in Hz */
#define CY_CLK_PILO_FREQ_HZ                 (32768UL)

/** WCO frequency in Hz */
#define CY_CLK_WCO_FREQ_HZ                  (32768UL)

/** ALTLF frequency in Hz */
#define CY_CLK_ALTLF_FREQ_HZ                (32768UL)


/**
* Holds the SlowClk (Cortex-M0+) or FastClk (Cortex-M4) system core clock,
* which is the system clock frequency supplied to the SysTick timer and the
* processor core clock.
* This variable implements CMSIS Core global variable.
* Refer to the [CMSIS documentation]
* (http://www.keil.com/pack/doc/CMSIS/Core/html/group__system__init__gr.html "System and Clock Configuration")
* for more details.
* This variable can be used by debuggers to query the frequency
* of the debug timer or to configure the trace clock speed.
*
* \attention Compilers must be configured to avoid removing this variable in case
* the application program is not using it. Debugging systems require the variable
* to be physically present in memory so that it can be examined to configure the debugger. */
uint32_t SystemCoreClock = CY_CLK_SYSTEM_FREQ_HZ_DEFAULT;

/** Holds the HFClk0 clock frequency. Updated by \ref SystemCoreClockUpdate(). */
uint32_t cy_Hfclk0FreqHz  = CY_CLK_HFCLK0_FREQ_HZ_DEFAULT;

/** Holds the PeriClk clock frequency. Updated by \ref SystemCoreClockUpdate(). */
uint32_t cy_PeriClkFreqHz = CY_CLK_PERICLK_FREQ_HZ_DEFAULT;

/** Holds the Alternate high frequency clock in Hz. Updated by \ref SystemCoreClockUpdate(). */
#if (defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL)) || defined (CY_DOXYGEN)
    uint32_t cy_BleEcoClockFreqHz = CY_CLK_ALTHF_FREQ_HZ;
#endif /* (defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL)) || defined (CY_DOXYGEN) */


/*******************************************************************************
* SystemInit()
*******************************************************************************/

/* CLK_FLL_CONFIG default values */
#define CY_FB_CLK_FLL_CONFIG_VALUE      (0x01000000u)
#define CY_FB_CLK_FLL_CONFIG2_VALUE     (0x00020001u)
#define CY_FB_CLK_FLL_CONFIG3_VALUE     (0x00002800u)
#define CY_FB_CLK_FLL_CONFIG4_VALUE     (0x000000FFu)


/*******************************************************************************
* SystemCoreClockUpdate (void)
*******************************************************************************/

/* Do not use these definitions directly in your application */
#define CY_DELAY_MS_OVERFLOW_THRESHOLD  (0x8000u)
#define CY_DELAY_1K_THRESHOLD           (1000u)
#define CY_DELAY_1K_MINUS_1_THRESHOLD   (CY_DELAY_1K_THRESHOLD - 1u)
#define CY_DELAY_1M_THRESHOLD           (1000000u)
#define CY_DELAY_1M_MINUS_1_THRESHOLD   (CY_DELAY_1M_THRESHOLD - 1u)
uint32_t cy_delayFreqHz   = CY_CLK_SYSTEM_FREQ_HZ_DEFAULT;

uint32_t cy_delayFreqKhz  = (CY_CLK_SYSTEM_FREQ_HZ_DEFAULT + CY_DELAY_1K_MINUS_1_THRESHOLD) /
                            CY_DELAY_1K_THRESHOLD;

uint8_t cy_delayFreqMhz  = (uint8_t)((CY_CLK_SYSTEM_FREQ_HZ_DEFAULT + CY_DELAY_1M_MINUS_1_THRESHOLD) /
                            CY_DELAY_1M_THRESHOLD);

uint32_t cy_delay32kMs    = CY_DELAY_MS_OVERFLOW_THRESHOLD *
                            ((CY_CLK_SYSTEM_FREQ_HZ_DEFAULT + CY_DELAY_1K_MINUS_1_THRESHOLD) / CY_DELAY_1K_THRESHOLD);

#define CY_ROOT_PATH_SRC_IMO                (0UL)
#define CY_ROOT_PATH_SRC_EXT                (1UL)
#if (SRSS_ECO_PRESENT == 1U)
    #define CY_ROOT_PATH_SRC_ECO            (2UL)
#endif /* (SRSS_ECO_PRESENT == 1U) */
#if (SRSS_ALTHF_PRESENT == 1U)
    #define CY_ROOT_PATH_SRC_ALTHF          (3UL)
#endif /* (SRSS_ALTHF_PRESENT == 1U) */
#define CY_ROOT_PATH_SRC_DSI_MUX            (4UL)
#define CY_ROOT_PATH_SRC_DSI_MUX_HVILO      (16UL)
#define CY_ROOT_PATH_SRC_DSI_MUX_WCO        (17UL)
#if (SRSS_ALTLF_PRESENT == 1U)
    #define CY_ROOT_PATH_SRC_DSI_MUX_ALTLF  (18UL)
#endif /* (SRSS_ALTLF_PRESENT == 1U) */
#if (SRSS_PILO_PRESENT == 1U)
    #define CY_ROOT_PATH_SRC_DSI_MUX_PILO   (19UL)
#endif /* (SRSS_PILO_PRESENT == 1U) */


/*******************************************************************************
* Cy_SysEnableCM4(), Cy_SysRetainCM4(), and Cy_SysResetCM4()
*******************************************************************************/
#define CY_SYS_CM4_PWR_CTL_KEY_OPEN  (0x05FAUL)
#define CY_SYS_CM4_PWR_CTL_KEY_CLOSE (0xFA05UL)
#define CY_SYS_CM4_VECTOR_TABLE_VALID_ADDR  (0x000003FFUL)

void Cy_Platform_Init(void);

/*******************************************************************************
* Function Name: SystemInit
****************************************************************************//**
*
* Initializes the system:
* - Restores FLL registers to the default state.
* - Unlocks and disables WDT.
* - Calls the Cy_SystemInit() function, if compiled from PSoC Creator.
* - Calls \ref SystemCoreClockUpdate().
*
*******************************************************************************/
void SystemInit(void)
{
    Cy_PDL_Init(CY_DEVICE_CFG);

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    extern const cy_israddress __Vectors[]; /* Vector Table in flash */;
    SCB->VTOR = (uint32_t) &__Vectors;
#endif

    /* Restore FLL registers to the default state as they are not restored by the ROM code */
    uint32_t copy = SRSS->CLK_FLL_CONFIG;
    copy &= ~SRSS_CLK_FLL_CONFIG_FLL_ENABLE_Msk;
    SRSS->CLK_FLL_CONFIG = copy;

    copy = SRSS->CLK_ROOT_SELECT[0u];
    copy &= ~SRSS_CLK_ROOT_SELECT_ROOT_DIV_Msk; /* Set ROOT_DIV = 0*/
    SRSS->CLK_ROOT_SELECT[0u] = copy;

    SRSS->CLK_FLL_CONFIG  = CY_FB_CLK_FLL_CONFIG_VALUE;
    SRSS->CLK_FLL_CONFIG2 = CY_FB_CLK_FLL_CONFIG2_VALUE;
    SRSS->CLK_FLL_CONFIG3 = CY_FB_CLK_FLL_CONFIG3_VALUE;
    SRSS->CLK_FLL_CONFIG4 = CY_FB_CLK_FLL_CONFIG4_VALUE;

    /* Unlock and disable WDT */
    Cy_WDT_Unlock();
    Cy_WDT_Disable();

    Cy_SystemInit();
    SystemCoreClockUpdate();

#if defined(CY_DEVICE_PSOC6ABLE2) && !defined(CY_PSOC6ABLE2_REV_0A_SUPPORT_DISABLE)
    if (CY_SYSLIB_DEVICE_REV_0A == Cy_SysLib_GetDeviceRevision())
    {
        /* Clear data register of IPC structure #7, reserved for the Deep-Sleep operations. */
        IPC_STRUCT7->DATA = 0UL;
        /* Release IPC structure #7 to avoid deadlocks in case of SW or WDT reset during Deep-Sleep entering. */
        IPC_STRUCT7->RELEASE = 0UL;
    }
#endif /* defined(CY_DEVICE_PSOC6ABLE2) && !defined(CY_PSOC6ABLE2_REV_0A_SUPPORT_DISABLE) */
}


/*******************************************************************************
* Function Name: Cy_SystemInit
****************************************************************************//**
*
* The function is called during device startup. Once project compiled as part of
* the PSoC Creator project, the Cy_SystemInit() function is generated by the
* PSoC Creator.
*
* The function generated by PSoC Creator performs all of the necessary device
* configuration based on the design settings.  This includes settings from the
* Design Wide Resources (DWR) such as Clocks and Pins as well as any component
* configuration that is necessary.
*
*******************************************************************************/
__WEAK void Cy_SystemInit(void)
{
     /* Empty weak function. The actual implementation to be in the PSoC Creator
      * generated strong function.
     */
}


/*******************************************************************************
* Function Name: SystemCoreClockUpdate
****************************************************************************//**
*
* Gets core clock frequency and updates \ref SystemCoreClock, \ref
* cy_Hfclk0FreqHz, and \ref cy_PeriClkFreqHz.
*
* Updates global variables used by the \ref Cy_SysLib_Delay(), \ref
* Cy_SysLib_DelayUs(), and \ref Cy_SysLib_DelayCycles().
*
*******************************************************************************/
void SystemCoreClockUpdate (void)
{
    uint32_t srcFreqHz;
    uint32_t pathFreqHz;
    uint32_t slowClkDiv;
    uint32_t periClkDiv;
    uint32_t rootPath;
    uint32_t srcClk;

    /* Get root path clock for the high-frequency clock # 0 */
    rootPath = _FLD2VAL(SRSS_CLK_ROOT_SELECT_ROOT_MUX, SRSS->CLK_ROOT_SELECT[0u]);

    /* Get source of the root path clock */
    srcClk = _FLD2VAL(SRSS_CLK_PATH_SELECT_PATH_MUX, SRSS->CLK_PATH_SELECT[rootPath]);

    /* Get frequency of the source */
    switch (srcClk)
    {
    case CY_ROOT_PATH_SRC_IMO:
        srcFreqHz = CY_CLK_IMO_FREQ_HZ;
    break;

    case CY_ROOT_PATH_SRC_EXT:
        srcFreqHz = CY_CLK_EXT_FREQ_HZ;
    break;

    #if (SRSS_ECO_PRESENT == 1U)
        case CY_ROOT_PATH_SRC_ECO:
            srcFreqHz = CY_CLK_ECO_FREQ_HZ;
        break;
    #endif /* (SRSS_ECO_PRESENT == 1U) */

#if defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL) && (SRSS_ALTHF_PRESENT == 1U)
    case CY_ROOT_PATH_SRC_ALTHF:
        srcFreqHz = cy_BleEcoClockFreqHz;
    break;
#endif /* defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL) && (SRSS_ALTHF_PRESENT == 1U) */

    case CY_ROOT_PATH_SRC_DSI_MUX:
    {
        uint32_t dsi_src;
        dsi_src = _FLD2VAL(SRSS_CLK_DSI_SELECT_DSI_MUX,  SRSS->CLK_DSI_SELECT[rootPath]);
        switch (dsi_src)
        {
        case CY_ROOT_PATH_SRC_DSI_MUX_HVILO:
            srcFreqHz = CY_CLK_HVILO_FREQ_HZ;
        break;

        case CY_ROOT_PATH_SRC_DSI_MUX_WCO:
            srcFreqHz = CY_CLK_WCO_FREQ_HZ;
        break;

        #if (SRSS_ALTLF_PRESENT == 1U)
            case CY_ROOT_PATH_SRC_DSI_MUX_ALTLF:
                srcFreqHz = CY_CLK_ALTLF_FREQ_HZ;
            break;
        #endif /* (SRSS_ALTLF_PRESENT == 1U) */

        #if (SRSS_PILO_PRESENT == 1U)
            case CY_ROOT_PATH_SRC_DSI_MUX_PILO:
                srcFreqHz = CY_CLK_PILO_FREQ_HZ;
            break;
        #endif /* (SRSS_PILO_PRESENT == 1U) */

        default:
            srcFreqHz = CY_CLK_HVILO_FREQ_HZ;
        break;
        }
    }
    break;

    default:
        srcFreqHz = CY_CLK_EXT_FREQ_HZ;
    break;
    }

    if (rootPath == 0UL)
    {
        /* FLL */
        bool fllLocked       = ( 0UL != _FLD2VAL(SRSS_CLK_FLL_STATUS_LOCKED, SRSS->CLK_FLL_STATUS));
        bool fllOutputOutput = ( 3UL == _FLD2VAL(SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, SRSS->CLK_FLL_CONFIG3));
        bool fllOutputAuto   = ((0UL == _FLD2VAL(SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, SRSS->CLK_FLL_CONFIG3)) ||
                                (1UL == _FLD2VAL(SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, SRSS->CLK_FLL_CONFIG3)));
        if ((fllOutputAuto && fllLocked) || fllOutputOutput)
        {
            uint32_t fllMult;
            uint32_t refDiv;
            uint32_t outputDiv;

            fllMult = _FLD2VAL(SRSS_CLK_FLL_CONFIG_FLL_MULT, SRSS->CLK_FLL_CONFIG);
            refDiv  = _FLD2VAL(SRSS_CLK_FLL_CONFIG2_FLL_REF_DIV, SRSS->CLK_FLL_CONFIG2);
            outputDiv = _FLD2VAL(SRSS_CLK_FLL_CONFIG_FLL_OUTPUT_DIV, SRSS->CLK_FLL_CONFIG) + 1UL;

            pathFreqHz = ((srcFreqHz / refDiv) * fllMult) / outputDiv;
        }
        else
        {
            pathFreqHz = srcFreqHz;
        }
    }
    else if ((rootPath == 1UL) || (rootPath == 2UL))
    {
        /* PLL */
        bool pllLocked       = ( 0UL != _FLD2VAL(SRSS_CLK_PLL_STATUS_LOCKED,     SRSS->CLK_PLL_STATUS[rootPath - 1UL]));
        bool pllOutputOutput = ( 3UL == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS->CLK_PLL_CONFIG[rootPath - 1UL]));
        bool pllOutputAuto   = ((0UL == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS->CLK_PLL_CONFIG[rootPath - 1UL])) ||
                                (1UL == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS->CLK_PLL_CONFIG[rootPath - 1UL])));
        if ((pllOutputAuto && pllLocked) || pllOutputOutput)
        {
            uint32_t feedbackDiv;
            uint32_t referenceDiv;
            uint32_t outputDiv;

            feedbackDiv  = _FLD2VAL(SRSS_CLK_PLL_CONFIG_FEEDBACK_DIV,  SRSS->CLK_PLL_CONFIG[rootPath - 1UL]);
            referenceDiv = _FLD2VAL(SRSS_CLK_PLL_CONFIG_REFERENCE_DIV, SRSS->CLK_PLL_CONFIG[rootPath - 1UL]);
            outputDiv    = _FLD2VAL(SRSS_CLK_PLL_CONFIG_OUTPUT_DIV,    SRSS->CLK_PLL_CONFIG[rootPath - 1UL]);

            pathFreqHz = ((srcFreqHz * feedbackDiv) / referenceDiv) / outputDiv;

        }
        else
        {
            pathFreqHz = srcFreqHz;
        }
    }
    else
    {
        /* Direct */
        pathFreqHz = srcFreqHz;
    }

    /* Get frequency after hf_clk pre-divider */
    pathFreqHz = pathFreqHz >> _FLD2VAL(SRSS_CLK_ROOT_SELECT_ROOT_DIV, SRSS->CLK_ROOT_SELECT[0u]);
    cy_Hfclk0FreqHz = pathFreqHz;

    /* Slow Clock Divider */
    slowClkDiv = 1u + _FLD2VAL(CPUSS_CM0_CLOCK_CTL_SLOW_INT_DIV, CPUSS->CM0_CLOCK_CTL);

    /* Peripheral Clock Divider */
    periClkDiv = 1u + _FLD2VAL(CPUSS_CM0_CLOCK_CTL_PERI_INT_DIV, CPUSS->CM0_CLOCK_CTL);

    pathFreqHz = pathFreqHz / periClkDiv;
    cy_PeriClkFreqHz = pathFreqHz;
    pathFreqHz = pathFreqHz / slowClkDiv;
    SystemCoreClock = pathFreqHz;

    /* Sets clock frequency for Delay API */
    cy_delayFreqHz = SystemCoreClock;
    cy_delayFreqMhz = (uint8_t)((cy_delayFreqHz + CY_DELAY_1M_MINUS_1_THRESHOLD) / CY_DELAY_1M_THRESHOLD);
    cy_delayFreqKhz = (cy_delayFreqHz + CY_DELAY_1K_MINUS_1_THRESHOLD) / CY_DELAY_1K_THRESHOLD;
    cy_delay32kMs   = CY_DELAY_MS_OVERFLOW_THRESHOLD * cy_delayFreqKhz;
}


#if (CY_SYSTEM_CPU_CM0P == 1UL) || defined(CY_DOXYGEN)
/*******************************************************************************
* Function Name: Cy_SysGetCM4Status
****************************************************************************//**
*
* Returns the Cortex-M4 core power mode.
*
* \return \ref group_system_config_cm4_status_macro
*
*******************************************************************************/
uint32_t Cy_SysGetCM4Status(void)
{
    uint32_t regValue;

    /* Get current power mode */
    regValue = CPUSS->CM4_PWR_CTL & CPUSS_CM4_PWR_CTL_PWR_MODE_Msk;

    return (regValue);
}


/*******************************************************************************
* Function Name: Cy_SysEnableCM4
****************************************************************************//**
*
* Sets vector table base address and enables the Cortex-M4 core.
*
* \note If the CPU is already enabled, it is reset and then enabled.
*
* \param vectorTableOffset The offset of the vector table base address from
* memory address 0x00000000. The offset should be multiple to 1024 bytes.
*
*******************************************************************************/
void Cy_SysEnableCM4(uint32_t vectorTableOffset)
{
    uint32_t regValue;
    uint32_t interruptState;
    uint32_t cpuState;

    CY_ASSERT_L2((vectorTableOffset & CY_SYS_CM4_VECTOR_TABLE_VALID_ADDR) == 0UL);

    interruptState = Cy_SysLib_EnterCriticalSection();

    cpuState = Cy_SysGetCM4Status();
    if (CY_SYS_CM4_STATUS_ENABLED == cpuState)
    {
        Cy_SysResetCM4();
    }

    CPUSS->CM4_VECTOR_TABLE_BASE = vectorTableOffset;

    regValue = CPUSS->CM4_PWR_CTL & ~(CPUSS_CM4_PWR_CTL_VECTKEYSTAT_Msk | CPUSS_CM4_PWR_CTL_PWR_MODE_Msk);
    regValue |= _VAL2FLD(CPUSS_CM4_PWR_CTL_VECTKEYSTAT, CY_SYS_CM4_PWR_CTL_KEY_OPEN);
    regValue |= CY_SYS_CM4_STATUS_ENABLED;
    CPUSS->CM4_PWR_CTL = regValue;

    while((CPUSS->CM4_STATUS & CPUSS_CM4_STATUS_PWR_DONE_Msk) == 0UL)
    {
        /* Wait for the power mode to take effect */
    }

    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_SysDisableCM4
****************************************************************************//**
*
* Disables the Cortex-M4 core and waits for the mode to take the effect.
*
* \warning Do not call the function while the Cortex-M4 is executing because
* such a call may corrupt/abort a pending bus-transaction by the CPU and cause
* unexpected behavior in the system including a deadlock. Call the function
* while the Cortex-M4 core is in the Sleep or Deep Sleep low-power mode. Use
* the \ref group_syspm Power Management (syspm) API to put the CPU into the
* low-power modes. Use the \ref Cy_SysPm_ReadStatus() to get a status of the
* CPU.
*
*******************************************************************************/
void Cy_SysDisableCM4(void)
{
    uint32_t interruptState;
    uint32_t regValue;

    interruptState = Cy_SysLib_EnterCriticalSection();

    regValue = CPUSS->CM4_PWR_CTL & ~(CPUSS_CM4_PWR_CTL_VECTKEYSTAT_Msk | CPUSS_CM4_PWR_CTL_PWR_MODE_Msk);
    regValue |= _VAL2FLD(CPUSS_CM4_PWR_CTL_VECTKEYSTAT, CY_SYS_CM4_PWR_CTL_KEY_OPEN);
    regValue |= CY_SYS_CM4_STATUS_DISABLED;
    CPUSS->CM4_PWR_CTL = regValue;

    while((CPUSS->CM4_STATUS & CPUSS_CM4_STATUS_PWR_DONE_Msk) == 0UL)
    {
        /* Wait for the power mode to take effect */
    }

    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_SysRetainCM4
****************************************************************************//**
*
* Retains the Cortex-M4 core and exists without waiting for the mode to take
* effect.
*
* \note The retained mode can be entered only from the enabled mode.
*
* \warning Do not call the function while the Cortex-M4 is executing because
* such a call may corrupt/abort a pending bus-transaction by the CPU and cause
* unexpected behavior in the system including a deadlock. Call the function
* while the Cortex-M4 core is in the Sleep or Deep Sleep low-power mode. Use
* the \ref group_syspm Power Management (syspm) API to put the CPU into the
* low-power modes. Use the \ref Cy_SysPm_ReadStatus() to get a status of the CPU.
*
*******************************************************************************/
void Cy_SysRetainCM4(void)
{
    uint32_t interruptState;
    uint32_t regValue;

    interruptState = Cy_SysLib_EnterCriticalSection();

    regValue = CPUSS->CM4_PWR_CTL & ~(CPUSS_CM4_PWR_CTL_VECTKEYSTAT_Msk | CPUSS_CM4_PWR_CTL_PWR_MODE_Msk);
    regValue |= _VAL2FLD(CPUSS_CM4_PWR_CTL_VECTKEYSTAT, CY_SYS_CM4_PWR_CTL_KEY_OPEN);
    regValue |= CY_SYS_CM4_STATUS_RETAINED;
    CPUSS->CM4_PWR_CTL = regValue;

    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_SysResetCM4
****************************************************************************//**
*
* Resets the Cortex-M4 core and waits for the mode to take the effect.
*
* \note The reset mode can not be entered from the retained mode.
*
* \warning Do not call the function while the Cortex-M4 is executing because
* such a call may corrupt/abort a pending bus-transaction by the CPU and cause
* unexpected behavior in the system including a deadlock. Call the function
* while the Cortex-M4 core is in the Sleep or Deep Sleep low-power mode. Use
* the \ref group_syspm Power Management (syspm) API to put the CPU into the
* low-power modes. Use the \ref Cy_SysPm_ReadStatus() to get a status of the CPU.
*
*******************************************************************************/
void Cy_SysResetCM4(void)
{
    uint32_t interruptState;
    uint32_t regValue;

    interruptState = Cy_SysLib_EnterCriticalSection();

    regValue = CPUSS->CM4_PWR_CTL & ~(CPUSS_CM4_PWR_CTL_VECTKEYSTAT_Msk | CPUSS_CM4_PWR_CTL_PWR_MODE_Msk);
    regValue |= _VAL2FLD(CPUSS_CM4_PWR_CTL_VECTKEYSTAT, CY_SYS_CM4_PWR_CTL_KEY_OPEN);
    regValue |= CY_SYS_CM4_STATUS_RESET;
    CPUSS->CM4_PWR_CTL = regValue;

    while((CPUSS->CM4_STATUS & CPUSS_CM4_STATUS_PWR_DONE_Msk) == 0UL)
    {
        /* Wait for the power mode to take effect */
    }

    Cy_SysLib_ExitCriticalSection(interruptState);
}
#endif /* #if (CY_SYSTEM_CPU_CM0P == 1UL) || defined(CY_DOXYGEN) */


#if !defined(CY_IPC_DEFAULT_CFG_DISABLE) && !defined(CY_FLASH_RWW_DRV_SUPPORT_DISABLED)
/*******************************************************************************
* Function Name: Cy_SysIpcPipeIsrCm0
****************************************************************************//**
*
* This is the interrupt service routine for the system pipe.
*
*******************************************************************************/
void Cy_SysIpcPipeIsrCm0(void)
{
    Cy_IPC_Pipe_ExecuteCallback(CY_IPC_EP_CYPIPE_CM0_ADDR);
}
#endif


/*******************************************************************************
* Function Name: Cy_MemorySymbols
****************************************************************************//**
*
* The intention of the function is to declare boundaries of the memories for the
* MDK compilers. For the rest of the supported compilers, this is done using
* linker configuration files. The following symbols used by the cymcuelftool.
*
*******************************************************************************/
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION < 6010050)
__asm void Cy_MemorySymbols(void)
{
    /* Flash */
    EXPORT __cy_memory_0_start
    EXPORT __cy_memory_0_length
    EXPORT __cy_memory_0_row_size

    /* Working Flash */
    EXPORT __cy_memory_1_start
    EXPORT __cy_memory_1_length
    EXPORT __cy_memory_1_row_size

    /* Supervisory Flash */
    EXPORT __cy_memory_2_start
    EXPORT __cy_memory_2_length
    EXPORT __cy_memory_2_row_size

    /* XIP */
    EXPORT __cy_memory_3_start
    EXPORT __cy_memory_3_length
    EXPORT __cy_memory_3_row_size

    /* eFuse */
    EXPORT __cy_memory_4_start
    EXPORT __cy_memory_4_length
    EXPORT __cy_memory_4_row_size

    /* Flash */
__cy_memory_0_start     EQU __cpp(CY_FLASH_BASE)
__cy_memory_0_length    EQU __cpp(CY_FLASH_SIZE)
__cy_memory_0_row_size  EQU 0x200

    /* Flash region for EEPROM emulation */
__cy_memory_1_start     EQU __cpp(CY_EM_EEPROM_BASE)
__cy_memory_1_length    EQU __cpp(CY_EM_EEPROM_SIZE)
__cy_memory_1_row_size  EQU 0x200

    /* Supervisory Flash */
__cy_memory_2_start     EQU __cpp(CY_SFLASH_BASE)
__cy_memory_2_length    EQU __cpp(CY_SFLASH_SIZE)
__cy_memory_2_row_size  EQU 0x200

    /* XIP */
__cy_memory_3_start     EQU __cpp(CY_XIP_BASE)
__cy_memory_3_length    EQU __cpp(CY_XIP_SIZE)
__cy_memory_3_row_size  EQU 0x200

    /* eFuse */
__cy_memory_4_start     EQU __cpp(0x90700000)
__cy_memory_4_length    EQU __cpp(0x100000)
__cy_memory_4_row_size  EQU __cpp(1)
}
#endif /* defined (__ARMCC_VERSION) && (__ARMCC_VERSION < 6010050) */


/*******************************************************************************
* Function Name: Cy_Platform_Init
****************************************************************************//**
*
* CM0 custom HW initialization
*
*******************************************************************************/
void Cy_Platform_Init(void)
{
#if !defined(CY_IPC_DEFAULT_CFG_DISABLE)
    /* Initialize semaphores for the system operations.*/
    (void) Cy_IPC_Sema_Init(CY_IPC_CHAN_SEMA, 0ul, NULL);

#if !defined(CY_FLASH_RWW_DRV_SUPPORT_DISABLED)
    /********************************************************************************
    *
    * Initializes the system pipes. The system pipes are used by BLE and Flash.
    *
    * If the default startup file is not used, or SystemInit() is not called in your
    * project, call the following three functions prior to executing any flash or
    * EmEEPROM write or erase operation:
    *  -# Cy_IPC_Sema_Init()
    *  -# Cy_IPC_Pipe_Config()
    *  -# Cy_IPC_Pipe_Init()
    *  -# Cy_Flash_Init()
    *
    *******************************************************************************/

    /* Create an array of endpoint structures */
    static cy_stc_ipc_pipe_ep_t systemIpcPipeEpArray[CY_IPC_MAX_ENDPOINTS];

    Cy_IPC_Pipe_Config(systemIpcPipeEpArray);

    static cy_ipc_pipe_callback_ptr_t systemIpcPipeSysCbArray[CY_SYS_CYPIPE_CLIENT_CNT];

    static const cy_stc_ipc_pipe_config_t systemIpcPipeConfigCm0 =
    {
    /* .ep0ConfigData */
        {
            /* .ipcNotifierNumber    */  CY_IPC_INTR_CYPIPE_EP0,
            /* .ipcNotifierPriority  */  CY_SYS_INTR_CYPIPE_PRIOR_EP0,
            /* .ipcNotifierMuxNumber */  CY_SYS_INTR_CYPIPE_MUX_EP0,
            /* .epAddress            */  CY_IPC_EP_CYPIPE_CM0_ADDR,
            /* .epConfig             */  CY_SYS_CYPIPE_CONFIG_EP0
        },
    /* .ep1ConfigData */
        {
            /* .ipcNotifierNumber    */  CY_IPC_INTR_CYPIPE_EP1,
            /* .ipcNotifierPriority  */  CY_SYS_INTR_CYPIPE_PRIOR_EP1,
            /* .ipcNotifierMuxNumber */  0u,
            /* .epAddress            */  CY_IPC_EP_CYPIPE_CM4_ADDR,
            /* .epConfig             */  CY_SYS_CYPIPE_CONFIG_EP1
        },
    /* .endpointClientsCount     */  CY_SYS_CYPIPE_CLIENT_CNT,
    /* .endpointsCallbacksArray  */  systemIpcPipeSysCbArray,
    /* .userPipeIsrHandler       */  &Cy_SysIpcPipeIsrCm0
    };

    if (cy_device->flashPipeRequired != 0u)
    {
        Cy_IPC_Pipe_Init(&systemIpcPipeConfigCm0);
    }

#if defined(CY_DEVICE_PSOC6ABLE2)
    Cy_Flash_Init();
#endif /* defined(CY_DEVICE_PSOC6ABLE2) */

#endif /* !defined(CY_FLASH_RWW_DRV_SUPPORT_DISABLED) */

#endif /* !defined(CY_IPC_DEFAULT_CFG_DISABLE) */


    return;
}

#ifdef BL2
/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

uint32_t bl2_platform_init(void)
{
    Cy_PDL_Init(CY_DEVICE_CFG);

    init_cycfg_all();
    Cy_Platform_Init();

    /* make sure CM4 is disabled */
    if (CY_SYS_CM4_STATUS_ENABLED == Cy_SysGetCM4Status()) {
        Cy_SysDisableCM4();
    }

    return FLASH_DEV_NAME.Initialize(NULL);
}
#endif

/* [] END OF FILE */
