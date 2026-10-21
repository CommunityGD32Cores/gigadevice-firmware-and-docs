/*!
    \file    gd32f1x0_dac.h
    \brief   definitions for the DAC

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=3,5)
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

#ifndef GD32F1X0_DAC_H
#define GD32F1X0_DAC_H

#include "gd32f1x0.h"

/* DACx(x=0) definitions */
#define DAC0                              (DAC_BASE)

/* registers definitions */
#define DAC_CTL0(dacx)                    REG32((dacx) + 0x00000000U)          /*!< DACx control register 0 */
#define DAC_SWT(dacx)                     REG32((dacx) + 0x00000004U)          /*!< DACx software trigger register */
#define DAC_OUT0_R12DH(dacx)              REG32((dacx) + 0x00000008U)          /*!< DACx_OUT0 12-bit right-aligned data holding register */
#define DAC_OUT0_L12DH(dacx)              REG32((dacx) + 0x0000000CU)          /*!< DACx_OUT0 12-bit left-aligned data holding register */
#define DAC_OUT0_R8DH(dacx)               REG32((dacx) + 0x00000010U)          /*!< DACx_OUT0 8-bit right-aligned data holding register */
#define DAC_OUT0_DO(dacx)                 REG32((dacx) + 0x0000002CU)          /*!< DACx_OUT0 data output register */
#define DAC_STAT0(dacx)                   REG32((dacx) + 0x00000034U)          /*!< DACx status register 0 */

/* bits definitions */
/* DAC_CTL0 */
#define DAC_CTL0_DEN0                     BIT(0)                               /*!< DACx_OUT0 enable */
#define DAC_CTL0_DBOFF0                   BIT(1)                               /*!< DACx_OUT0 output buffer turn off */
#define DAC_CTL0_DTEN0                    BIT(2)                               /*!< DACx_OUT0 trigger enable */
#define DAC_CTL0_DTSEL0                   BITS(3,5)                            /*!< DACx_OUT0 trigger selection */
#define DAC_CTL0_DDMAEN0                  BIT(12)                              /*!< DACx_OUT0 DMA enable */
#define DAC_CTL0_DDUDRIE0                 BIT(13)                              /*!< DACx_OUT0 DMA underrun interrupt enable */

/* DAC_SWT */
#define DAC_SWT_SWTR0                     BIT(0)                               /*!< DACx_OUT0 software trigger */

/* DAC0_R12DH */
#define DAC_OUT0_DH_R12                   BITS(0,11)                           /*!< DACx_OUT0 12-bit right-aligned data */

/* DAC0_L12DH */
#define DAC_OUT0_DH_L12                   BITS(4,15)                           /*!< DACx_OUT0 12-bit left-aligned data */

/* DAC0_R8DH */
#define DAC_OUT0_DH_R8                    BITS(0,7)                            /*!< DACx_OUT0 8-bit right-aligned data */

/* DAC0_DO */
#define DAC_OUT0_DO_BITS                  BITS(0,11)                           /*!< DACx_OUT0 12-bit output data */

/* DAC_STAT0 */
#define DAC_STAT0_DDUDR0                  BIT(13)                              /*!< DACx_OUT0 DMA underrun flag */

/* constants definitions */
/* DAC trigger source */
#define CTL_DTSEL(regval)                 (BITS(3,5) & ((uint32_t)(regval) << 3))
#define DAC_TRIGGER_T5_TRGO               CTL_DTSEL(0)                         /*!< TIMER5 TRGO */
#define DAC_TRIGGER_T2_TRGO               CTL_DTSEL(1)                         /*!< TIMER2 TRGO */
#define DAC_TRIGGER_T14_TRGO              CTL_DTSEL(3)                         /*!< TIMER14 TRGO */
#define DAC_TRIGGER_T1_TRGO               CTL_DTSEL(4)                         /*!< TIMER1 TRGO */
#define DAC_TRIGGER_EXTI_9                CTL_DTSEL(6)                         /*!< EXTI interrupt line9 event */
#define DAC_TRIGGER_SOFTWARE              CTL_DTSEL(7)                         /*!< software trigger */

/* DAC data alignment */
#define DATA_ALIGN(regval)                (BITS(0,1) & ((uint32_t)(regval) << 0))
#define DAC_ALIGN_12B_R                   DATA_ALIGN(0)                        /*!< 12-bit right-aligned data */
#define DAC_ALIGN_12B_L                   DATA_ALIGN(1)                        /*!< 12-bit left-aligned data */
#define DAC_ALIGN_8B_R                    DATA_ALIGN(2)                        /*!< 8-bit right-aligned data */

/* DAC output channel definitions */
#define DAC_OUT0                          ((uint8_t)0x00U)                     /*!< DACx_OUT0 channel */

/* DAC interrupt */
#define DAC_INT_DDUDR0                    DAC_CTL0_DDUDRIE0                    /*!< DACx_OUT0 DMA underrun interrupt enable */

/* DAC interrupt flag */
#define DAC_INT_FLAG_DDUDR0               DAC_STAT0_DDUDR0                     /*!< DACx_OUT0 DMA underrun interrupt flag */

/* DAC flags */
#define DAC_FLAG_DDUDR0                   DAC_STAT0_DDUDR0                     /*!< DACx_OUT0 DMA underrun flag */

/* function declarations */
/* DAC initialization functions */
/* deinitialize DAC */
void dac_deinit(uint32_t dac_periph);
/* enable DAC */
void dac_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC */
void dac_disable(uint32_t dac_periph, uint8_t dac_out);
/* enable DAC DMA function */
void dac_dma_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC DMA function */
void dac_dma_disable(uint32_t dac_periph, uint8_t dac_out);

/* DAC buffer functions */
/* enable DAC output buffer */
void dac_output_buffer_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC output buffer */
void dac_output_buffer_disable(uint32_t dac_periph, uint8_t dac_out);

/* read and write operation functions */
/* get DAC output value */
uint16_t dac_output_value_get(uint32_t dac_periph, uint8_t dac_out);
/* set DAC data holding register value */
void dac_data_set(uint32_t dac_periph, uint8_t dac_out, uint32_t dac_align, uint16_t data);

/* DAC trigger configuration */
/* enable DAC trigger */
void dac_trigger_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC trigger */
void dac_trigger_disable(uint32_t dac_periph, uint8_t dac_out);
/* configure DAC trigger source */
void dac_trigger_source_config(uint32_t dac_periph, uint8_t dac_out, uint32_t triggersource);
/* enable DAC software trigger */
void dac_software_trigger_enable(uint32_t dac_periph, uint8_t dac_out);

/* DAC interrupt and flag functions */
/* get DAC flag */
FlagStatus dac_flag_get(uint32_t dac_periph, uint32_t flag);
/* clear DAC flag */
void dac_flag_clear(uint32_t dac_periph, uint32_t flag);
/* enable DAC interrupt */
void dac_interrupt_enable(uint32_t dac_periph, uint32_t interrupt);
/* disable DAC interrupt */
void dac_interrupt_disable(uint32_t dac_periph, uint32_t interrupt);
/* get DAC interrupt flag */
FlagStatus dac_interrupt_flag_get(uint32_t dac_periph, uint32_t int_flag);
/* clear DAC interrupt flag */
void dac_interrupt_flag_clear(uint32_t dac_periph, uint32_t int_flag);

#endif /* GD32F1X0_DAC_H */
