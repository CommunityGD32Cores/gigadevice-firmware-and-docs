/*!
;    \file    mbl_startup_gdm32.s
;    \brief   start up file

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
  .syntax unified
  .cpu cortex-m33
  .fpu softvfp
  .thumb

.global  Default_Handler

/* necessary symbols defined in linker script to initialize data */
.word  _sidata
.word  _sdata
.word  _edata
.word  _sbss
.word  _ebss

.equ  BootRAM,        0xF1E0F85F

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function

/* reset Handler */
Reset_Handler:
  movs r1, #0
  b CodeInit

CopyData:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

DataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyData
  ldr r2, =_sbss
  b Zerobss

CopyCode:
  ldr r3, =_sicode
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

CodeInit:
  ldr r0, =_scode
  ldr r3, =_ecode
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyCode
  movs r1, #0
  b DataInit

FillZerobss:
  movs r3, #0
  str r3, [r2], #4

Zerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss
/* Call SystemInit function */
  bl  SystemInit
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
  bl  main

LoopForever:
    b LoopForever


.size  Reset_Handler, .-Reset_Handler



    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler


   .section  .vectors,"a",%progbits
   .global __Vectors



__Vectors:
                .word     Image$$ARM_LIB_STACKHEAP$$ZI$$Limit                      /* Top of Stack */
                .word     Reset_Handler                     /* Reset Handler */
                .word     NMI_Handler                       /* NMI Handler */
                .word     HardFault_Handler                 /* Hard Fault Handler */
                .word     MemManage_Handler                 /* MPU Fault Handler */
                .word     BusFault_Handler                  /* Bus Fault Handler */
                .word     UsageFault_Handler                /* Usage Fault Handler */
                .word     SecureFault_Handler               /* Secure Fault Handler */
                .word     0                                 /* Reserved */
                .word     0                                 /* Reserved */
                .word     0                                 /* Reserved */
                .word     SVC_Handler                       /* SVCall Handler */
                .word     DebugMon_Handler                  /* Debug Monitor Handler */
                .word     0                                 /* Reserved */
                .word     PendSV_Handler                    /* PendSV Handler */
                .word     SysTick_Handler                   /* SysTick Handler */

                /* external interrupts handler */
                .word     WWDGT_IRQHandler                  /* 16:Window Watchdog Timer */
                .word     LVD_IRQHandler                    /* 17:LVD through EXTI Line detect */
                .word     TAMPER_STAMP_IRQHandler           /* 18:Tamper and TimeStamp through EXTI Line detect */
                .word     RTC_WKUP_IRQHandler               /* 19:RTC Wakeup through EXTI Line */
                .word     FMC_IRQHandler                    /* 20:FMC */
                .word     RCU_IRQHandler                    /* 21:RCU */
                .word     EXTI0_IRQHandler                  /* 22:EXTI Line 0 */
                .word     EXTI1_IRQHandler                  /* 23:EXTI Line 1 */
                .word     EXTI2_IRQHandler                  /* 24:EXTI Line 2 */
                .word     EXTI3_IRQHandler                  /* 25:EXTI Line 3 */
                .word     EXTI4_IRQHandler                  /* 26:EXTI Line 4 */
                .word     DMA0_Channel0_IRQHandler          /* 27:DMA0 Channel0 */
                .word     DMA0_Channel1_IRQHandler          /* 28:DMA0 Channel1 */
                .word     DMA0_Channel2_IRQHandler          /* 29:DMA0 Channel2 */
                .word     DMA0_Channel3_IRQHandler          /* 30:DMA0 Channel3 */
                .word     DMA0_Channel4_IRQHandler          /* 31:DMA0 Channel4 */
                .word     DMA0_Channel5_IRQHandler          /* 32:DMA0 Channel5 */
                .word     DMA0_Channel6_IRQHandler          /* 33:DMA0 Channel6 */
                .word     DMA0_Channel7_IRQHandler          /* 34:DMA0 Channel7 */
                .word     ADC_IRQHandler                    /* 35:ADC */
                .word     TAMP_STAMP_S_IRQHandler           /* 36:RTC tamper security interrupt */
                .word     RTC_WKUP_S_IRQHandler             /* 37:RTC wakeup security interrupt */
                .word     RTC_Alarm_S_IRQHandler            /* 38:RTC Alarm security interrupt */
                .word     EXTI5_9_IRQHandler                /* 39:EXTI5 to EXTI9 */
                .word     TIMER0_BRK_IRQHandler             /* 40:TIMER0 Break */
                .word     TIMER0_UP_IRQHandler              /* 41:TIMER0 Update */
                .word     TIMER0_CMT_IRQHandler             /* 42:TIMER0 Commutation */
                .word     TIMER0_Channel_IRQHandler         /* 43:TIMER0 Channel Capture Compare */
                .word     TIMER1_IRQHandler                 /* 44:TIMER1 */
                .word     TIMER2_IRQHandler                 /* 45:TIMER2 */
                .word     TIMER3_IRQHandler                 /* 46:TIMER3 */
                .word     I2C0_EV_IRQHandler                /* 47:I2C0 Event */
                .word     I2C0_ER_IRQHandler                /* 48:I2C0 Error */
                .word     I2C1_EV_IRQHandler                /* 49:I2C1 Event */
                .word     I2C1_ER_IRQHandler                /* 50:I2C1 Error */
                .word     SPI0_IRQHandler                   /* 51:SPI0 */
                .word     SPI1_IRQHandler                   /* 52:SPI1/I2S1 */
                .word     USART0_IRQHandler                 /* 53:USART0 */
                .word     USART1_IRQHandler                 /* 54:USART1 */
                .word     USART2_IRQHandler                 /* 55:USART2 */
                .word     EXTI10_15_IRQHandler              /* 56:EXTI10 to EXTI15 */
                .word     RTC_Alarm_IRQHandler              /* 57:RTC Alarm */
                .word     PVM_IRQHandler                    /* 58:EXTI[18](PVMO) */
                .word     0                                 /* 59:Reserved */
                .word     TIMER15_IRQHandler                /* 60:TIMER15 */
                .word     TIMER16_IRQHandler                /* 61:TIMER16 */
                .word     0                                 /* 62:Reserved */
                .word     0                                 /* 63:Reserved */
   .size   __Vectors, . - __Vectors


  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler


  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler


  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler


  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler


  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler


  .weak SecureFault_Handler
  .thumb_set SecureFault_Handler,Default_Handler


  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler


  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler


  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler


  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler


  .weak WWDGT_IRQHandler
  .thumb_set WWDGT_IRQHandler,Default_Handler


  .weak LVD_IRQHandler
  .thumb_set LVD_IRQHandler,Default_Handler


  .weak TAMPER_STAMP_IRQHandler
  .thumb_set TAMPER_STAMP_IRQHandler,Default_Handler


  .weak RTC_WKUP_IRQHandler
  .thumb_set RTC_WKUP_IRQHandler,Default_Handler


  .weak FMC_IRQHandler
  .thumb_set FMC_IRQHandler,Default_Handler


  .weak RCU_IRQHandler
  .thumb_set RCU_IRQHandler,Default_Handler


  .weak EXTI0_IRQHandler
  .thumb_set EXTI0_IRQHandler,Default_Handler


  .weak EXTI1_IRQHandler
  .thumb_set EXTI1_IRQHandler,Default_Handler


  .weak EXTI2_IRQHandler
  .thumb_set EXTI2_IRQHandler,Default_Handler


  .weak EXTI3_IRQHandler
  .thumb_set EXTI3_IRQHandler,Default_Handler


  .weak EXTI4_IRQHandler
  .thumb_set EXTI4_IRQHandler,Default_Handler


  .weak DMA0_Channel0_IRQHandler
  .thumb_set DMA0_Channel0_IRQHandler,Default_Handler


  .weak DMA0_Channel1_IRQHandler
  .thumb_set DMA0_Channel1_IRQHandler,Default_Handler


  .weak DMA0_Channel2_IRQHandler
  .thumb_set DMA0_Channel2_IRQHandler,Default_Handler


  .weak DMA0_Channel3_IRQHandler
  .thumb_set DMA0_Channel3_IRQHandler,Default_Handler


  .weak DMA0_Channel4_IRQHandler
  .thumb_set DMA0_Channel4_IRQHandler,Default_Handler


  .weak DMA0_Channel5_IRQHandler
  .thumb_set DMA0_Channel5_IRQHandler,Default_Handler


  .weak DMA0_Channel6_IRQHandler
  .thumb_set DMA0_Channel6_IRQHandler,Default_Handler


  .weak DMA0_Channel7_IRQHandler
  .thumb_set DMA0_Channel7_IRQHandler,Default_Handler


  .weak ADC_IRQHandler
  .thumb_set ADC_IRQHandler,Default_Handler


  .weak TAMP_STAMP_S_IRQHandler
  .thumb_set TAMP_STAMP_S_IRQHandler,Default_Handler


  .weak RTC_WKUP_S_IRQHandler
  .thumb_set RTC_WKUP_S_IRQHandler,Default_Handler


  .weak RTC_Alarm_S_IRQHandler
  .thumb_set RTC_Alarm_S_IRQHandler,Default_Handler


  .weak EXTI5_9_IRQHandler
  .thumb_set EXTI5_9_IRQHandler,Default_Handler


  .weak TIMER0_BRK_IRQHandler
  .thumb_set TIMER0_BRK_IRQHandler,Default_Handler


  .weak TIMER0_UP_IRQHandler
  .thumb_set TIMER0_UP_IRQHandler,Default_Handler


  .weak TIMER0_CMT_IRQHandler
  .thumb_set TIMER0_CMT_IRQHandler,Default_Handler


  .weak TIMER0_Channel_IRQHandler
  .thumb_set TIMER0_Channel_IRQHandler,Default_Handler


  .weak TIMER1_IRQHandler
  .thumb_set TIMER1_IRQHandler,Default_Handler


  .weak TIMER2_IRQHandler
  .thumb_set TIMER2_IRQHandler,Default_Handler


  .weak TIMER3_IRQHandler
  .thumb_set TIMER3_IRQHandler,Default_Handler


  .weak I2C0_EV_IRQHandler
  .thumb_set I2C0_EV_IRQHandler,Default_Handler


  .weak I2C0_ER_IRQHandler
  .thumb_set I2C0_ER_IRQHandler,Default_Handler


  .weak I2C1_EV_IRQHandler
  .thumb_set I2C1_EV_IRQHandler,Default_Handler


  .weak I2C1_ER_IRQHandler
  .thumb_set I2C1_ER_IRQHandler,Default_Handler


  .weak SPI0_IRQHandler
  .thumb_set SPI0_IRQHandler,Default_Handler


  .weak SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler


  .weak USART0_IRQHandler
  .thumb_set USART0_IRQHandler,Default_Handler


  .weak USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler


  .weak USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler


  .weak EXTI10_15_IRQHandler
  .thumb_set EXTI10_15_IRQHandler,Default_Handler


  .weak RTC_Alarm_IRQHandler
  .thumb_set RTC_Alarm_IRQHandler,Default_Handler


  .weak PVM_IRQHandler
  .thumb_set PVM_IRQHandler,Default_Handler


  .weak TIMER15_IRQHandler
  .thumb_set TIMER15_IRQHandler,Default_Handler


  .weak TIMER16_IRQHandler
  .thumb_set TIMER16_IRQHandler,Default_Handler
