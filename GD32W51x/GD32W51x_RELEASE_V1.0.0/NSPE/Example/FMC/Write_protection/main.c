/*!
    \file    main.c
    \brief   main flash program, write protection

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

#define FLASH_PAGE_PROGRAM
//#define WRITE_PROTECTION_ENABLE
#define WRITE_PROTECTION_DISABLE

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;

#define FLASH_PAGE_SIZE                         ((uint16_t)0x1000U)           /*!< size of page */
#define WRP_REGION_SPAGE                        ((uint32_t)0x00000080U)       /*!< start page of write protection region: page128 */
#define WRP_REGION_EPAGE                        ((uint32_t)0x00000082U)       /*!< end page of write protection region: page130 */
#define BANK_WRITE_START_ADDR                   ((uint32_t)0x08080000U)       /*!< user write protection start page address */
#define BANK_WRITE_END_ADDR                     ((uint32_t)0x08082000U)       /*!< user write protection end page address */
#define WRP_REGION_SPAGE_DEFAULT_VALUE          ((uint32_t)0x000003FFU)       /*!< start page of write protection region default value: page1024 */
#define WRP_REGION_EPAGE_DEFAULT_VALUE          ((uint32_t)0x00000000U)       /*!< end page of write protection region default value: page0 */
#define OBWRP_VALUE                             ((uint32_t)0x00820080U)       /*!< the value of OBWRP */
#define OBWRP_RESET_VALUE                       ((uint32_t)0x000003FFU)       /*!< the reset value of OBWRP */

uint32_t erase_counter = 0x0, address = 0x0;
uint32_t data = 0x12345678;
uint32_t num_of_page;
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
    uint32_t obwrp0, obwrp1;
    
    obwrp0 = FMC_OBWRP0;
    obwrp1 = FMC_OBWRP1;

    /* initialize leds */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    
    /* unlock the flash program/erase controller */
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);

#ifdef WRITE_PROTECTION_DISABLE
    if((OBWRP_VALUE == obwrp0) || (OBWRP_RESET_VALUE != obwrp1)){
        ob_unlock();
        /* configure write protection pages */
        ob_write_protection_config(WRP_REGION_SPAGE_DEFAULT_VALUE, WRP_REGION_EPAGE_DEFAULT_VALUE, OBWRP_INDEX0);
        ob_write_protection_config(WRP_REGION_SPAGE_DEFAULT_VALUE, WRP_REGION_EPAGE_DEFAULT_VALUE, OBWRP_INDEX1);
        /* start option bytes modification */
        ob_start();
        ob_reload();
    }
#elif defined WRITE_PROTECTION_ENABLE
    if((OBWRP_VALUE != obwrp0) || (OBWRP_RESET_VALUE != obwrp1)){
        ob_unlock();
        /* configure write protection pages */
        ob_write_protection_config(WRP_REGION_SPAGE, WRP_REGION_EPAGE, OBWRP_INDEX0);
        ob_write_protection_config(WRP_REGION_SPAGE_DEFAULT_VALUE, WRP_REGION_EPAGE_DEFAULT_VALUE, OBWRP_INDEX1);
        /* start option bytes modification */
        ob_start();
        ob_reload();
    }
#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_PAGE_PROGRAM  
    /* get the number of pages to be erased */
    num_of_page = (BANK_WRITE_END_ADDR - BANK_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    /* the selected pages are not write protected */
    if ((OBWRP_RESET_VALUE == obwrp0) && (OBWRP_RESET_VALUE == obwrp1)){
        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_OBERR);

        /* erase the flash pages */
        for(erase_counter = 0; (erase_counter < num_of_page) && (fmc_state == FMC_READY); erase_counter++){
            fmc_state = fmc_page_erase(BANK_WRITE_START_ADDR + (FLASH_PAGE_SIZE * erase_counter));
        }
  
        /* flash word program of data 0x12345678 at addresses defined by BANK_WRITE_START_ADDR and BANK_WRITE_END_ADDR */
        address = BANK_WRITE_START_ADDR;

        while((address < BANK_WRITE_END_ADDR) && (fmc_state == FMC_READY)){
            fmc_state = fmc_word_program(address, data);
            address = address + 4;
        }

        /* check the correctness of written data */
        address = BANK_WRITE_START_ADDR;

        while((address < BANK_WRITE_END_ADDR) && (program_state != FAILED)){
            if((*(__IO uint32_t*) address) != data){
                program_state = FAILED;
            }
            address += 4;
        }
        gd_eval_led_on(LED2);
    }
    else{ 
        /* error to program the flash : the desired pages are write protected */ 
        program_state = FAILED;
        gd_eval_led_on(LED3);

    }
#endif /* FLASH_PAGE_PROGRAM */
    while(1){
    }
}
