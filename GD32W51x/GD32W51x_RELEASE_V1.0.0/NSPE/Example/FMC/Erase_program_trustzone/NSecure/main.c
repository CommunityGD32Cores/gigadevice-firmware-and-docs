/*!
    \file    main.c
    \brief   erase and program flash example, non-secure code with TZEN=1
    
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

uint32_t *ptrd;
uint32_t address = 0x00;
uint32_t data = 0x01234567U;

#define FMC_PAGE_SIZE                 ((uint16_t)0x1000U)
#define FMC_USER_NSEC_START_ADDR      ((uint32_t)0x08060000U)       /* user start page address */
#define FMC_USER_NSEC_END_ADDR        ((uint32_t)0x08062000U)       /* user end page address */

/* calculate the number of page to be programmed/erased */
uint32_t page_num = (FMC_USER_NSEC_END_ADDR - FMC_USER_NSEC_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t word_num = ((FMC_USER_NSEC_END_ADDR - FMC_USER_NSEC_START_ADDR) >> 2);

/* non-secure call function: initialize LED2/LED3 */
extern void gd_eval_leds_init(void);
/* erase fmc pages from FMC_USER_SEC_START_ADDR to FMC_USER_SEC_END_ADDR */
void fmc_erase_pages(void);
/* program fmc word by word from FMC_USER_SEC_START_ADDR to FMC_USER_SEC_END_ADDR */
void fmc_program(void);
/* check fmc erase result */
ErrStatus fmc_erase_pages_check(void);
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
    /* initialize leds */
    gd_eval_leds_init();

    /* step1: erase pages and check if it is successful. If successful, light the LED2 */
    fmc_erase_pages();
    if(ERROR != fmc_erase_pages_check()){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED2);
    }
    
    /* step2: program and check if it is successful. If successful, light the LED3 */
    fmc_program();
    if(ERROR != fmc_program_check()){
        gd_eval_led_on(LED3);
    }else{
        gd_eval_led_off(LED3);
    }

    while(1){
    }
}

/*!
    \brief      erase fmc pages from FMC_USER_NSEC_START_ADDR to FMC_USER_NSEC_END_ADDR
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
        fmc_page_erase(FMC_USER_NSEC_START_ADDR + (FMC_PAGE_SIZE * EraseCounter));
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc word by word from FMC_USER_NSEC_START_ADDR to FMC_USER_NSEC_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_USER_NSEC_START_ADDR;

    /* program flash */
    while(address < FMC_USER_NSEC_END_ADDR){
        fmc_word_program(address, data);
        address += 4;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus fmc_erase_pages_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_USER_NSEC_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0; i < word_num; i++){
        if(0xFFFFFFFF != (*ptrd)){
            return ERROR;
            break;
        }else{
            ptrd++;
        }
    }
    return SUCCESS;
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

    ptrd = (uint32_t *)FMC_USER_NSEC_START_ADDR;

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
