/*!
    \file    main.c
    \brief   USART printf trustzone
    
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

#include "gd32w51x.h"
#include "gd32w515p_eval.h"
#include <stdio.h>
#include "main.h"
#include <arm_cmse.h>

#define NONSECURE_START  ((uint32_t)0x08040000U)       /* nonsecure start address */

void fmc_trustzone_enable(void);
void tzbmpc_config();
void nonsecure_init();
/* configure USART to non-secure */
CMSE_NS_ENTRY void usart_nonsecure_cfg(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure TZBMPC */
    tzbmpc_config();

    /* setup and jump to non-secure */
    nonsecure_init();

    while(1){
    }
}

/*!
    \brief      configure tzbmpc
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tzbmpc_config()
{
    uint16_t block_number = 0;
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);
    
    /* SRAM1 is used to non-secure code, so all blocks of SRAM1 should set to non-secure */
    for(block_number = 0; block_number <= TZBMPC1_BLOCK_NUMBER; block_number++){
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, block_number, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
}

/*!
    \brief      setup and jump to non-secure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nonsecure_init()
{
    uint32_t NonSecure_StackPointer = (*((uint32_t *)(NONSECURE_START)));
    funcptr_NS NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)(NONSECURE_START + 4U)));
    /* set non-secure vector table location */
    SCB_NS->VTOR = NONSECURE_START;
    /* set non-secure stack pointer */
    __TZ_set_MSP_NS(NonSecure_StackPointer);
    /* start non-secure application */
    NonSecure_ResetHandler();
}

/*!
    \brief      configure PB10, PB11 and USART to nonsecure
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void usart_nonsecure_cfg(void)
{
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable the RCU_TZGPC clock */
    rcu_periph_clock_enable(RCU_TZPCU);
    /* configure the PB10 and PB11 pin to non-secure */
    gpio_bit_reset_sec_cfg(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
    /* configure USART0 to non-secure */
    tzgpc_tzspc_peripheral_attributes_config(TZGPC_PERIPH_USART2, TZGPC_NSEC);
}
