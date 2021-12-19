/*!
    \file    partition_gd32w51x.h
    \brief   partition_gd32w51x head file

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

#ifndef PARTITION_GD32W51X_H
#define PARTITION_GD32W51X_H

/*
// <<< Use Configuration Wizard in Context Menu >>>
*/

/*
// <e>Configure Security Attribution Unit (SAU)
*/
#define SAU_INIT_CTRL          1

/*
//   <q> Enable SAU
//   <i> Enable security attribution unit
*/
#define SAU_INIT_CTRL_ENABLE   1

/*
//   <o> When SAU is disabled, the attributes of all memory
//     <0=>  Secure
//     <1=>  Non-Secure
//   <i> Set the value of ALLNS bit in SAU->CTRL register  
//   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
*/
#define SAU_INIT_CTRL_ALLNS    1

/*
// </e>
*/

/*
// <h>Configure Security Attribution Unit (SAU) Address Regions Attributes
// <i>SAU address area attributes will choose one from the following two options:
// <i> + Secure and Non-Secure Callable
// <i> + Non-Secure
// <i>Note: All memory regions are secure when SAU is not configured
*/
#define SAU_REGIONS_MAX   8                 /* Max. number of SAU regions */

/*
//   <e>Configure SAU Region 0
//   <i> Configure SAU Region 0 memory attributes
*/
#define SAU_INIT_REGION0    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START0     0x0C03E000      /* start address of SAU region 0 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END0       0x0C03FFFF      /* end address of SAU region 0 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC0       1
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 1
//   <i> Configure SAU Region 1 memory attributes
*/
#define SAU_INIT_REGION1    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START1     0x08040000      /* start address of SAU region 1 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END1       0x08200000      /* end address of SAU region 1 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC1       0
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 2
//   <i> Configure SAU Region 2 memory attributes
*/
#define SAU_INIT_REGION2    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START2     0x20010000      /* start address of SAU region 2 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END2       0x2001FFFF      /* end address of SAU region 2 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC2       0
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 3
//   <i> Configure SAU Region 3 memory attributes
*/
#define SAU_INIT_REGION3    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START3     0x40000000      /* start address of SAU region 3 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END3       0x4FFFFFFF      /* end address of SAU region 3 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC3       0
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 4
//   <i> Configure SAU Region 4 memory attributes
*/
#define SAU_INIT_REGION4    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START4     0x60000000      /* start address of SAU region 4 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END4       0x9FFFFFFF      /* end address of SAU region 4 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC4       0
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 5
//   <i> Configure SAU Region 5 memory attributes
*/
#define SAU_INIT_REGION5    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START5     0x0BF90000      /* start address of SAU region 5 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END5       0x0BFA8FFF      /* end address of SAU region 5 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC5       0
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 6
//   <i> Configure SAU Region 6 memory attributes
*/
#define SAU_INIT_REGION6    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START6     0x40021000      /* start address of SAU region 6 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END6       0x400213FF      /* end address of SAU region 6 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC6       1
/*
//   </e>
*/

/*
//   <e>Configure SAU Region 7
//   <i> Configure SAU Region 7 memory attributes
*/
#define SAU_INIT_REGION7    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START7     0x40008000      /* start address of SAU region 7 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END7       0x000803FF      /* end address of SAU region 7 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC7       1
/*
//   </e>
*/

/*
// </h>
*/

/*
// <e>Configure behaviour of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT          0

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Set the value of DEEPSLEEPS bit in SCB->CSR register 
*/
#define SCB_CSR_DEEPSLEEPS_VAL      0

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Set the value of SYSRESETREQS bit in SCB->AIRCR register
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x04-0x07
//   <i> Set the value of PRIS bit in SCB->AIRCR register
*/
#define SCB_AIRCR_PRIS_VAL          0

/*
//   <o>BusFault, HardFault, and NMI target
//     <0=> Secure state
//     <1=> Non-Secure state
//   <i> Set the value of BFHFNMINS bit in SCB->AIRCR register
*/
#define SCB_AIRCR_BFHFNMINS_VAL     0

/*
// </e>
*/

/*
// <e>Configure behaviour of Floating Point Unit
*/
#define TZ_FPU_NS_USAGE             0

/*
// <o>Floating Point Unit usage
//     <0=> Secure state only
//     <3=> Secure and Non-Secure state
//   <i> Set the value of CP10 and CP11 bits in SCB->NSACR register
*/
#define SCB_NSACR_CP10_11_VAL       3

/*
// <o>Treat floating-point registers as Secure
//     <0=> Disabled
//     <1=> Enabled
//   <i> Set the value of TS bit in FPU->FPCCR register
*/
#define FPU_FPCCR_TS_VAL            0

/*
// <o>Clear on return (CLRONRET) accessibility
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Set the value of CLRONRETS bits in FPU->FPCCR register bit 
*/
#define FPU_FPCCR_CLRONRETS_VAL     0

/*
// <o>Clear floating-point caller saved registers on exception return
//     <0=> Disabled
//     <1=> Enabled
//   <i> Set the value of CLRONRET bit in FPU->FPCCR register
*/
#define FPU_FPCCR_CLRONRET_VAL      1

/*
// </e>
*/

/*
// <h>Configure Interrupt Target to Secure or Non-secure Vector
*/

/*
//   <e>Initialize ITNS 0 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS0    1

/*
// Interrupts 0..31
//   <o.0>  WWDG_IRQn           <0=> Secure <1=> Non-Secure
//   <o.1>  LVD_IRQn            <0=> Secure <1=> Non-Secure
//   <o.2>  TAMPER_STAMP_IRQn   <0=> Secure <1=> Non-Secure
//   <o.3>  RTC_WKUP_IRQn       <0=> Secure <1=> Non-Secure
//   <o.4>  FMC_IRQn            <0=> Secure <1=> Non-Secure
//   <o.5>  RCU_IRQn            <0=> Secure <1=> Non-Secure
//   <o.6>  EXTI0_IRQn          <0=> Secure <1=> Non-Secure
//   <o.7>  EXTI1_IRQn          <0=> Secure <1=> Non-Secure
//   <o.8>  EXTI2_IRQn          <0=> Secure <1=> Non-Secure
//   <o.9>  EXTI3_IRQn          <0=> Secure <1=> Non-Secure
//   <o.10> EXTI4_IRQn          <0=> Secure <1=> Non-Secure
//   <o.11> DMA0_Channel0_IRQn  <0=> Secure <1=> Non-Secure
//   <o.12> DMA0_Channel1_IRQn  <0=> Secure <1=> Non-Secure
//   <o.13> DMA0_Channel2_IRQn  <0=> Secure <1=> Non-Secure
//   <o.14> DMA0_Channel3_IRQn  <0=> Secure <1=> Non-Secure
//   <o.15> DMA0_Channel4_IRQn  <0=> Secure <1=> Non-Secure
//   <o.16> DMA0_Channel5_IRQn  <0=> Secure <1=> Non-Secure
//   <o.17> DMA0_Channel6_IRQn  <0=> Secure <1=> Non-Secure
//   <o.18> DMA0_Channel7_IRQn  <0=> Secure <1=> Non-Secure
//   <o.19> ADC_IRQn            <0=> Secure <1=> Non-Secure
//   <o.20> TAMP_STAMP_S_IRQn   <0=> Secure <1=> Non-Secure
//   <o.21> RTC_WKUP_S_IRQn     <0=> Secure <1=> Non-Secure
//   <o.22> RTC_Alarm_S_IRQn    <0=> Secure <1=> Non-Secure
//   <o.23> EXTI5_9_IRQn        <0=> Secure <1=> Non-Secure
//   <o.24> TIMER0_BRK_IRQn     <0=> Secure <1=> Non-Secure
//   <o.25> TIMER0_UP_IRQn      <0=> Secure <1=> Non-Secure
//   <o.26> TIMER0_CMT_IRQn     <0=> Secure <1=> Non-Secure
//   <o.27> TIMER0_Channel_IRQn <0=> Secure <1=> Non-Secure
//   <o.28> TIMER1_IRQn         <0=> Secure <1=> Non-Secure
//   <o.29> TIMER2_IRQn         <0=> Secure <1=> Non-Secure
//   <o.30> TIMER3_IRQn         <0=> Secure <1=> Non-Secure
//   <o.31> I2C0_EV_IRQn        <0=> Secure <1=> Non-Secure
*/
#define NVIC_INIT_ITNS0_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 1 (Interrupts 32..63)
*/
#define NVIC_INIT_ITNS1    1

/*
/ Interrupts 32..63
//   <o.0>  I2C0_ER_IRQn        <0=> Secure <1=> Non-Secure
//   <o.1>  I2C1_EV_IRQn        <0=> Secure <1=> Non-Secure
//   <o.2>  I2C1_ER_IRQn        <0=> Secure <1=> Non-Secure
//   <o.3>  SPI0_IRQn           <0=> Secure <1=> Non-Secure
//   <o.4>  SPI1_IRQn           <0=> Secure <1=> Non-Secure
//   <o.5>  USART0_IRQn         <0=> Secure <1=> Non-Secure
//   <o.6>  USART1_IRQn         <0=> Secure <1=> Non-Secure
//   <o.7>  USART2_IRQn         <0=> Secure <1=> Non-Secure
//   <o.8>  EXTI10_15_IRQn      <0=> Secure <1=> Non-Secure
//   <o.9>  RTC_Alarm_IRQn      <0=> Secure <1=> Non-Secure
//   <o.10> PVM_IRQn            <0=> Secure <1=> Non-Secure
//   <o.12> TIMER15_IRQn        <0=> Secure <1=> Non-Secure
//   <o.13> TIMER16_IRQn        <0=> Secure <1=> Non-Secure
//   <o.17> SDIO_IRQn           <0=> Secure <1=> Non-Secure
//   <o.18> TIMER4_IRQn         <0=> Secure <1=> Non-Secure
//   <o.19> I2C0_WKUP_IRQn      <0=> Secure <1=> Non-Secure
//   <o.20> USART0_WKUP_IRQn    <0=> Secure <1=> Non-Secure
//   <o.21> USART2_WKUP_IRQn    <0=> Secure <1=> Non-Secure
//   <o.22> TIMER5_IRQn         <0=> Secure <1=> Non-Secure
//   <o.24> DMA1_Channel0_IRQn  <0=> Secure <1=> Non-Secure
//   <o.25> DMA1_Channel1_IRQn  <0=> Secure <1=> Non-Secure
//   <o.26> DMA1_Channel2_IRQn  <0=> Secure <1=> Non-Secure
//   <o.27> DMA1_Channel3_IRQn  <0=> Secure <1=> Non-Secure
//   <o.28> DMA1_Channel4_IRQn  <0=> Secure <1=> Non-Secure
//   <o.29> DMA1_Channel5_IRQn  <0=> Secure <1=> Non-Secure
//   <o.30> DMA1_Channel6_IRQn  <0=> Secure <1=> Non-Secure
//   <o.31> DMA1_Channel7_IRQn  <0=> Secure <1=> Non-Secure
*/
#define NVIC_INIT_ITNS1_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 2 (Interrupts 64..95)
*/
#define NVIC_INIT_ITNS2    1

/*
// Interrupts 64..95
//   <o.2>  WIFI11N_WKUP_IRQn   <0=> Secure <1=> Non-Secure
//   <o.3>  USBFS_IRQn          <0=> Secure <1=> Non-Secure
//   <o.12> USBF_WKUP_IRQn      <0=> Secure <1=> Non-Secure
//   <o.14> DCI_IRQn            <0=> Secure <1=> Non-Secure
//   <o.15> CAU_IRQn            <0=> Secure <1=> Non-Secure
//   <o.16> HAU_TRNG_IRQn       <0=> Secure <1=> Non-Secure
//   <o.17> FPU_IRQn            <0=> Secure <1=> Non-Secure
//   <o.25> HPDF_INT0_IRQn      <0=> Secure <1=> Non-Secure
//   <o.26> HPDF_INT1_IRQn      <0=> Secure <1=> Non-Secure
//   <o.27> WIFI11N_INT0_IRQn   <0=> Secure <1=> Non-Secure
//   <o.28> WIFI11N_INT1_IRQn   <0=> Secure <1=> Non-Secure
//   <o.29> WIFI11N_INT2_IRQn   <0=> Secure <1=> Non-Secure
//   <o.30> EFUSE_IRQn          <0=> Secure <1=> Non-Secure
//   <o.31> QSPI_IRQn           <0=> Secure <1=> Non-Secure
*/
#define NVIC_INIT_ITNS2_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 3 (Interrupts 96..108)
*/
#define NVIC_INIT_ITNS3    1

/*
// Interrupts 96..108
//   <o.0>  PKCAU_IRQn          <0=> Secure <1=> Non-Secure
//   <o.1>  TSI_IRQn            <0=> Secure <1=> Non-Secure
//   <o.2>  ICHACHE_IRQn        <0=> Secure <1=> Non-Secure
//   <o.3>  TZIAC_S_IRQn        <0=> Secure <1=> Non-Secure
//   <o.4>  FMC_S_IRQn          <0=> Secure <1=> Non-Secure
//   <o.5>  QSPI_S_IRQn         <0=> Secure <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS3_VAL      0x00000000

/*
//   </e>
*/

/*
// </h>
*/



/*
    max 8 SAU regions.
    SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U

/*!
    \brief      configure SAU region attributes
    \param[in]  none
    \param[out] none
    \retval     none
*/
__STATIC_INLINE void sau_region_config (void)
{

#if defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)

  #if defined (SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
    SAU_INIT_REGION(0);
  #endif

  #if defined (SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
    SAU_INIT_REGION(1);
  #endif

  #if defined (SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
    SAU_INIT_REGION(2);
  #endif

  #if defined (SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
    SAU_INIT_REGION(3);
  #endif

  #if defined (SAU_INIT_REGION4) && (SAU_INIT_REGION4 == 1U)
    SAU_INIT_REGION(4);
  #endif

  #if defined (SAU_INIT_REGION5) && (SAU_INIT_REGION5 == 1U)
    SAU_INIT_REGION(5);
  #endif

  #if defined (SAU_INIT_REGION6) && (SAU_INIT_REGION6 == 1U)
    SAU_INIT_REGION(6);
  #endif

  #if defined (SAU_INIT_REGION7) && (SAU_INIT_REGION7 == 1U)
    SAU_INIT_REGION(7);
  #endif

/* repeat this for all possible SAU regions */

#endif /* defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U) */


  #if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
    SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
                ((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;
  #endif

  #if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
    SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk    )) |
                   ((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

    SCB->AIRCR = (SCB->AIRCR & ~(SCB_AIRCR_VECTKEY_Msk   | SCB_AIRCR_SYSRESETREQS_Msk |
                                 SCB_AIRCR_BFHFNMINS_Msk | SCB_AIRCR_PRIS_Msk)        )                     |
                   ((0x05FAU                    << SCB_AIRCR_VECTKEY_Pos)      & SCB_AIRCR_VECTKEY_Msk)      |
                   ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
                   ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk)         |
                   ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk);
  #endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

  #if defined (__FPU_USED) && (__FPU_USED == 1U) && \
      defined (TZ_FPU_NS_USAGE) && (TZ_FPU_NS_USAGE == 1U)

    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP10_Msk)) |
                   ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
                   ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos       ) & FPU_FPCCR_TS_Msk       ) |
                   ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
                   ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos ) & FPU_FPCCR_CLRONRET_Msk );
  #endif

  #if defined (NVIC_INIT_ITNS0) && (NVIC_INIT_ITNS0 == 1U)
    NVIC->ITNS[0] = NVIC_INIT_ITNS0_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS1) && (NVIC_INIT_ITNS1 == 1U)
    NVIC->ITNS[1] = NVIC_INIT_ITNS1_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS2) && (NVIC_INIT_ITNS2 == 1U)
    NVIC->ITNS[2] = NVIC_INIT_ITNS2_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS3) && (NVIC_INIT_ITNS3 == 1U)
    NVIC->ITNS[3] = NVIC_INIT_ITNS3_VAL;
  #endif

}

#endif  /* PARTITION_GD32W51X_H */
