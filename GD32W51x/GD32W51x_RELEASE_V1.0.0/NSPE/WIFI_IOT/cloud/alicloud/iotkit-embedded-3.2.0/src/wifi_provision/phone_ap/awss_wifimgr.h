/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_WIFIMGR_H__
#define __AWSS_WIFIMGR_H__

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#if defined(AWSS_SUPPORT_AHA)

enum {
    SHUB_ERR,
    SHUB_OK
};

int wifimgr_process_switch_ap_request(void *ctx, void *resource, void *remote, void *request);

#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
