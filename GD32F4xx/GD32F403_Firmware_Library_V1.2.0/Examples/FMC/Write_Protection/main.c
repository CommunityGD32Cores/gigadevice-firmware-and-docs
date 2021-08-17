/*!
    \file  main.c
    \brief main flash program, write_protection
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h" 
#include "gd32f403_eval.h"
#include <stdio.h>

#define FMC_PAGE_SIZE                ((uint16_t)0x800)
#define FMC_PAGES_PROTECTED          (OB_WP_4)

#define BANK0_WRITE_START_ADDR       ((uint32_t)0x08004000)
#define BANK0_WRITE_END_ADDR         ((uint32_t)0x08004800)

typedef enum {FAILED = 0, PASSED = !FAILED} tset_state_enum;
volatile fmc_state_enum fmc_state = FMC_READY;
volatile tset_state_enum memory_program_state = PASSED;
__IO tset_state_enum transfer_state = FAILED;

uint32_t *ptrd;
uint32_t write_protected_value = 0xFFFFFFFF, protected_pages = 0x0;
uint32_t erase_counter = 0x00, address = 0x00 ,wrp0 = 0,wrp1 = 0;
uint32_t data = 0x1234ABCD;
__IO uint32_t erase_page_number = 0x00;

#define FMC_PAGE_PROGRAM
//#define WRITE_PROTECTION_ENABLE
#define WRITE_PROTECTION_DISABLE

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i;
 
    transfer_state = PASSED;
    
    /* led configuration */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    
    /* turn off led1 and led2,turn on led3 and led4 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED4);

    /* unlock the flash bank0 program erase controller */
    fmc_unlock();
    
    ob_unlock();
    
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    
    /* get status of pages write protection */
    write_protected_value = ob_write_protection_get();
   
#ifdef WRITE_PROTECTION_DISABLE 
  
    /* get write protected pages */
    protected_pages = ~(write_protected_value | FMC_PAGES_PROTECTED);
    
    /* check whether the specified pages is write protected  */
    if((write_protected_value | (~FMC_PAGES_PROTECTED)) != 0xFFFFFFFF){
        /* erase all the option bytes */
        fmc_state = ob_erase();
    
        /* check if other pages write protected */
        if(protected_pages != 0x0){
            /* restore other pages write protected */
            fmc_state = ob_write_protection_enable(protected_pages);
        }
    NVIC_SystemReset();
    }

#elif defined WRITE_PROTECTION_ENABLE
    /* get already write protected pages and the new specified pages to be protected */
    protected_pages =  (~write_protected_value) | FMC_PAGES_PROTECTED; 
    
    /* check whether the specified pages is write protected */
    if(((~write_protected_value) & FMC_PAGES_PROTECTED )!= FMC_PAGES_PROTECTED)
    {
        fmc_state = ob_erase();
        
        /* enable the pages write protection */
        fmc_state = ob_write_protection_enable(protected_pages);
    NVIC_SystemReset();
    }  
#endif 

#ifdef FMC_PAGE_PROGRAM  
    
    /* define the number of page to be erased */
    erase_page_number = (BANK0_WRITE_END_ADDR - BANK0_WRITE_START_ADDR) / FMC_PAGE_SIZE;
    
    /* erase the flash pages */
    for(erase_counter = 0; erase_counter < erase_page_number; erase_counter++)
    {
        fmc_state = fmc_page_erase(BANK0_WRITE_START_ADDR + (FMC_PAGE_SIZE * erase_counter));
        wrp0 += (fmc_state == FMC_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }
    
    fmc_lock();
    
    ptrd =  (uint32_t*)BANK0_WRITE_START_ADDR;
    for(i = 0; i < 512; i++)
    {
        if(*ptrd != 0xFFFFFFFF)
        { 
            transfer_state = FAILED;
            goto fail;
        }
        ptrd++;
    }

    /* unlock the flash Bank0 program erase controller */
    fmc_unlock();
    
    /* define the number of page to be erased */
    erase_page_number = (BANK0_WRITE_END_ADDR - BANK0_WRITE_START_ADDR) / FMC_PAGE_SIZE;
    
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);    
    
    
    /* program flash bank0 */
    address = BANK0_WRITE_START_ADDR;
    wrp1 = 0;
    while(address < BANK0_WRITE_END_ADDR){
        fmc_state = fmc_word_program(address, data);
        address = address + 4; 
        wrp1 += (fmc_state == FMC_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }
    
    fmc_lock();
    
    ptrd =  (uint32_t*)BANK0_WRITE_START_ADDR;
    for(i = 0; i < 512; i++){
        if(*ptrd != data){
            transfer_state = FAILED;
            goto fail;
        }
        ptrd++;
    }
    if(transfer_state != FAILED){
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED3);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED4);
    }else{
fail:
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED4);
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED3);
    }
#endif 
    while(1){
    }
}
