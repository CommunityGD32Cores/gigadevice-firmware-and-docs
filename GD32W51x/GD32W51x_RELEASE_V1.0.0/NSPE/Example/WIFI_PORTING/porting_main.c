#include "gd32w51x.h"
#include <stdio.h>
#include "wrapper_os.h"
#include "debug_print.h"
#include "bsp_inc.h"
#include "bsp_wlan.h"
#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "main.h"
#include "console.h"

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

    console_init();

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

    sys_os_init();

    if (NULL == sys_task_create(NULL, (const uint8_t *)"start_task", NULL, START_TASK_STK_SIZE, 0,
            START_TASK_PRIO, start_task, NULL)) {
        DEBUGPRINT("ERROR: create start task failed\r\n");
    }
    sys_os_start();
    while(1);
}
