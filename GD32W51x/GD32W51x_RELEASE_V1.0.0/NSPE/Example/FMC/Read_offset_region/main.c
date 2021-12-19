/*!
    \file    main.c
    \brief   read offset flash region example

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
uint32_t data0 = 0x01234567U;
uint32_t data1 = 0x89abcdefU;

#define FMC_PAGE_SIZE            ((uint16_t)0x1000U)        /*!< size of page */
#define PAGE_OFFSET_VALUE        ((uint32_t)0x00000010U)    /*!< FMC offset value: offset 16 pages */
#define SOURCE_START_PAGE        ((uint32_t)0x00000010U)    /*!< start page of source region: page 16 */
#define SOURCE_END_PAGE          ((uint32_t)0x00000011U)    /*!< end page of source region: page 17 */
#define SOURCE_START_ADDR        ((uint32_t)0x08010000U)    /*!< start address of source region: 0x08010000 */
#define SOURCE_END_ADDR          ((uint32_t)0x08011000U)    /*!< end address of source region: 0x08011000 */
#define ADDOFFSET_START_PAGE     ((uint32_t)0x00000020U)    /*!< start page of add offset region: page 32 */
#define ADDOFFSET_END_PAGE       ((uint32_t)0x00000021U)    /*!< end page of add offset region: page 33 */
#define ADDOFFSET_START_ADDR     ((uint32_t)0x08020000U)    /*!< start address of add offset region: 0x08050000 */
#define ADDOFFSET_END_ADDR       ((uint32_t)0x08021000U)    /*!< end address of add offset region: 0x08051000 */

/* calculate the number of page to be programmed/erased */
uint32_t page_num = (SOURCE_END_ADDR - SOURCE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t word_num = ((SOURCE_END_ADDR - SOURCE_START_ADDR) >> 2);

/* erase flash pages */
void fmc_erase_pages(uint32_t start_page_addr, uint32_t page_num);
/* word program flash */
void fmc_program(uint32_t start_addr, uint32_t word_num, uint32_t data);
/* check fmc program result */
ErrStatus fmc_program_check(uint32_t start_addr, uint32_t word_num, uint32_t compare_data);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize leds */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    
    /* step1: erase source region */
    fmc_erase_pages(SOURCE_START_ADDR, page_num);

    /* step2: program source region with data0 and check if it is successful. If successful, light the LED1 */
    fmc_program(SOURCE_START_ADDR, word_num, data0);
    if(ERROR != fmc_program_check(SOURCE_START_ADDR, word_num, data0)){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_off(LED1);
    }

    /* step3: erase add offset region */
    fmc_erase_pages(ADDOFFSET_START_ADDR, page_num);

    /* step4: program add offset region with data1 and check if it is successful. If successful, light the LED2 */
    fmc_program(ADDOFFSET_START_ADDR, word_num, data1);
    if(ERROR != fmc_program_check(ADDOFFSET_START_ADDR, word_num, data1)){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED2);
    }

    /* step5: configure offset region */
    fmc_unlock();
    ob_unlock();
    fmc_offset_region_config(SOURCE_START_PAGE, SOURCE_END_PAGE);
    fmc_offset_value_config(PAGE_OFFSET_VALUE);
    ob_lock();
    fmc_lock();

    /* step6: reading the value of the source region and check if it is equivalent to data1(add offset region value). if successful, light the LED3 */
    if(ERROR != fmc_program_check(SOURCE_START_ADDR, word_num, data1)){
        gd_eval_led_on(LED3);
    }else{
        gd_eval_led_off(LED3);
    }

    while(1){
    }
}

/*!
    \brief      erase flash pages
    \param[in]  start_page_addr: the start address of pages to be erased
    \param[in]  page_num: the number of pages to be erased
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(uint32_t start_page_addr, uint32_t page_num)
{
    uint32_t EraseCounter;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    
    /* erase the flash pages */
    for(EraseCounter = 0; EraseCounter < page_num; EraseCounter++){
        fmc_page_erase(start_page_addr + (FMC_PAGE_SIZE * EraseCounter));
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      word program flash
    \param[in]  start_addr: the start address to be programmed
    \param[in]  word_num: the number of words to be programmed
    \param[in]  data: the data to be programmed to flash
    \param[out] none
    \retval     none
*/
void fmc_program(uint32_t start_addr, uint32_t word_num, uint32_t data)
{
    uint32_t ProgramCounter;
    uint32_t address = start_addr;
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* program flash */
    for(ProgramCounter = 0; ProgramCounter < word_num; ProgramCounter++){
        fmc_word_program(address, data);
        address += 4;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc program result
    \param[in]  start_addr: the start address to be checked
    \param[in]  word_num: the number of words to be checked
    \param[in]  compare_data: the data to be commpared with data reading from flash
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus fmc_program_check(uint32_t start_addr, uint32_t word_num, uint32_t compare_data)
{
    uint32_t i;

    ptrd = (uint32_t *)start_addr;

    /* check flash whether has been programmed */
    for(i = 0; i < word_num; i++){
        if((*ptrd) != compare_data){
            return ERROR;
            break;
        }else{
            ptrd++;
        }
    }
    return SUCCESS;
}
