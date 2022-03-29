/*!
    \file    main.c
    \brief   main flash pages write protection

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
    \version 2022-01-06, V2.2.0, firmware for GD32F3x0
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "gd32f3x0.h"
#include "gd32f350r_eval.h"
#include "main.h"

/*!
    \brief      enable some pages' write protection function by configuring option byte
    \param[in]  wp_pages_bitmap: bitmap of pages which need to be enabled write protection function
    \param[out] none
    \retval     none
*/
void fmc_ob_write_protection_enable(uint16_t wp_pages_bitmap)
{
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* unlock the main flash and option byte */
    fmc_unlock();
    ob_unlock();

    /* enable the new write protection in option byte */
    ob_write_protection_enable(wp_pages_bitmap);

    /* lock the option byte firstly and then lock the main flash after operation */
    ob_lock();
    fmc_lock();

    /* reload the option byte and generate a system reset */
    ob_reset();
}

/*!
    \brief      disable some pages' write protection function by configuring option byte
    \param[in]  wp_pages_bitmap: bitmap of pages which need to be disabled write protection function
    \param[out] none
    \retval     none
*/
void fmc_ob_write_protection_disable(uint16_t wp_pages_bitmap)
{
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* unlock the main flash and option byte */
    fmc_unlock();
    ob_unlock();

    ob_erase();

    /* enable the new write protection in option byte */
    ob_write_protection_enable(wp_pages_bitmap);

    /* lock the option byte firstly and then lock the main flash after operation */
    ob_lock();
    fmc_lock();

    /* reload the option byte and generate a system reset */
    ob_reset();
}

/*!
    \brief      reset option byte value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_ob_deinitialize(void)
{
    /* clear all pending flags */
    fmc_unlock();
    ob_unlock();
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    ob_erase();
    ob_lock();
    fmc_lock();
    ob_reset();
}

/*!
    \brief      erase and program flash, meanwhile check the operation result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_and_program(void)
{
    uint32_t *ptr = (uint32_t *)ERASE_PAGE_START_ADDR;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* erase target page */
    fmc_page_erase(ERASE_PAGE_START_ADDR);
    /* check the erase result, light the LED3 if the result is failed */
    if(0xFFFFFFFF != (*ptr)) {
        gd_eval_led_on(LED3);

        return;
    }

    /* program target address */
    fmc_word_program(PROGRAM_ADDRESS, PROGRAM_DATA);
    /* check the program result, light the LED3 if the result is failed */
    if(PROGRAM_DATA != (*ptr)) {
        gd_eval_led_on(LED3);

        return;
    }

    /* light the LED4 if the erase and program result are both successful */
    gd_eval_led_on(LED4);

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* lock the main FMC after the operation */
    fmc_lock();
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* configure the keys */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);

    /* check the write protection result and light corresponding LEDs */
    if(WP_ALL_PAGES_BITMAP == ob_write_protection_get()) {
        gd_eval_led_on(LED1);
    } else {
        gd_eval_led_on(LED2);
    }

    /* erase and program flash,
       failure (light LED3) indicates the page is in write protection,
       success (light LED4) indicates the page is not in write protection */
    fmc_erase_and_program();

    while(1);
}

