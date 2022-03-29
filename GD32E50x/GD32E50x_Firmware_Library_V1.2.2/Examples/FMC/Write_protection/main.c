/*!
    \file    main.c
    \brief   main flash program, write_protection

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x.h" 
#include "gd32e507z_eval.h"
#include <stdio.h>

#define FLASH_PAGE_PROGRAM
//#define WRITE_PROTECTION_ENABLE
#define WRITE_PROTECTION_DISABLE

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;
#define FLASH_PAGE_SIZE ((uint16_t)0x2000)
#define FMC_PAGES_PROTECTED (OB_WP_1 | OB_WP_2)

#define BANK_WRITE_START_ADDR  ((uint32_t)0x08008000)
#define BANK_WRITE_END_ADDR    ((uint32_t)0x0800A000)

uint32_t erase_counter = 0x0, Address = 0x0;
uint16_t data = 0x1753;
uint32_t wp_value = 0xFFFFFFFF, protected_pages = 0x0;
uint32_t NbrOfPage;
__IO fmc_state_enum fmc_state = FMC_READY;
__IO test_state program_state = PASSED;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize leds on the board */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* unlock the flash program/erase controller */
    fmc_unlock();
    ob_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR);
    
    /* Get pages write protection status */
    wp_value = ob_write_protection_get();

#ifdef WRITE_PROTECTION_DISABLE
    /* Get pages already write protected */
    protected_pages = ~(wp_value | FMC_PAGES_PROTECTED);

    /* Check if desired pages are already write protected */
    if((wp_value | (~FMC_PAGES_PROTECTED)) != 0xFFFFFFFF ){
        /* Erase all the option Bytes */
        fmc_state = ob_erase();

        /* Check if there is write protected pages */
        if(protected_pages != 0x0){
            /* Restore write protected pages */
            fmc_state = ob_write_protection_enable(protected_pages);
        }
        /* Generate System Reset to load the new option byte values */
        NVIC_SystemReset();
    }

#elif defined WRITE_PROTECTION_ENABLE
    /* Get current write protected pages and the new pages to be protected */
    protected_pages =  (~wp_value) | FMC_PAGES_PROTECTED; 

    /* Check if desired pages are not yet write protected */
    if(((~wp_value) & FMC_PAGES_PROTECTED )!= FMC_PAGES_PROTECTED){

        /* Erase all the option Bytes because if a program operation is 
        performed on a protected page, the Flash memory returns a 
        protection error */
        fmc_state = ob_erase();

        /* Enable the pages write protection */
        fmc_state = ob_write_protection_enable(protected_pages);

        /* Generate System Reset to load the new option byte values */
        NVIC_SystemReset();
    }
#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_PAGE_PROGRAM  
    /* Get the number of pages to be erased */
    NbrOfPage = (BANK_WRITE_END_ADDR - BANK_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    /* The selected pages are not write protected */
    if ( (wp_value & FMC_PAGES_PROTECTED) != 0x00){
        /* Clear All pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );

        /* erase the FLASH pages */
        for(erase_counter = 0; (erase_counter < NbrOfPage) && (fmc_state == FMC_READY); erase_counter++){
            fmc_state = fmc_page_erase(BANK_WRITE_START_ADDR + (FLASH_PAGE_SIZE * erase_counter));
        }

        /* FLASH Half Word program of data 0x1753 at addresses defined by  BANK1_WRITE_START_ADDR and BANK1_WRITE_END_ADDR */
        Address = BANK_WRITE_START_ADDR;

        while((Address < BANK_WRITE_END_ADDR) && (fmc_state == FMC_READY)){
            fmc_state = fmc_word_program(Address, data);
            Address = Address + 4;
        }

        /* Check the correctness of written data */
        Address = BANK_WRITE_START_ADDR;

        while((Address < BANK_WRITE_END_ADDR) && (program_state != FAILED)){
            if((*(__IO uint16_t*) Address) != data){
                program_state = FAILED;
            }
            Address += 2;
        }
        gd_eval_led_on(LED2);
    }
    else{ 
        /* Error to program the flash : The desired pages are write protected */ 
        program_state = FAILED;
        gd_eval_led_on(LED3);

    }
#endif /* FLASH_PAGE_PROGRAM */
    while(1){
    }
}
