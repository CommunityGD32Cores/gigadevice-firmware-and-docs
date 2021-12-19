/**************************************************************************//**
 * @file     system_ARMCM33.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM33 Device Series
 * @version  V5.00
 * @date     02. November 2016
 ******************************************************************************/
/*
 * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
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

#include "stdint.h"
#include "platform_def.h"
#include "gd32w51x.h"

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
#if defined (__CC_ARM) || defined ( __ARMCC_VERSION )
  extern uint32_t __Vectors;
#endif
#if defined (__ICCARM__)
  extern uint32_t __vector_table;
#endif
#endif

/*!
    \brief      configure the system clock to HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* power up the HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    RCU_CTL |= RCU_CTL_HXTALENPLL;

    RCU_CTL |= RCU_CTL_HXTALREADY;
    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(1){
        }
    }
    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL0 |= PMU_CTL0_LDOVS;
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    // RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* select HXTAL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_HXTAL;

    /* wait until HXTAL is selected as system clock */
    while(0 == (RCU_CFG0 & RCU_SCSS_HXTAL)){
    }
}

/*!
    \brief      configure the system clock to 180M by PLL which selects HXTAL(40M) as its clock source
    \param[in]  none
    \param[out] none
    \retval     none
*/
void system_clock_180m_40m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* power up HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    RCU_CTL |= RCU_CTL_HXTALENPLL;
    {
       __IO int i;
       for (i = 0; i < 0x20; i++);
    }

    RCU_CTL |= RCU_CTL_HXTALREADY;
    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(1){
        }
    }

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL0 |= PMU_CTL0_LDOVS;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* Configure the main PLL, PSC = 40, PLL_N = 360, PLL_P = 2 */
    RCU_PLL = (40U | (360U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_HXTAL) );

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

    /* select PLLP as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)){
    }
}

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
/*!
    \brief      setup the microcontroller system, initialize the system
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SystemInit (void)
	{
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
#if defined (__CC_ARM) || defined ( __ARMCC_VERSION )
    SCB->VTOR = (uint32_t) &__Vectors;
#endif
#if defined (__ICCARM__)
    SCB->VTOR = (uint32_t) &__vector_table;
#endif
#endif
#ifdef UNALIGNED_SUPPORT_DISABLE
	SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
	/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) && (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	/* set CP10 and CP11 non-secure access */
	SCB->NSACR |= BIT(SCB_NSACR_CP11_Pos) | BIT(SCB_NSACR_CP10_Pos);
#endif

	/* Reset the RCC clock configuration to the default reset state ------------*/
	RCU_CTL |= RCU_CTL_HXTALBPS;
	{
		__IO uint32_t i;
		for (i=0; i<20; i++);
	}

	/* Reset CFG0 register */
	RCU_CFG0 = 0x00000000U;

	/* Reset HSEON, CSSON and PLLON bits */
	RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);

	/* Reset PLLCFG register */
	RCU_PLLCFG = RCU_PLLCFG_PLLDIGOSEL;

#ifdef CONFIG_PLATFORM_ASIC
	system_clock_180m_40m_hxtal();
#else
	system_clock_hxtal();
#endif

	/* Enable icache */
	icache_enable();
}
