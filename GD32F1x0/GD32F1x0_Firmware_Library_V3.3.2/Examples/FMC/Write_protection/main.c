/*!
    \file    main.c
    \brief   main flash pages write protection

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#include "gd32f1x0.h" 
#include "gd32f1x0r_eval.h"

#define FLASH_PAGE_PROGRAM
#define WRITE_PROTECTION_ENABLE 
//#define WRITE_PROTECTION_DISABLE

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;
#define FLASH_PAGE_SIZE ((uint16_t)0x400)
#define FMC_PAGES_PROTECTED (OB_WP_6 | OB_WP_7)

#define WRITE_START_ADDR  ((uint32_t)0x08006000)
#define WRITE_END_ADDR    ((uint32_t)0x08008000)

uint32_t erase_counter = 0x0, Address = 0x0;
uint16_t data = 0x1753U, wp_value = 0xFFFFU, protected_pages = 0x0000U;
uint32_t page_num;
__IO fmc_state_enum fmc_state = FMC_READY;
__IO test_state program_state = PASSED;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* initialize led on the board */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* unlock the flash program/erase controller */
    fmc_unlock();
    ob_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* Get current pages write protection status */
    wp_value = ob_write_protection_get();

#ifdef WRITE_PROTECTION_DISABLE
    /* Get pages already write protected */
    protected_pages = ~(wp_value | FMC_PAGES_PROTECTED);

    wp_value |= (~FMC_PAGES_PROTECTED);
    /* Check if desired pages are already write protected */
    if(wp_value != 0xFFFFU){

        /* Check if there is write protected pages */
        if(protected_pages != 0x0000U){
            /* Restore write protected pages */
            fmc_state = ob_write_protection_enable(protected_pages);
        }
        /* Reload the new option byte values */
        ob_reset();
    }

#elif defined WRITE_PROTECTION_ENABLE
    /* Get current write protected pages and the new pages to be protected */
    protected_pages = (~wp_value) | FMC_PAGES_PROTECTED;

    /* Check if desired pages are not yet write protected */
    if(((~wp_value) & FMC_PAGES_PROTECTED)!= FMC_PAGES_PROTECTED){

        /* Enable the pages write protection */
        fmc_state = ob_write_protection_enable(protected_pages);

        /* Reload the new option byte values */
        ob_reset();
    }
#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_PAGE_PROGRAM
    /* Get the number of pages to be erased */
    page_num = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    /* The selected pages are not write protected */
    if ( (wp_value & FMC_PAGES_PROTECTED) != 0x0000){
        /* Clear All pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

        /* erase the FLASH pages */
        for(erase_counter = 0; (erase_counter < page_num) && (fmc_state == FMC_READY); erase_counter++){
            fmc_state = fmc_page_erase(WRITE_START_ADDR + (FLASH_PAGE_SIZE * erase_counter));
        }
  
        /* FLASH Half Word program of data 0x1753 at addresses defined by WRITE_START_ADDR and WRITE_END_ADDR */
        Address = WRITE_START_ADDR;

        while((Address < WRITE_END_ADDR) && (fmc_state == FMC_READY)){
            fmc_state = fmc_halfword_program(Address, data);
            Address = Address + 2;
        }

        /* Check the correctness of written data */
        Address = WRITE_START_ADDR;

        while((Address < WRITE_END_ADDR) && (program_state != FAILED)){
            if((*(__IO uint16_t*) Address) != data){
                program_state = FAILED;
            }
            Address += 2;
        }
    }else{
        /* Error to program the flash : The desired pages are write protected */ 
        program_state = FAILED;
    }
    if(FAILED == program_state){
        gd_eval_led_on(LED3);
    }else{
        gd_eval_led_on(LED2);
    }
#endif /* FLASH_PAGE_PROGRAM */
    while(1){
    }
}
