
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "app_cfg.h"
#include "wrapper_os.h"
#include "debug_print.h"
#include "alicloud_entry.h"
#include "ali_linkkit_sdk_include.h"


#ifdef CONFIG_ALICLOUD_SUPPORT
static void alicloud_task(void *argv)
{
    app_main_paras_t *paras = (app_main_paras_t *)argv;
    linkkit_main(paras->argc, paras->argv);
    DEBUGPRINT("alicloud task exit.\r\n");
    sys_task_delete(NULL);
}

void cmd_alicloud_linkkit(int argc, char **argv)
{
    void *handle;
    app_main_paras_t paras;

    if (argc != 2) {
        DEBUGPRINT("Usage: ali_cloud <mode>\r\n");
        DEBUGPRINT("<mode>: 1 - smart config, 2 - softap config\r\n");
        return;
    }

    paras.argc = argc;
    paras.argv = argv;

    handle = sys_task_create(NULL, (const uint8_t *)"alicloud_task", NULL,
                    ALICLOUD_STACK_SIZE, 0, ALICLOUD_TASK_PRIO,
                    (task_func_t)alicloud_task, (void *)&paras);
    if (handle == NULL) {
        DEBUGPRINT("ERROR: create alicloud task failed.\r\n");
        return;
    }

    sys_ms_sleep(1000);
    awss_config_press();

    return;
}
#endif
