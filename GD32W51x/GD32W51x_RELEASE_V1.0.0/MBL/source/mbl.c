/*!
    \file    mbl.c
    \brief   Main boot loader for GD32W51x WiFi SDK

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

#include "mbl_includes.h"
#include "rom_export_mbedtls.h"
#include "mbl_nsc_api.h"
#include "mbl_dummy.c"

#if defined (__ICCARM__)
extern uint32_t MBL_CMSE_VENEER$$Base;
extern uint32_t MBL_CMSE_VENEER$$Limit;
extern uint32_t MBL_BUF$$Base;
extern uint32_t HEAP$$Limit;
#else
REGION_DECLARE(Image$$, MBL_CMSE_VENEER, $$Base);
REGION_DECLARE(Image$$, MBL_CMSE_VENEER, $$Limit);
REGION_DECLARE(Image$$, MBL_BUF, $$ZI$$Base);
#endif


typedef void (*nsfptr_t) (void) __attribute__((cmse_nonsecure_call));
struct rom_api_s_t *p_rom_api_s = (struct rom_api_s_t *)ROM_API_ARRAY_BASE;
static uint8_t alloc_buf[MBL_BUF_SIZE];

/*!
    \brief      initialize usart
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void log_uart_init(void)
{
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_15);
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_15);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_8);

    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_enable(USART1);

    usart_interrupt_enable(USART1, USART_INT_RBNE);
}

/*!
    \brief      wait usart transmit complete
    \param[in]  none
    \param[out] none
    \retval     none
*/
void log_uart_idle_wait(void)
{
    while (RESET == usart_flag_get(USART1, USART_FLAG_TC));
}

/*!
    \brief      deinitialize mbedtls
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mbedtls_deinit(void)
{
    mbedtls_platform_set_calloc_free(NULL, NULL);
    mbedtls_platform_set_snprintf(NULL);
    mbedtls_platform_set_printf(NULL);
}

#if defined (__ICCARM__)
/*!
    \brief      clear ram data
    \param[in]  none
    \param[out] none
    \retval     none
*/
__attribute__((naked))
void ram_data_clear(void)
{
    __ASM volatile(
        "mov     r0, #0                              \n"
        "ldr     r1, =MBL_BUF$$Base                  \n"
        "ldr     r2, =HEAP$$Limit                    \n"
        "subs    r2, r2, r1                          \n"
        "Loop:                                       \n"
        "subs    r2, #4                              \n"
        "itt     ge                                  \n"
        "strge   r0, [r1, r2]                        \n"
        "bge     Loop                                \n"
        "bx      lr                                  \n"
         : : : "r0" , "r1" , "r2" , "memory"
    );
}
#else
/*!
    \brief      clear ram data
    \param[in]  none
    \param[out] none
    \retval     none
*/
__attribute__((naked))
void ram_data_clear(void)
{
    __ASM volatile(
        "mov     r0, #0                              \n"
        "ldr     r1, =Image$$MBL_BUF$$ZI$$Base       \n"
        "ldr     r2, =Image$$ARM_LIB_STACKHEAP$$ZI$$Limit \n"
        "subs    r2, r2, r1                          \n"
        "Loop:                                       \n"
        "subs    r2, #4                              \n"
        "itt     ge                                  \n"
        "strge   r0, [r1, r2]                        \n"
        "bge     Loop                                \n"
        "bx      lr                                  \n"
         : : : "r0" , "r1" , "r2" , "memory"
    );
}
#endif
//        ".syntax unified                 \n"
/*!
    \brief      jump to PSA root
    \param[in]  reset_handler_addr: reset handler adress
    \param[out] none
    \retval     none
*/
__attribute__((naked))
void jump_to_prot_asm(uint32_t reset_handler_addr)
{
    __ASM volatile(

        "mov     r7, r0                  \n"
        "bl      ram_data_clear          \n" /* Clear ram data before jump to Main Image including the data rom uesed */
        "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
        "mov     r1, r0                  \n" /* except R7 */
        "mov     r2, r0                  \n"
        "mov     r3, r0                  \n"
        "mov     r4, r0                  \n"
        "mov     r5, r0                  \n"
        "mov     r6, r0                  \n"
        "mov     r8, r0                  \n"
        "mov     r9, r0                  \n"
        "mov     r10, r0                 \n"
        "mov     r11, r0                 \n"
        "mov     r12, r0                 \n"
        "mov     lr,  r0                 \n"
        "bx      r7                      \n" /* Jump to Reset_handler */
    );
}

/*!
    \brief      jump to main image
    \param[in]  to_nsec: decide jujmp to non-secure or secure
    \param[in]  msp: main stack pointer
    \param[in]  reset: reset handler
    \param[out] none
    \retval     none
*/
static void jump_to_main_image(IN uint32_t to_nsec, IN uint32_t msp, IN uint32_t reset)
{
    static uint32_t prot_reset;
    static nsfptr_t nspe_reset;

    if (to_nsec) {
        nspe_reset = (nsfptr_t)cmse_nsfptr_create(reset);
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
        __TZ_set_MSPLIM_NS(0);
#endif
        __TZ_set_MSP_NS(msp);
        __DSB();
        __ISB();
        nspe_reset();
    } else {
        prot_reset = reset;
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
        /* Restore the Main Stack Pointer Limit register's reset value
         * before passing execution to runtime firmware to make the
         * bootloader transparent to it.
         */
        __set_MSPLIM(0);
#endif
        __set_MSP(msp);

        __DSB();
        __ISB();

        jump_to_prot_asm(prot_reset);
    }

}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    struct ibl_state_t ibl_state;
    uint32_t boot_idx = 0, image_addr = 0;
    uint8_t boot_seed[BOOT_SEED_SIZE];
    uint8_t impl_id[IMPLEMENTATION_ID_MAX_SIZE];
    uint8_t inst_id[INSTANCE_ID_MAX_SIZE];
    uint32_t arm_vector[2];
    uint32_t bonly_nspe = 0;
    uint32_t nsc_start, nsc_end;
    int ret;

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
#if defined (__ICCARM__)
    uint32_t msp_stack_bottom = (uint32_t)&MBL_BUF$$Base + 0x1000;
#else
    uint32_t msp_stack_bottom = (uint32_t)&REGION_NAME(Image$$, MBL_BUF, $$ZI$$Base) + 0x1000;
#endif
    __set_MSPLIM(msp_stack_bottom);
#endif

    /* Set all flash pages as secure */
    mbl_flash_wm_init();

    /* Reinitialize uart since system clock source changed from IRC16M to HXTAL40M */
    log_uart_init();

    /* Read Initial boot state from shared SRAM */
    memcpy(&ibl_state, (void *)IBL_SHARED_DATA_START, sizeof(struct ibl_state_t));

    /* Re-initialize shared SRAM */
    mbl_status_init();
    mbl_status_sw_info_add(&ibl_state.mbl_info);

    mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));

    /* Find the correct image to boot, Image 0 or Image 1 */
    ret = boot_image_find(&boot_idx, &image_addr);
    if (ret < 0) {
        rom_trace_ex(ROM_ERR, "No image to boot (ret = %d).\r\n", ret);
        goto BootFailed;
    } else {
        rom_trace_ex(ROM_ALWAYS, "MBL: Boot from Image %d.\r\n", boot_idx);
    }

    /* If boot from Image 1, config offset mapping */
    if (boot_idx == IMAGE_1) {
        flash_offset_mapping();
    }

    /* Verify Image */
    ret = image_x_validate(image_addr, ibl_state.mbl_pk, ibl_state.ibl_opt, &bonly_nspe);
    if (ret < 0) {
        rom_trace_ex(ROM_ERR, "Validate Image %d failed (ret = %d).\r\n", boot_idx, ret);
        rom_sys_set_img_flag(boot_idx, IMG_FLAG_VERIFY_MASK, IMG_FLAG_VERIFY_FAIL);
        goto BootFailed;
    } else {
        rom_trace_ex(ROM_ALWAYS, "MBL: Validate Image %d OK.\r\n", boot_idx);
    }

    /* Update Image status and Running Image flag */
    rom_sys_set_img_flag(boot_idx, IMG_FLAG_VERIFY_MASK, IMG_FLAG_VERIFY_OK);
    rom_sys_set_running_img(boot_idx);

    /* Generate boot seed, Instance ID, Implementation ID  */
    boot_seed_gen(boot_seed);
    memcpy(impl_id, ibl_state.impl_id, sizeof(impl_id));
    instance_id_gen(inst_id);

    /* Save General info to MBL status */
    mbl_status_boot_seed_add(boot_seed);
    mbl_status_impl_id_add(impl_id);
    mbl_status_inst_id_add(inst_id);
    mbl_status_running_img_add((uint8_t *)&boot_idx);
#if defined (__ICCARM__)
    nsc_start = (uint32_t)&MBL_CMSE_VENEER$$Base;
    nsc_end = (uint32_t)&MBL_CMSE_VENEER$$Limit;
#else
    nsc_start = (uint32_t)&REGION_NAME(Image$$, MBL_CMSE_VENEER, $$Base);
    nsc_end = (uint32_t)&REGION_NAME(Image$$, MBL_CMSE_VENEER, $$Limit);
#endif
    mbl_status_sau_entry_add((uint8_t *)&nsc_start, (uint8_t *)&nsc_end);
    mbl_status_rom_version_add((uint8_t *)&ibl_state.rom_ver);

    /* Clear the pointers of mbedtls platform APIs */
    if (!bonly_nspe)
        mbedtls_deinit();

    /*####### Test #######*/
    //mbl_status_show();

    /* Jump to PROT or AROT or NSPE */
    rom_flash_read((image_addr + sizeof(struct image_header)), arm_vector, 8);
    if (!rom_is_valid_flash_addr(arm_vector[1])) {
        rom_trace_ex(ROM_ERR, "Image vector reset is invalid (%08x).\r\n", arm_vector[1]);
        goto BootFailed;
    }

    rom_trace_ex(ROM_ALWAYS, "MBL: Jump to Main Image.\r\n");

    /* Setup security related settings for NSPE if no PROT or AROT */
    if (bonly_nspe) {
        if (mbl_sec_for_nspe_setup(boot_idx)< 0) {
            rom_trace_ex(ROM_ERR, "Setup security settings for NSPE failed. \r\n");
            goto BootFailed;
        }
        SCB_NS->VTOR = FLASH_BASE_NS + image_addr + sizeof(struct image_header);
    }
    log_uart_idle_wait();

    jump_to_main_image(bonly_nspe, arm_vector[0], arm_vector[1]);

BootFailed:
    while(1);
}
