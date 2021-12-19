/*!
    \file    ns_interface.c
    \brief   MbedTLS NS interface for GD32W51x WiFi SDK

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

#include "wrapper_os.h"
#include "ssl_tz.h"

#if defined(CONFIG_TZ_ENABLED)
#include "ns_interface.h"

/**
 * \brief the ns_lock ID
 */
static os_mutex_t ns_lock_handle = NULL;

/*
* Implement tfm_ns_interface_dispatch for call tfm crypto service
*/
// __attribute__((weak))
int32_t
tfm_ns_interface_dispatch(veneer_int_fn fn,
						  uint32_t arg0, uint32_t arg1,
						  uint32_t arg2, uint32_t arg3)
{
	int32_t result;

	sys_mutex_get(&ns_lock_handle);
	result = fn(arg0, arg1, arg2, arg3);
	sys_mutex_put(&ns_lock_handle);

	return result;
}


// __attribute__((weak))
int32_t
mbedtls_ns_interface_dispatch(veneer_int_fn fn,
						  uint32_t arg0, uint32_t arg1,
						  uint32_t arg2, uint32_t arg3)
{
	int32_t result;

	sys_mutex_get(&ns_lock_handle);
	result = fn(arg0, arg1, arg2, arg3);
	sys_mutex_put(&ns_lock_handle);

	return result;
}

// __attribute__((weak))
void
mbedtls_ns_interface_void_dispatch(veneer_void_fn fn,
						  uint32_t arg0, uint32_t arg1,
						  uint32_t arg2, uint32_t arg3)
{
	sys_mutex_get(&ns_lock_handle);
	fn(arg0, arg1, arg2, arg3);
	sys_mutex_put(&ns_lock_handle);
}

// __attribute__((weak))
int ns_interface_init(void)
{
	sys_mutex_init(&ns_lock_handle);
	return 0;
}
#endif
