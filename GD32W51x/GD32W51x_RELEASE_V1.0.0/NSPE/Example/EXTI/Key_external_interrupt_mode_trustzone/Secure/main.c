/*!
    \file    main.c
    \brief   the example of EXTI which generates an interrupt request and toggle the LED, secure code with TZEN = 1

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
#include "systick.h"
#include "gd32w515p_eval.h"
#include "main.h"

#define TZBMPC1_BLOCK_NUMBER    255                          /* TZBMPC1 block number */

#define NONSECURE_START        ((uint32_t)0x08040000U)       /* nonsecure start address */

/* callback function */
ns_fptr nonsecure_func = (ns_fptr)NULL;

void tzbmpc_config();
void nonsecure_config(void);
void nonsecure_init();
CMSE_NS_ENTRY void secure_led_toggle(void);
CMSE_NS_ENTRY void entry_cb_func_register(void *callback);

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

    /* configure systick */
    systick_config();
    /* initilize the LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* turn on LED1 */
    gd_eval_led_on(LED1);
    
    /* configure LED and KEY pin to nonsecure */
    nonsecure_config();
    
    /* setup and jump to non-secure */
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
void tzbmpc_config()
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
    \brief      configure LED and KEY pin to nonsecure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nonsecure_config(void)
{
    /* configure the LED pin to nonsecure */
    gpio_bit_reset_sec_cfg(LED2_GPIO_PORT, LED2_PIN);
    
    /* configure the KEY pin to nonsecure */
    gpio_bit_reset_sec_cfg(TAMPER_WAKEUP_KEY_GPIO_PORT, TAMPER_WAKEUP_KEY_PIN);
}

/*!
    \brief      setup and jump to non-secure
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
    \param[in]  callback: nonsecure function pointer
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
    \brief      toggle the led in secure world
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void secure_led_toggle(void)
{
    gd_eval_led_toggle(LED3);
}
