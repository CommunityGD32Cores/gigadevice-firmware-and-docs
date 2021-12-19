/*!
    \file    main.c
    \brief   flash secure mark region example, non-secure code with TZEN=1
    
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

#define FMC_PAGE_SIZE           ((uint16_t)0x1000U)
#define FMC_USER_START_ADDR     ((uint32_t)0x08080000U)       /* user start page address */
#define FMC_USER_END_ADDR       ((uint32_t)0x08082000U)       /* user end page address */

uint32_t *ptrd;
uint32_t address = 0U;
uint32_t data = 0x01234567U;
__IO uint32_t flash_secure_status = 0U;

/* calculate the number of page to be programmed/erased */
uint32_t page_num = (FMC_USER_END_ADDR - FMC_USER_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t word_num = ((FMC_USER_END_ADDR - FMC_USER_START_ADDR) >> 2);

/* non-secure call function: disable secure mark region */
extern void fmc_secmark_region_disable(void);
/* non-secure call function: initialize LED2/LED3 */
extern void gd_eval_leds_init(void);
/* check fmc read result */
ErrStatus fmc_read_check(uint32_t compare_data);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* initilize the leds */
    gd_eval_leds_init();

    /* step1: check if it is read as 0. If it is, light the LED2 */
    if(ERROR != fmc_read_check(0)){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED2);
    }
    
    /* disable secure mark region */
    fmc_secmark_region_disable();
    
    /* step2: check if it is read as 0x01234567U. If it is, light the LED3 */
    if(ERROR != fmc_read_check(data)){
        gd_eval_led_on(LED3);
    }else{
        gd_eval_led_off(LED3);
    }
    
    while(1){
    }
}

/*!
    \brief      check fmc read result
    \param[in]  compare_data: the data to be compared
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus fmc_read_check(uint32_t compare_data)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_USER_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0; i < word_num; i++){
        if(compare_data != (*ptrd)){
            return ERROR;
            break;
        }else{
            ptrd++;
        }
    }
    return SUCCESS;
}
