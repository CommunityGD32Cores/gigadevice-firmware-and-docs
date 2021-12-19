/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __NS_INTERFACE_H__
#define __NS_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


typedef void (*veneer_void_fn) (uint32_t arg0, uint32_t arg1,
                              uint32_t arg2, uint32_t arg3);
typedef int32_t (*veneer_int_fn) (uint32_t arg0, uint32_t arg1,
                              uint32_t arg2, uint32_t arg3);


void
mbedtls_ns_interface_void_dispatch(veneer_void_fn fn,
						  uint32_t arg0, uint32_t arg1,
						  uint32_t arg2, uint32_t arg3);
int32_t
mbedtls_ns_interface_dispatch(veneer_int_fn fn,
						  uint32_t arg0, uint32_t arg1,
						  uint32_t arg2, uint32_t arg3);

int ns_interface_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_NS_INTERFACE_H__ */
