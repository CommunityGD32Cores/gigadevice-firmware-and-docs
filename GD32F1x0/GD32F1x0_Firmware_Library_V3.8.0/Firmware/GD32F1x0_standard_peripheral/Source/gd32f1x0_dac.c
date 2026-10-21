/*!
    \file    gd32f1x0_dac.c
    \brief   DAC driver

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

#include "gd32f1x0_dac.h"

/* DAC register bit offset */
#define OUT1_REG_OFFSET           ((uint32_t)0x00000010U)
#define DH_12BIT_OFFSET           ((uint32_t)0x00000010U)
#define DH_8BIT_OFFSET            ((uint32_t)0x00000008U)

#define DAC_STAT_FLAG_MASK0       (DAC_INT_FLAG_DDUDR0)
#define DAC_INT_EN_MASK0          (DAC_INT_DDUDR0)
#define DAC_INT_FLAG_MASK0        (DAC_INT_FLAG_DDUDR0)

/*!
    \brief      deinitialize DAC
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_deinit(uint32_t dac_periph)
{
    switch(dac_periph){
        case DAC0:
            /* reset DAC0 */
            rcu_periph_reset_enable(RCU_DACRST);
            rcu_periph_reset_disable(RCU_DACRST);
            break;
        default:
            break;
    }
}

/*!
    \brief      enable DAC
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DEN0;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DEN0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC DMA function
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_dma_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DDMAEN0;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC DMA function
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_dma_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DDMAEN0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC output buffer
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DBOFF0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC output buffer
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DBOFF0;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      get DAC output value
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     DAC output data: 0~4095
*/
uint16_t dac_output_value_get(uint32_t dac_periph, uint8_t dac_out)
{
    uint16_t data = 0U;

    if(DAC_OUT0 == dac_out){
        /* store the DACx_OUT0 output value */
        data = (uint16_t)DAC_OUT0_DO(dac_periph);
    }else{
        /* illegal parameters */
    }

    return data;
}

/*!
    \brief      set DAC data holding register value
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[in]  dac_align: DAC data alignment mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_12B_R: 12-bit right-aligned data
      \arg        DAC_ALIGN_12B_L: 12-bit left-aligned data
      \arg        DAC_ALIGN_8B_R: 8-bit right-aligned data
    \param[in]  data: data to be loaded(0~4095)
    \param[out] none
    \retval     none
*/
void dac_data_set(uint32_t dac_periph, uint8_t dac_out, uint32_t dac_align, uint16_t data)
{
    /* DAC_OUT0 data alignment */
    if(DAC_OUT0 == dac_out){
        switch(dac_align){
        /* 12-bit right-aligned data */
        case DAC_ALIGN_12B_R:
            DAC_OUT0_R12DH(dac_periph) = data;
            break;
        /* 12-bit left-aligned data */
        case DAC_ALIGN_12B_L:
            DAC_OUT0_L12DH(dac_periph) = data;
            break;
        /* 8-bit right-aligned data */
        case DAC_ALIGN_8B_R:
            DAC_OUT0_R8DH(dac_periph) = data;
            break;
        default:
            break;
        }
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_trigger_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DTEN0;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[out] none
    \retval     none
*/
void dac_trigger_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DTEN0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      configure DAC trigger source
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \param[in]  triggersource: external trigger of DAC
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIGGER_T5_TRGO: TIMER5 TRGO
      \arg        DAC_TRIGGER_T14_TRGO: TIMER14 TRGO
      \arg        DAC_TRIGGER_T2_TRGO: TIMER2 TRGO
      \arg        DAC_TRIGGER_T1_TRGO: TIMER1 TRGO
      \arg        DAC_TRIGGER_EXTI_9: EXTI interrupt line9 event
      \arg        DAC_TRIGGER_SOFTWARE: software trigger
    \param[out] none
    \retval     none
*/
void dac_trigger_source_config(uint32_t dac_periph, uint8_t dac_out, uint32_t triggersource)
{
    if(DAC_OUT0 == dac_out){
        /* configure DACx_OUT0 trigger source */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DTSEL0);
        DAC_CTL0(dac_periph) |= triggersource;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC software trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0)
    \retval     none
*/
void dac_software_trigger_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_SWT(dac_periph) |= (uint32_t)DAC_SWT_SWTR0;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      get DAC flag
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  flag: the DAC status flags, only one parameter can be selected which is shown
                as below:
      \arg        DAC_FLAG_DDUDR0: DACx_OUT0 DMA underrun flag
    \param[out] none
    \retval     the state of DAC bit(SET or RESET)
*/
FlagStatus dac_flag_get(uint32_t dac_periph, uint32_t flag)
{
    FlagStatus Status = RESET;
    if(flag & DAC_STAT_FLAG_MASK0){
        /* check DAC_STAT0 flag */
        if(RESET != (DAC_STAT0(dac_periph) & flag)){
            Status = SET;
        }else{
            Status = RESET;
        }
    }else{
        /* illegal parameters */
        Status =  RESET;
    }
    return Status;
}

/*!
    \brief      clear DAC flag
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  flag: DAC flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_FLAG_DDUDR0: DACx_OUT0 DMA underrun flag
    \param[out] none
    \retval     none
*/
void dac_flag_clear(uint32_t dac_periph, uint32_t flag)
{
    if(flag & DAC_STAT_FLAG_MASK0){
        /* check DAC_STAT0 flag */
        DAC_STAT0(dac_periph) = (uint32_t)(flag & DAC_STAT_FLAG_MASK0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC interrupt
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  interrupt: the DAC interrupt
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_DDUDR0: DACx_OUT0 DMA underrun interrupt
    \param[out] none
    \retval     none
*/
void dac_interrupt_enable(uint32_t dac_periph, uint32_t interrupt)
{
    if(interrupt & DAC_INT_EN_MASK0){
        /* enable underrun interrupt */
        DAC_CTL0(dac_periph) |= (uint32_t)(interrupt & DAC_INT_EN_MASK0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC interrupt
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  interrupt: the DAC interrupt
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_DDUDR0: DACx_OUT0 DMA underrun interrupt
    \param[out] none
    \retval     none
*/
void dac_interrupt_disable(uint32_t dac_periph, uint32_t interrupt)
{
    if(interrupt & DAC_INT_EN_MASK0){
        /* disable underrun interrupt */
        DAC_CTL0(dac_periph) &= (uint32_t)(~(interrupt & DAC_INT_EN_MASK0));
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      get DAC interrupt flag
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  int_flag: DAC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_FLAG_DDUDR0: DACx_OUT0 DMA underrun interrupt flag
    \param[out] none
    \retval     the state of DAC interrupt flag(SET or RESET)
*/
FlagStatus dac_interrupt_flag_get(uint32_t dac_periph, uint32_t int_flag)
{
    uint32_t reg1 = 0U, reg2 = 0U;
    FlagStatus Status = RESET;
    if(int_flag & DAC_INT_FLAG_MASK0){
        /* check underrun interrupt int_flag */
        reg1 = DAC_STAT0(dac_periph) & int_flag;
        reg2 = DAC_CTL0(dac_periph) & int_flag;
    }else{
        /* illegal parameters */
    }

    /*get DAC interrupt flag status */
    if((RESET != reg1) && (RESET != reg2)){
        Status = SET;
    }else{
        Status = RESET;
    }
    return Status;
}

/*!
    \brief      clear DAC interrupt flag
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  int_flag: DAC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_FLAG_DDUDR0: DACx_OUT0 DMA underrun interrupt flag
    \param[out] none
    \retval     none
*/
void dac_interrupt_flag_clear(uint32_t dac_periph, uint32_t int_flag)
{
    /* clear underrun interrupt int_flag */
    if(int_flag & DAC_INT_FLAG_MASK0){
        DAC_STAT0(dac_periph) = (uint32_t)(int_flag & DAC_INT_FLAG_MASK0);
    }else{
        /* illegal parameters */
    }
}
