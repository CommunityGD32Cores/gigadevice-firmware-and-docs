/*!
    \file    main.c
    \brief   running LED trustzone, secure code with TZEN = 1
    
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
#include <stdio.h>
#include "main.h"
#include <arm_cmse.h>

#define NONSECURE_START  ((uint32_t)0x08040000U)       /* non-secure start address   */
#define SECM_SPAGE       ((uint32_t)0x00000000U)       /* start page of mark    */
#define SECM_EPAGE       ((uint32_t)0x0000003FU)       /* end page of mark      */

void fmc_trustzone_enable(void);
void tzbmpc_config();
void nonsecure_init();
/* configure LED pin to non-secure */
CMSE_NS_ENTRY void led_nonsecure_cfg(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable trustzone */
    fmc_trustzone_enable();

    fmc_unlock();
    ob_unlock();
    /* configure mark secure pages */
    ob_secmark_config(SECM_SPAGE, SECM_EPAGE, SECM_INDEX2);
    ob_lock();
    fmc_lock();

    /* configure TZBMPC */
    tzbmpc_config();

    /* Setup and jump to non-secure */
    nonsecure_init();

    while(1){
    }
}

/*!
    \brief      enable trustzone
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_trustzone_enable(void)
{
    fmc_unlock();
    ob_unlock();
    trustzone_enable();
    if(!FMC_SECSTAT_SECENDF ){
        ob_reload();
    }
    ob_lock();
    fmc_lock();
}

/*!
    \brief      configure tzbmpc
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tzbmpc_config(void)
{
    uint16_t block_number = 0;
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);

    /* SRAM1 is used to non-secure code, so all blocks of SRAM1 should set to non-secure */
    for(block_number=0U; block_number<=TZBMPC1_BLOCK_NUMBER; block_number++){
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, block_number, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
}

/*!
    \brief      Setup and jump to non-secure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nonsecure_init(void)
{
    uint32_t NonSecure_StackPointer = (*((uint32_t *)(NONSECURE_START)));
    funcptr_NS NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)(NONSECURE_START + 4U)));
    /* Set non-secure vector table location */
    SCB_NS->VTOR = NONSECURE_START;
    /* Set non-secure stack pointer */
    __TZ_set_MSP_NS(NonSecure_StackPointer);
    /* Start non-secure application */
    NonSecure_ResetHandler();
}

/*!
    \brief      configure LED pin to nonsecure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_nonsecure_cfg(void)
{
    /* enable the LED GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* configure the LED pin to non-secure */
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_15);
    gpio_bit_reset_sec_cfg(GPIOB, GPIO_PIN_6 | GPIO_PIN_8);
}
