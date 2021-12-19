/*
 *  SSL client demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#include "ssl_tz.h"
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t
#define mbedtls_fprintf    fprintf
#define mbedtls_printf     printf
#endif
#undef mbedtls_printf
#define mbedtls_printf     printf
#include "wrapper_os.h"
#ifdef CONFIG_SSL_TEST

//#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_CERTS_C) ||
#if !defined(MBEDTLS_BIGNUM_C) || \
    !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) || \
    !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||         \
    !defined(MBEDTLS_PEM_PARSE_C) || \
    !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)
int ssl_self_test( void )
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_ENTROPY_C and/or "
           "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_CLI_C and/or "
           "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
           "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
           "not defined.\r\n");
    return( 0 );
}
#else

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/dhm.h"
#include "mbedtls/gcm.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cmac.h"
#include "mbedtls/md2.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/arc4.h"
#include "mbedtls/des.h"
#include "mbedtls/aes.h"
#include "mbedtls/camellia.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/rsa.h"
#include "mbedtls/x509.h"
#include "mbedtls/xtea.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecjpake.h"
#include "mbedtls/timing.h"

void ssl_self_test(void *arg)
{
#if defined(MBEDTLS_SELF_TEST)

    int v, suites_tested = 0, suites_failed = 0;
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && defined(MBEDTLS_SELF_TEST)
    unsigned char buf[1000000];
#endif
    void *pointer;

    /*
     * The C standard doesn't guarantee that all-bits-0 is the representation
     * of a NULL pointer. We do however use that in our code for initializing
     * structures, which should work on every modern platform. Let's be sure.
     */
    memset( &pointer, 0, sizeof( void * ) );
    if( pointer != NULL )
    {
        mbedtls_printf( "all-bits-zero is not a NULL pointer\r\n" );
        //mbedtls_exit( MBEDTLS_EXIT_FAILURE );
    }

    {
        v = 1;
        mbedtls_printf( "\r\n" );
    }


#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    mbedtls_memory_buffer_alloc_init( buf, sizeof(buf) );
#endif

#if defined(MBEDTLS_MD2_C)
    if( mbedtls_md2_self_test( v )  != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_MD4_C)
    if( mbedtls_md4_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_MD5_C)
    if( mbedtls_md5_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_RIPEMD160_C)
    if( mbedtls_ripemd160_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_SHA1_C)
    if( mbedtls_sha1_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_SHA256_C)
    if( mbedtls_sha256_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_SHA512_C)
    if( mbedtls_sha512_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_ARC4_C)
    if( mbedtls_arc4_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_DES_C)
    if( mbedtls_des_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_AES_C)
    if( mbedtls_aes_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_GCM_C) && defined(MBEDTLS_AES_C)
    if( mbedtls_gcm_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_AES_C)
    if( mbedtls_ccm_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_CMAC_C)
    if( ( mbedtls_cmac_self_test( v ) ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_BASE64_C)
    if( mbedtls_base64_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_BIGNUM_C)
    if( mbedtls_mpi_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
    mbedtls_mpi_exp_mod_self_test_512(1);
    mbedtls_mpi_exp_mod_self_test_1024(1);
#endif

#if defined(MBEDTLS_RSA_C)
    if( mbedtls_rsa_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if 0//defined(MBEDTLS_X509_USE_C)
    if( mbedtls_x509_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_XTEA_C)
    if( mbedtls_xtea_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_CAMELLIA_C)
    if( mbedtls_camellia_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_CTR_DRBG_C)
    if( mbedtls_ctr_drbg_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_HMAC_DRBG_C)
    if( mbedtls_hmac_drbg_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif


#if 1
#if defined(MBEDTLS_ECP_C)
    if( mbedtls_ecp_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif
#endif

#if defined(MBEDTLS_ECJPAKE_C)
//    if( mbedtls_ecjpake_self_test( v ) != 0 )
//    {
//        suites_failed++;
//    }
//    suites_tested++;
#endif

#if defined(MBEDTLS_DHM_C)
    if( mbedtls_dhm_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_ENTROPY_C)

#if defined(MBEDTLS_ENTROPY_NV_SEED) && !defined(MBEDTLS_NO_PLATFORM_ENTROPY)
    create_entropy_seed_file();
#endif

    if( mbedtls_entropy_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#if defined(MBEDTLS_PKCS5_C)
    if( mbedtls_pkcs5_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

/* Slow tests last */

#if defined(MBEDTLS_TIMING_C)
/*
    if( mbedtls_timing_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
*/
#endif

    if( v != 0 )
    {
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && defined(MBEDTLS_MEMORY_DEBUG)
        mbedtls_memory_buffer_alloc_status();
#endif
    }

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    mbedtls_memory_buffer_alloc_free();
    if( mbedtls_memory_buffer_alloc_self_test( v ) != 0 )
    {
        suites_failed++;
    }
    suites_tested++;
#endif

#else
    mbedtls_printf( " MBEDTLS_SELF_TEST not defined.\r\n" );
#endif

    if( v != 0 )
    {
        mbedtls_printf( "  Executed %d test suites\r\n", suites_tested );

        if( suites_failed > 0)
        {
            mbedtls_printf( "  [ %d tests FAIL ]\r\n", suites_failed );
        }
        else
        {
            mbedtls_printf( "  [ All tests PASS ]\r\n" );
        }
#if defined(_WIN32)
        mbedtls_printf( "  Press Enter to exit this program.\r\n" );
        fflush( stdout ); getchar();
#endif
    }

    //if( suites_failed > 0)
    //    mbedtls_exit( MBEDTLS_EXIT_FAILURE );

#endif//MBEDTLS_SELF_TEST

	sys_task_delete(NULL);
}

#endif

void cmd_ssl_selftest(int argc, char **argv)
{
	if (NULL == sys_task_create(NULL, (const uint8_t *)"ssl_selftest", NULL,
					3072, 0, SSL_CLIENT_TASK_PRIO,
					(task_func_t)ssl_self_test, NULL)) {
		mbedtls_printf("ERROR: Create ssl selftest task failed\r\n");
	}
}
#endif //CONFIG_SSL_TEST
