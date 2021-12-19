/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "target_cfg.h"
#include "tfm_spm_hal.h"
#include "uart_stdout.h"
#ifdef GD_MODIFY_TAG
#include "log/tfm_assert.h"
#include "log/tfm_log.h"
#endif
/* platform-specific hw initialization */
__WEAK enum tfm_plat_err_t tfm_spm_hal_post_init_platform(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_post_init(void)
{
    if (tfm_spm_hal_post_init_platform() != TFM_PLAT_ERR_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    __enable_irq();
    stdio_init();

    return TFM_PLAT_ERR_SUCCESS;
}

__WEAK void tfm_spm_hal_system_reset(void)
{
    NVIC_SystemReset();
}

#ifdef GD_MODIFY_TAG
#if defined(__ARMCC_VERSION)
/* reimplement the function to reach Error Handler */
void __aeabi_assert(const char *expr, const char *file, int line)
{

  LOG_MSG("assertion \" %s \" failed: file %s %d\n", expr, file, line);

  while(1);
}
#endif  /*  __ARMCC_VERSION */
#endif
