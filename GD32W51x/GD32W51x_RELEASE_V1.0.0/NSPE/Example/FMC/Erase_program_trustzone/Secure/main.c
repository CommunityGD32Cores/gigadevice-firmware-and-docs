/*!
    \file    main.c
    \brief   erase and program flash example, secure code with TZEN = 1

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
#include "main.h"
#include <arm_cmse.h>

#define TZBMPC1_BLOCK_NUMBER         255                           /* TZBMPC1 block number */
#define NONSECURE_START              ((uint32_t)0x08040000U)       /* nonsecure start address */

#define FMC_PAGE_SIZE                ((uint16_t)0x1000U)
#define FMC_USER_SEC_START_ADDR      ((uint32_t)0x0C020000U)       /* user start page address */
#define FMC_USER_SEC_END_ADDR        ((uint32_t)0x0C022000U)       /* user end page address */

uint32_t *ptrd;
uint32_t address = 0x00;
uint32_t data = 0x01234567U;

/* calculate the number of page to be programmed/erased */
uint32_t page_num = (FMC_USER_SEC_END_ADDR - FMC_USER_SEC_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t word_num = ((FMC_USER_SEC_END_ADDR - FMC_USER_SEC_START_ADDR) >> 2);

/* enable trustzone */
void fmc_trustzone_enable(void);
/* configure sram security */
void tzbmpc_config();
/* initialize non-secure application */
void nonsecure_init();
/* non-secure call function: initialize LED2/LED3 */
CMSE_NS_ENTRY void gd_eval_leds_init(void);
/* erase fmc pages from FMC_USER_SEC_START_ADDR to FMC_USER_SEC_END_ADDR */
void fmc_erase_pages(void);
/* program fmc word by word from FMC_USER_SEC_START_ADDR to FMC_USER_SEC_END_ADDR */
void fmc_program(void);
/* check fmc program result */
ErrStatus fmc_program_check(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable SecureFault handler */
    SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

    /* configure TZBMPC */
    tzbmpc_config();

    /* initialize leds */
    gd_eval_led_init(LED1);

    /* step1: erase pages */
    fmc_erase_pages();

    /* step2: program and check if it is successful. If successful, light the LED1 */
    fmc_program();
    if(ERROR != fmc_program_check()){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_off(LED1);
    }
    
    /* Setup and jump to non-secure */
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
void tzbmpc_config(void)
{
    uint16_t block_number = 0U;
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);

    /* SRAM1 is used to nonsecure code, so all blocks of SRAM1 should set to nonsecure */
    for(block_number = 0U; block_number <= TZBMPC1_BLOCK_NUMBER; block_number++){
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, block_number, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
}

/*!
    \brief      configure and jump to non-secure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nonsecure_init()
{
    uint32_t nonsecure_stackpointer = (*((uint32_t *)(NONSECURE_START)));
    ns_fptr nonsecure_reset_handler = (ns_fptr)(*((uint32_t *)(NONSECURE_START + 4U)));
    /* set non-secure vector table location */
    SCB_NS->VTOR = NONSECURE_START;
    /* set non-secure stack pointer */
    __TZ_set_MSP_NS(nonsecure_stackpointer);
    /* start non-secure application */
    nonsecure_reset_handler();
}

/*!
    \brief      non-secure call function: initialize LED2/LED3
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void gd_eval_leds_init(void)
{
    /* configure LED2/3 IO non-secure */
    gd_eval_led_init(LED2);
    gpio_bit_reset_sec_cfg(LED2_GPIO_PORT, LED2_PIN);
    gd_eval_led_init(LED3);
    gpio_bit_reset_sec_cfg(LED3_GPIO_PORT, LED3_PIN);
}

/*!
    \brief      erase fmc pages from FMC_USER_SEC_START_ADDR to FMC_USER_SEC_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(void)
{
    uint32_t EraseCounter;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    
    /* erase the flash pages */
    for(EraseCounter = 0; EraseCounter < page_num; EraseCounter++){
        fmc_page_erase(FMC_USER_SEC_START_ADDR + (FMC_PAGE_SIZE * EraseCounter));
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc word by word from FMC_USER_SEC_START_ADDR to FMC_USER_SEC_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_USER_SEC_START_ADDR;

    /* program flash */
    while(address < FMC_USER_SEC_END_ADDR){
        fmc_word_program(address, data);
        address += 4;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_USER_SEC_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0; i < word_num; i++){
        if((*ptrd) != data){
            return ERROR;
            break;
        }else{
            ptrd++;
        }
    }
    return SUCCESS;
}
