/*!
    \file    main.c
    \brief   Trustzone demo, secure code with TZEN = 1

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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

#include "gd32w51x.h"
#include "gd32w515p_eval.h"
#include "main.h"
#include "stdio.h"
#include "systick.h"

/* callback function */
ns_fptr nonsecure_func = (ns_fptr)NULL;

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

    /* configure systick */
    systick_config();

    /* configure mark secure pages */
    if(SECM_SPAGE != (FMC_SECMCFG0 & 0x3FF) || SECM_EPAGE != ((FMC_SECMCFG0 >> 16) & 0x3FF)){
        fmc_secmark_config();
    }

    if(0U == (FMC_OBR & FMC_OBR_TZEN)){
        /* enable trustzone */
        fmc_trustzone_enable();
    }

    /* configure TZBMPC */
    tzbmpc_config();

    led_config();
    com_config();

    /* setup and jump to non-secure */
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
    ob_trustzone_enable();
    ob_start();
    __NVIC_SystemReset();
    ob_lock();
    fmc_lock();
}

/*!
    \brief      configure mark secure pages
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_secmark_config(void)
{
    fmc_unlock();
    ob_unlock();
    /* configure mark secure pages */
    ob_secmark_config(SECM_SPAGE, SECM_EPAGE, SECM_INDEX0);
    ob_start();
    while(0U != (FMC_SECSTAT & (FMC_SECSTAT_SECBUSY))){
    }
    ob_reload();
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
    uint16_t block_number = 0U;
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);

    /* SRAM1 is used to nonsecure code, so all blocks of SRAM1 should set to nonsecure */
    for(block_number = 0U; block_number <= TZBMPC1_BLOCK_NUMBER; block_number++){
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, block_number, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
}

/*!
    \brief      configure LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    /* enable LEDs clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure the LEDs pin to non-secure */ 
    gpio_bit_reset_sec_cfg(LED1_GPIO_PORT, LED1_PIN);
    gpio_bit_reset_sec_cfg(LED2_GPIO_PORT, LED2_PIN);
}

/*!
    \brief      configure COM
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_config(void)
{
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);
    /* configure USART2 to secure,  non-secure aceess USART2 is illegal */
    tzpcu_tzspc_peripheral_attributes_config(TZPCU_PERIPH_USART2, TZPCU_SEC);

    /* configure COM0 port */
    gd_eval_com_init(EVAL_COM0);
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
    \brief      register nonscure callback function
    \param[in]  callback : nonsecure function pointer
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void entry_cb_func_register(void *callback)
{
    if(callback != NULL){
    nonsecure_func = (ns_fptr)cmse_nsfptr_create(callback);
    }
}

/*!
    \brief      non-secure code print function
    \param[in]  str : print string
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void non_secure_print(const char * str)
{
    printf("non-secure code print: %s", str);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}
