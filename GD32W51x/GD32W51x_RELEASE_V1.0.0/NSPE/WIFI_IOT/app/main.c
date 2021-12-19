/*!
    \file    main.c
    \brief   Main function for GD32W51x WiFi SDK

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

/*============================ INCLUDES ======================================*/
#include "wrapper_os.h"
#include "debug_print.h"
#include "bsp_inc.h"
#include "bsp_wlan.h"
#include "tcpip.h"
#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "console.h"
#include "main.h"
#include "wifi_version.h"
#ifdef CONFIG_FATFS_SUPPORT
#include "fatfs.h"
#endif
/*============================ MACROS ========================================*/
/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#if defined (__ICCARM__)
#pragma section = "RAM_CODE_init"
#pragma section = "RAM_CODE"
#pragma section = "RAM_CODE_HT_init"
#pragma section = "RAM_CODE_HT"
void do_ram_code_copy(void)
{
    uint8_t *from, *to;
    from = __section_begin("RAM_CODE_init");
    to = __section_begin("RAM_CODE");
    memcpy(to, from, __section_size("RAM_CODE_init"));
#ifdef CONFIG_WIFI_HIGH_PERFORMANCE
    from = __section_begin("RAM_CODE_HT_init");
    to = __section_begin("RAM_CODE_HT");
    sys_memcpy(to, from, __section_size("RAM_CODE_HT_init"));
#endif
}
#endif

/*!
    \brief      initialize application
    \param[in]  none
    \param[out] none
    \retval     none
*/
__WEAK void app_init(void)
{
    /* User application entry */
}

/*!
    \brief      start task
    \param[in]  p_arg: the pointer of parameters
    \param[out] none
    \retval     none
*/
void start_task(void *p_arg)
{
    (void)p_arg;

    sys_reset_flag_check();
    sys_os_misc_init();
    systick_delay_init();

    wifi_management_init();
#ifdef CONFIG_FATFS_SUPPORT
    rcu_periph_clock_enable(RCU_CRC);
    fatfs_mk_mount();
#endif
#ifdef CONFIG_CONSOLE_ENABLE
    console_init();
#else
    app_init();
#endif

    sys_task_delete(NULL);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    platform_init();

    DEBUGPRINT("SDK version: "WIFI_RELEASE_VERSION" \r\n");
    DEBUGPRINT("SDK build revision: "WIFI_BUILD_REVISION" \r\n");
    DEBUGPRINT("SDK build date: "BUILD_DATE" \r\n");

    sys_os_init();

    if (NULL == sys_task_create(NULL, (const uint8_t *)"start_task", NULL, START_TASK_STK_SIZE, 0,
            START_TASK_PRIO, start_task, NULL)) {
        DEBUGPRINT("ERROR: create start task failed\r\n");
    }

    sys_os_start();
}
