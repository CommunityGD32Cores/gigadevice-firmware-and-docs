/*!
    \file    mbedtls_ns_function_api.c
    \brief   MbedTLS NS function API for GD32W51x WiFi SDK

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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif


#include "ssl_tz.h"

#if defined(SSL_USE_MBEDTLS_NSC)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free      free
#endif
#if defined(MBEDTLS_CIPHER_C)
#include "mbedtls/cipher.h"
#endif

#if defined(MBEDTLS_DHM_C)
#include "mbedtls/dhm.h"
#endif

#if defined(MBEDTLS_ECDH_C)
#include "mbedtls/ecdh.h"
#endif

#if defined(MBEDTLS_ECP_C)
#include "mbedtls/ecp.h"
#endif

#if defined(MBEDTLS_PK_C)
#include "mbedtls/pk.h"
#endif

#if defined(MBEDTLS_BIGNUM_C)
#include "mbedtls/bignum.h"
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
#include "mbedtls/x509_crt.h"
#endif

#if defined(MBEDTLS_MD_C)
#include "mbedtls/md_internal.h"
#include "mbedtls/md.h"
#endif

#if defined(MBEDTLS_AES_C)
#include "mbedtls/aes.h"
#endif

#include "mbedtls_nsc.h"
#include "ns_interface.h"




#define MBEDTLS_SFN_ID 1

#define API_INT_DISPATCH(nsc_name, sfn_id)                               \
        mbedtls_ns_interface_dispatch((veneer_int_fn)mbedtls_##nsc_name##_nsc, \
        (uint32_t)in_vec, ARRAY_SIZE(in_vec),                        \
        (uint32_t)out_vec, ARRAY_SIZE(out_vec))

#define API_INT_DISPATCH_NO_OUTVEC(nsc_name, sfn_id)                     \
        mbedtls_ns_interface_dispatch((veneer_int_fn)mbedtls_##nsc_name##_nsc, \
        (uint32_t)in_vec, ARRAY_SIZE(in_vec),                        \
        (uint32_t)NULL, 0)

#define API_VOID_DISPATCH(nsc_name, sfn_id)                               \
        mbedtls_ns_interface_void_dispatch((veneer_void_fn)mbedtls_##nsc_name##_nsc, \
        (uint32_t)in_vec, ARRAY_SIZE(in_vec),                        \
        (uint32_t)out_vec, ARRAY_SIZE(out_vec))

#define API_VOID_DISPATCH_NO_OUTVEC(nsc_name, sfn_id)                     \
        mbedtls_ns_interface_void_dispatch((veneer_void_fn)mbedtls_##nsc_name##_nsc, \
        (uint32_t)in_vec, ARRAY_SIZE(in_vec),                        \
        (uint32_t)NULL, 0)


#if defined(MBEDTLS_CIPHER_C)

#define CIPHER_VALIDATE_RET(cond) \
	MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA)
#define CIPHER_VALIDATE(cond) \
	MBEDTLS_INTERNAL_VALIDATE(cond)

/*
 * Packet-oriented decryption for AEAD modes
 */
int mbedtls_cipher_auth_decrypt(mbedtls_cipher_context_t *ctx,
								const unsigned char *iv, size_t iv_len,
								const unsigned char *ad, size_t ad_len,
								const unsigned char *input, size_t ilen,
								unsigned char *output, size_t *olen,
								const unsigned char *tag, size_t tag_len)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		{.base = iv, .len = iv_len},
		{.base = ad, .len = ad_len},
		{.base = input, .len = ilen},
		{.base = tag, .len = tag_len},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
	};

	result =  API_INT_DISPATCH(mbedtls_cipher_auth_decrypt, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}
#if defined(MBEDTLS_CIPHER_MODE_AEAD)
/*
 * Packet-oriented encryption for AEAD modes
 */
int mbedtls_cipher_auth_encrypt(mbedtls_cipher_context_t *ctx,
								const unsigned char *iv, size_t iv_len,
								const unsigned char *ad, size_t ad_len,
								const unsigned char *input, size_t ilen,
								unsigned char *output, size_t *olen,
								unsigned char *tag, size_t tag_len)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		{.base = iv, .len = iv_len},
		{.base = ad, .len = ad_len},
		{.base = input, .len = ilen},
		{.base = tag, .len = tag_len},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
	};

	result =  API_INT_DISPATCH(mbedtls_cipher_auth_encrypt, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

#endif

/*
 * Packet-oriented wrapper for non-AEAD modes
 */
int mbedtls_cipher_crypt(mbedtls_cipher_context_t *ctx,
						 const unsigned char *iv, size_t iv_len,
						 const unsigned char *input, size_t ilen,
						 unsigned char *output, size_t *olen)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		{.base = iv, .len = iv_len},
		{.base = input, .len = ilen},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
	};

	result =  API_INT_DISPATCH(mbedtls_cipher_crypt, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

void mbedtls_cipher_free(mbedtls_cipher_context_t *ctx)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		};
	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_cipher_free, MBEDTLS_SFN_ID);
}

const mbedtls_cipher_info_t *mbedtls_cipher_info_from_type(
	const mbedtls_cipher_type_t cipher_type)
{
	mbedtls_cipher_info_t *cipher_info = mbedtls_calloc(1, sizeof(mbedtls_cipher_info_t));
	nsc_invec in_vec[] = {
		{.base = &cipher_type, .len = sizeof(mbedtls_cipher_type_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = cipher_info, .len = sizeof(mbedtls_cipher_info_t)},
	};
	API_VOID_DISPATCH(mbedtls_cipher_info_from_type, MBEDTLS_SFN_ID);

	if (out_vec[0].len == 0) {
		mbedtls_free(cipher_info);
		cipher_info =  NULL;
	}

	return cipher_info;
}

void mbedtls_cipher_init(mbedtls_cipher_context_t *ctx)
{
	CIPHER_VALIDATE(ctx != NULL);
	memset(ctx, 0, sizeof(mbedtls_cipher_context_t));
}

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
int mbedtls_cipher_set_padding_mode(mbedtls_cipher_context_t *ctx,
									mbedtls_cipher_padding_t mode)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		{.base = &mode, .len = sizeof(mbedtls_cipher_padding_t)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_cipher_set_padding_mode, MBEDTLS_SFN_ID);
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

int mbedtls_cipher_setkey(mbedtls_cipher_context_t *ctx,
						  const unsigned char *key,
						  int key_bitlen,
						  const mbedtls_operation_t operation)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		{.base = key, .len = key_bitlen},
		{.base = &operation, .len = sizeof(mbedtls_operation_t)},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_cipher_setkey, MBEDTLS_SFN_ID);
}

int mbedtls_cipher_setup(mbedtls_cipher_context_t *ctx,
						 const mbedtls_cipher_info_t *cipher_info)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_cipher_context_t)},
		{.base = cipher_info, .len = sizeof(mbedtls_cipher_info_t)},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_cipher_setup, MBEDTLS_SFN_ID);
}

#endif /* MBEDTLS_CIPHER_C */

#if defined(MBEDTLS_DHM_C)

#define DHM_VALIDATE_RET(cond) \
	MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_DHM_BAD_INPUT_DATA)
#define DHM_VALIDATE(cond) \
	MBEDTLS_INTERNAL_VALIDATE(cond)

int mbedtls_dhm_calc_secret(mbedtls_dhm_context *ctx,
							unsigned char *output, size_t output_size, size_t *olen,
							int (*f_rng)(void *, unsigned char *, size_t),
							void *p_rng)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = output_size},
	};

	result =  API_INT_DISPATCH(mbedtls_dhm_calc_secret, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

/*
 * Free the components of a DHM key
 */
void mbedtls_dhm_free(mbedtls_dhm_context *ctx)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
	};

	if (ctx == NULL)
		return;

	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_dhm_free, MBEDTLS_SFN_ID);
}
void mbedtls_dhm_init(mbedtls_dhm_context *ctx)
{
	DHM_VALIDATE(ctx != NULL);
	memset(ctx, 0, sizeof(mbedtls_dhm_context));
}

/*
 * Create own private value X and export G^X
 */
int mbedtls_dhm_make_public(mbedtls_dhm_context *ctx, int x_size,
							unsigned char *output, size_t olen,
							int (*f_rng)(void *, unsigned char *, size_t),
							void *p_rng)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
		{.base = (void *)f_rng, .len = x_size},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = olen},
	};

	return API_INT_DISPATCH(mbedtls_dhm_make_public, MBEDTLS_SFN_ID);
}

/*
 * Parse the ServerKeyExchange parameters
 */
int mbedtls_dhm_read_params(mbedtls_dhm_context *ctx,
							unsigned char **p,
							const unsigned char *end)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
		{.base = p, .len = 0},
		{.base = end, .len = 0},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_dhm_read_params, MBEDTLS_SFN_ID);
}
/*
 * Setup and write the ServerKeyExchange parameters
 */
int mbedtls_dhm_make_params(mbedtls_dhm_context *ctx, int x_size,
							unsigned char *output, size_t *olen,
							int (*f_rng)(void *, unsigned char *, size_t),
							void *p_rng)
{
	int result;
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
		{.base = (void *)f_rng, .len = x_size},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
	};

	result = API_INT_DISPATCH(mbedtls_dhm_make_params, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;
	return result;
}

/*
 * Import the peer's public value G^Y
 */
int mbedtls_dhm_read_public(mbedtls_dhm_context *ctx,
							const unsigned char *input, size_t ilen)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
		{.base = input, .len = ilen},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_dhm_read_public, MBEDTLS_SFN_ID);
}

/*
 * Set prime modulus and generator
 */
int mbedtls_dhm_set_group(mbedtls_dhm_context *ctx,
						  const mbedtls_mpi *P,
						  const mbedtls_mpi *G)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_dhm_context)},
		{.base = P, .len = sizeof(mbedtls_mpi)},
		{.base = G, .len = sizeof(mbedtls_mpi)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_dhm_set_group, MBEDTLS_SFN_ID);
}

#endif /* MBEDTLS_DHM_C */

#if defined(MBEDTLS_ECDH_C)
/*
 * Derive and export the shared secret
 */
int mbedtls_ecdh_calc_secret(mbedtls_ecdh_context *ctx, size_t *olen,
							 unsigned char *buf, size_t blen,
							 int (*f_rng)(void *, unsigned char *, size_t),
							 void *p_rng)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = buf, .len = blen},
	};

	result =  API_INT_DISPATCH(mbedtls_ecdh_calc_secret, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

/*
 * Get parameters from a keypair
 */
int mbedtls_ecdh_get_params(mbedtls_ecdh_context *ctx,
							const mbedtls_ecp_keypair *key,
							mbedtls_ecdh_side side)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = key, .len = sizeof(mbedtls_ecp_keypair)},
		{.base = &side, .len = sizeof(mbedtls_ecdh_side)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_ecdh_get_params, MBEDTLS_SFN_ID);
}

/*
 * Setup and export the client public value
 */
int mbedtls_ecdh_make_public(mbedtls_ecdh_context *ctx, size_t *olen,
							 unsigned char *buf, size_t blen,
							 int (*f_rng)(void *, unsigned char *, size_t),
							 void *p_rng)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = buf, .len = blen},
	};

	result =  API_INT_DISPATCH(mbedtls_ecdh_make_public, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

/*
 * Read the ServerKeyExhange parameters (RFC 4492)
 *      struct {
 *          ECParameters    curve_params;
 *          ECPoint         public;
 *      } ServerECDHParams;
 */
int mbedtls_ecdh_read_params(mbedtls_ecdh_context *ctx,
							 const unsigned char **buf,
							 const unsigned char *end)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = buf, .len = 0},
		{.base = end, .len = 0},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_ecdh_read_params, MBEDTLS_SFN_ID);
}

/*
 * Setup and write the ServerKeyExhange parameters (RFC 4492)
 *      struct {
 *          ECParameters    curve_params;
 *          ECPoint         public;
 *      } ServerECDHParams;
 */
int mbedtls_ecdh_make_params(mbedtls_ecdh_context *ctx, size_t *olen,
							 unsigned char *buf, size_t blen,
							 int (*f_rng)(void *, unsigned char *, size_t),
							 void *p_rng)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = buf, .len = blen},
	};

	result =  API_INT_DISPATCH(mbedtls_ecdh_make_params, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

/*
 * Parse and import the client's public value
 */
int mbedtls_ecdh_read_public(mbedtls_ecdh_context *ctx,
							 const unsigned char *buf, size_t blen)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = buf, .len = blen},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_ecdh_read_public, MBEDTLS_SFN_ID);
}

/*
 * Setup context
 */
int mbedtls_ecdh_setup(mbedtls_ecdh_context *ctx, mbedtls_ecp_group_id grp_id)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
		{.base = &grp_id, .len = sizeof(mbedtls_ecp_group_id)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_ecdh_setup, MBEDTLS_SFN_ID);
}
/*
 * Free context
 */
void mbedtls_ecdh_free(mbedtls_ecdh_context *ctx)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
	};

	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_ecdh_free, MBEDTLS_SFN_ID);
}
/*
 * Initialize context
 */
void mbedtls_ecdh_init(mbedtls_ecdh_context *ctx)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_ecdh_context)},
	};

	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_ecdh_init, MBEDTLS_SFN_ID);
}

#endif /* MBEDTLS_ECDH_C */

#if defined(MBEDTLS_ECP_C)
/*
* The following code just copy from ecp.c
*
*/
/*
 * List of supported curves:
 *  - internal ID
 *  - TLS NamedCurve ID (RFC 4492 sec. 5.1.1, RFC 7071 sec. 2)
 *  - size in bits
 *  - readable name
 *
 * Curves are listed in order: largest curves first, and for a given size,
 * fastest curves first. This provides the default order for the SSL module.
 *
 * Reminder: update profiles in x509_crt.c when adding a new curves!
 */
const mbedtls_ecp_curve_info ecp_supported_curves[] =
	{
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
		{MBEDTLS_ECP_DP_SECP521R1, 25, 521, "secp521r1"},
#endif
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
		{MBEDTLS_ECP_DP_BP512R1, 28, 512, "brainpoolP512r1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
		{MBEDTLS_ECP_DP_SECP384R1, 24, 384, "secp384r1"},
#endif
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
		{MBEDTLS_ECP_DP_BP384R1, 27, 384, "brainpoolP384r1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
		{MBEDTLS_ECP_DP_SECP256R1, 23, 256, "secp256r1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
		{MBEDTLS_ECP_DP_SECP256K1, 22, 256, "secp256k1"},
#endif
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
		{MBEDTLS_ECP_DP_BP256R1, 26, 256, "brainpoolP256r1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
		{MBEDTLS_ECP_DP_SECP224R1, 21, 224, "secp224r1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
		{MBEDTLS_ECP_DP_SECP224K1, 20, 224, "secp224k1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
		{MBEDTLS_ECP_DP_SECP192R1, 19, 192, "secp192r1"},
#endif
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
		{MBEDTLS_ECP_DP_SECP192K1, 18, 192, "secp192k1"},
#endif
		{MBEDTLS_ECP_DP_NONE, 0, 0, NULL},
};

#define ECP_NB_CURVES sizeof(ecp_supported_curves) / \
						  sizeof(ecp_supported_curves[0])

static mbedtls_ecp_group_id ecp_supported_grp_id[ECP_NB_CURVES];

/*
 * List of supported curves and associated info
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_list(void)
{
	return (ecp_supported_curves);
}

/*
 * List of supported curves, group ID only
 */
const mbedtls_ecp_group_id *mbedtls_ecp_grp_id_list(void)
{
	static int init_done = 0;

	if (!init_done)
	{
		size_t i = 0;
		const mbedtls_ecp_curve_info *curve_info;

		for (curve_info = mbedtls_ecp_curve_list();
			 curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
			 curve_info++)
		{
			ecp_supported_grp_id[i++] = curve_info->grp_id;
		}
		ecp_supported_grp_id[i] = MBEDTLS_ECP_DP_NONE;

		init_done = 1;
	}

	return (ecp_supported_grp_id);
}

/*
 * Get the curve info for the internal identifier
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_grp_id(mbedtls_ecp_group_id grp_id)
{
	const mbedtls_ecp_curve_info *curve_info;

	for (curve_info = mbedtls_ecp_curve_list();
		 curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
		 curve_info++)
	{
		if (curve_info->grp_id == grp_id)
			return (curve_info);
	}

	return (NULL);
}

/*
 * Get the curve info from the TLS identifier
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_tls_id(uint16_t tls_id)
{
	const mbedtls_ecp_curve_info *curve_info;

	for (curve_info = mbedtls_ecp_curve_list();
		 curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
		 curve_info++)
	{
		if (curve_info->tls_id == tls_id)
			return (curve_info);
	}

	return (NULL);
}

#endif /* MBEDTLS_ECP_C */

#if defined(MBEDTLS_PK_C)

/* Parameter validation macros based on platform_util.h */
#define PK_VALIDATE_RET(cond) \
	MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_PK_BAD_INPUT_DATA)
#define PK_VALIDATE(cond) \
	MBEDTLS_INTERNAL_VALIDATE(cond)

/*
 * Tell if a PK can do the operations of the given type
 */
int mbedtls_pk_can_do(const mbedtls_pk_context *ctx, mbedtls_pk_type_t type)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
		{.base = &type, .len = sizeof(mbedtls_pk_type_t)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_pk_can_do, MBEDTLS_SFN_ID);
}

/*
 * Free (the components of) a mbedtls_pk_context
 */
void mbedtls_pk_free(mbedtls_pk_context *ctx)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
	};

	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_pk_free, MBEDTLS_SFN_ID);
}

/*
 * Initialise a mbedtls_pk_context
 */
void mbedtls_pk_init(mbedtls_pk_context *ctx)
{
	PK_VALIDATE(ctx != NULL);

	ctx->pk_info = NULL;
	ctx->pk_ctx = NULL;
}

/*
 * Encrypt message
 */
int mbedtls_pk_encrypt(mbedtls_pk_context *ctx,
					   const unsigned char *input, size_t ilen,
					   unsigned char *output, size_t *olen, size_t osize,
					   int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
		{.base = input, .len = ilen},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = osize},
	};

	result =  API_INT_DISPATCH(mbedtls_pk_encrypt, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

/*
 * Make a signature (restartable)
 */
int mbedtls_pk_sign_restartable(mbedtls_pk_context *ctx,
								mbedtls_md_type_t md_alg,
								const unsigned char *hash, size_t hash_len,
								unsigned char *sig, size_t *sig_len,
								int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
								mbedtls_pk_restart_ctx *rs_ctx)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
		{.base = &md_alg, .len = sizeof(mbedtls_md_type_t)},
		{.base = hash, .len = hash_len},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
		{.base = rs_ctx, .len = sizeof(mbedtls_pk_restart_ctx)},
	};

	nsc_outvec out_vec[] = {
		{.base = sig, .len = 0},
	};

	result =  API_INT_DISPATCH(mbedtls_pk_sign_restartable, MBEDTLS_SFN_ID);
	*sig_len = out_vec[0].len;

	return result;
}

/*
 * Verify a signature (restartable)
 */
int mbedtls_pk_verify_restartable(mbedtls_pk_context *ctx,
								  mbedtls_md_type_t md_alg,
								  const unsigned char *hash, size_t hash_len,
								  const unsigned char *sig, size_t sig_len,
								  mbedtls_pk_restart_ctx *rs_ctx)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
		{.base = &md_alg, .len = sizeof(mbedtls_md_type_t)},
		{.base = hash, .len = hash_len},
		{.base = sig, .len = sig_len},
		{.base = rs_ctx, .len = sizeof(mbedtls_pk_restart_ctx)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_pk_verify_restartable, MBEDTLS_SFN_ID);
}

int mbedtls_pk_decrypt(mbedtls_pk_context *ctx,
					   const unsigned char *input, size_t ilen,
					   unsigned char *output, size_t *olen, size_t osize,
					   int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
		{.base = input, .len = ilen},
		{.base = (void *)f_rng, .len = sizeof(uint32_t)},
		{.base = p_rng, .len = sizeof(uint32_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = osize},
	};

	result =  API_INT_DISPATCH(mbedtls_pk_decrypt, MBEDTLS_SFN_ID);
	*olen = out_vec[0].len;

	return result;
}

/*
 * Get key size in bits
 */
size_t mbedtls_pk_get_bitlen(const mbedtls_pk_context *ctx)
{
	size_t olen;
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
	};

	nsc_outvec out_vec[] = {
		{.base = &olen, .len = sizeof(olen)},
	};
	API_VOID_DISPATCH(mbedtls_pk_get_bitlen, MBEDTLS_SFN_ID);
	olen = out_vec[0].len;
	return olen;
}

/*
 * Make a signature
 */
int mbedtls_pk_sign(mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg,
					const unsigned char *hash, size_t hash_len,
					unsigned char *sig, size_t *sig_len,
					int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
	return (mbedtls_pk_sign_restartable(ctx, md_alg, hash, hash_len,
										sig, sig_len, f_rng, p_rng, NULL));
}

/*
 * Verify a signature
 */
int mbedtls_pk_verify(mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg,
					  const unsigned char *hash, size_t hash_len,
					  const unsigned char *sig, size_t sig_len)
{
	return (mbedtls_pk_verify_restartable(ctx, md_alg, hash, hash_len,
										  sig, sig_len, NULL));
}

/*
 * Export debug information
 */
int mbedtls_pk_debug(const mbedtls_pk_context *ctx, mbedtls_pk_debug_item *items)
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
		{.base = items, .len = sizeof(mbedtls_pk_debug_item)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_pk_debug, MBEDTLS_SFN_ID);
}

int mbedtls_pk_parse_subpubkey(unsigned char **p, const unsigned char *end,
                        mbedtls_pk_context *pk)
{
	nsc_invec in_vec[] = {
		{.base = p, .len = sizeof(uint32_t)},
		{.base = end, .len = sizeof(uint32_t)},
		{.base = pk, .len = sizeof(mbedtls_pk_context)},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_pk_parse_subpubkey, MBEDTLS_SFN_ID);
}

/*
 * Parse a private key
 */
int mbedtls_pk_parse_key(mbedtls_pk_context *pk,
						 const unsigned char *key, size_t keylen,
						 const unsigned char *pwd, size_t pwdlen)
{
	nsc_invec in_vec[] = {
		{.base = pk, .len = sizeof(mbedtls_pk_context)},
		{.base = key, .len = keylen},
		{.base = pwd, .len = pwdlen},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_pk_parse_key, MBEDTLS_SFN_ID);
}

mbedtls_ecp_group_id mbedtls_pk_get_grp_id(const mbedtls_pk_context *ctx)
{
	mbedtls_ecp_group_id gid;
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_pk_context)},
	};
	nsc_outvec out_vec[] = {
		{.base = &gid, .len = sizeof(mbedtls_ecp_group_id)},
	};

	API_VOID_DISPATCH(mbedtls_pk_get_grp_id, MBEDTLS_SFN_ID);

	return gid;
}

#endif /* MBEDTLS_PK_C */

#if defined(MBEDTLS_BIGNUM_C)

/*
 * Unallocate one MPI
 */
void mbedtls_mpi_free(mbedtls_mpi *X)
{
	nsc_invec in_vec[] = {
		{.base = X, .len = sizeof(mbedtls_mpi)},
	};
	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_mpi_free, MBEDTLS_SFN_ID);
}

/*
 * Copy the contents of Y into X
 */
int mbedtls_mpi_copy(mbedtls_mpi *X, const mbedtls_mpi *Y)
{
	nsc_invec in_vec[] = {
		{.base = X, .len = sizeof(mbedtls_mpi)},
	};
	nsc_outvec out_vec[] = {
		{.base = (void *)Y, .len = sizeof(mbedtls_mpi)},
	};

	return API_INT_DISPATCH(mbedtls_mpi_copy, MBEDTLS_SFN_ID);
}

/*
 * Return the total size in bytes
 */
size_t mbedtls_mpi_size(const mbedtls_mpi *X)
{
	size_t len;
	nsc_invec in_vec[] = {
		{.base = X, .len = sizeof(mbedtls_mpi)},
	};
	nsc_outvec out_vec[] = {
		{.base = &len, .len = sizeof(len)},
	};

	API_VOID_DISPATCH(mbedtls_mpi_size, MBEDTLS_SFN_ID);
	len = out_vec[0].len;
	return len;
}

/*
 * Import X from unsigned binary data, big endian
 */
int mbedtls_mpi_read_binary(mbedtls_mpi *X, const unsigned char *buf, size_t buflen)
{
	nsc_invec in_vec[] = {
		{.base = X, .len = sizeof(mbedtls_mpi)},
		{.base = buf, .len = buflen},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_mpi_read_binary, MBEDTLS_SFN_ID);
}

/*
 * Import from an ASCII string
 */
int mbedtls_mpi_read_string(mbedtls_mpi *X, int radix, const char *s)
{
	nsc_invec in_vec[] = {
		{.base = X, .len = sizeof(mbedtls_mpi)},
		{.base = s, .len = radix},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_mpi_read_string, MBEDTLS_SFN_ID);
}
#endif /* MBEDTLS_BIGNUM_C */

#if defined(MBEDTLS_X509_CRT_PARSE_C)

/*
 * NSA Suite B Profile
 */
const mbedtls_x509_crt_profile mbedtls_x509_crt_profile_suiteb =
	{
		/* Only SHA-256 and 384 */
		MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256) |
			MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384),
		/* Only ECDSA */
		MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECDSA) |
			MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECKEY),
#if defined(MBEDTLS_ECP_C)
		/* Only NIST P-256 and P-384 */
		MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP256R1) |
			MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP384R1),
#else
		0,
#endif
		0,
};

int mbedtls_x509_crt_verify_info(char *buf, size_t size, const char *prefix,
								 uint32_t flags)
{
	nsc_invec in_vec[] = {
		{.base = prefix, .len = 0},
		{.base = &flags, .len = sizeof(uint32_t)},
	};
	nsc_outvec out_vec[] = {
		{.base = buf, .len = size},
	};
	return API_INT_DISPATCH(mbedtls_x509_crt_verify_info, MBEDTLS_SFN_ID);
}
#if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
int mbedtls_x509_crt_check_extended_key_usage(const mbedtls_x509_crt *crt,
											  const char *usage_oid,
											  size_t usage_len)
{
	nsc_invec in_vec[] = {
		{.base = crt, .len = sizeof(mbedtls_x509_crt)},
		{.base = usage_oid, .len = usage_len},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_x509_crt_check_extended_key_usage, MBEDTLS_SFN_ID);
}
#endif

#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
int mbedtls_x509_crt_check_key_usage(const mbedtls_x509_crt *crt,
									 unsigned int usage)
{
	nsc_invec in_vec[] = {
		{.base = crt, .len = sizeof(mbedtls_x509_crt)},
		{.base = &usage, .len = sizeof(unsigned int)},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_x509_crt_check_key_usage, MBEDTLS_SFN_ID);
}
#endif

/*
 * Initialize a certificate chain
 */
void mbedtls_x509_crt_init(mbedtls_x509_crt *crt)
{
	memset(crt, 0, sizeof(mbedtls_x509_crt));
}

/*
 * Unallocate all certificate data
 */
void mbedtls_x509_crt_free(mbedtls_x509_crt *crt)
{
	nsc_invec in_vec[] = {
		{.base = crt, .len = sizeof(mbedtls_x509_crt)},
	};

	API_VOID_DISPATCH_NO_OUTVEC(mbedtls_x509_crt_free, MBEDTLS_SFN_ID);
}

int mbedtls_x509_crt_parse_der_nocopy(mbedtls_x509_crt *chain,
									  const unsigned char *buf,
									  size_t buflen)
{
	nsc_invec in_vec[] = {
		{.base = chain, .len = sizeof(mbedtls_x509_crt)},
		{.base = buf, .len = buflen},
	};

	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_x509_crt_parse_der_nocopy, MBEDTLS_SFN_ID);
}

/*
 * Verify the certificate validity, with profile, restartable version
 *
 * This function:
 *  - checks the requested CN (if any)
 *  - checks the type and size of the EE cert's key,
 *    as that isn't done as part of chain building/verification currently
 *  - builds and verifies the chain
 *  - then calls the callback and merges the flags
 */
int mbedtls_x509_crt_verify_restartable(mbedtls_x509_crt *crt,
										mbedtls_x509_crt *trust_ca,
										mbedtls_x509_crl *ca_crl,
										const mbedtls_x509_crt_profile *profile,
										const char *cn, uint32_t *flags,
										int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
										void *p_vrfy,
										mbedtls_x509_crt_restart_ctx *rs_ctx)
{
	nsc_invec in_vec[] = {
		{.base = crt, .len = sizeof(mbedtls_x509_crt)},
		{.base = trust_ca, .len = sizeof(mbedtls_x509_crt)},
		{.base = ca_crl, .len = sizeof(mbedtls_x509_crl)},
		{.base = profile, .len = sizeof(mbedtls_x509_crt_profile)},
		{.base = cn, .len = 0},
		{.base = flags, .len = sizeof(flags)},
		{.base = (void *)f_vrfy, .len = sizeof(uint32_t)},
		{.base = p_vrfy, .len = sizeof(uint32_t)},
		{.base = rs_ctx, .len = sizeof(mbedtls_x509_crt_restart_ctx)},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_x509_crt_verify_restartable, MBEDTLS_SFN_ID);
}

int mbedtls_x509_crt_info(char *buf, size_t size, const char *prefix,
						  const mbedtls_x509_crt *crt)
{
	nsc_invec in_vec[] = {
		{.base = crt, .len = sizeof(mbedtls_x509_crt)},
		{.base = prefix, .len = 0},
	};

	nsc_outvec out_vec[] = {
		{.base = buf, .len = size},
	};
	return API_INT_DISPATCH(mbedtls_x509_crt_info, MBEDTLS_SFN_ID);
}

int mbedtls_x509_crt_parse(mbedtls_x509_crt *chain,
                            const unsigned char *buf,
                            size_t buflen)
{
	int result;
	// const unsigned char *buf_alloc = mbedtls_calloc(1, buflen),
	nsc_invec in_vec[] = {
		{.base = chain, .len = sizeof(mbedtls_x509_crt)},
		{.base = buf, .len = buflen},
	};
	result = API_INT_DISPATCH_NO_OUTVEC(mbedtls_x509_crt_parse, MBEDTLS_SFN_ID);
	// mbedtls_free(buf_alloc);
	return result;
}

#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_MD_C)
const mbedtls_md_info_t *mbedtls_md_info_from_type( mbedtls_md_type_t md_type )
{
	mbedtls_md_info_t *md_info = mbedtls_calloc(1, sizeof(struct mbedtls_md_info_t));
	nsc_invec in_vec[] = {
		{.base = &md_type, .len = sizeof(struct mbedtls_md_info_t)},
	};

	nsc_outvec out_vec[] = {
		{.base = md_info, .len = sizeof(mbedtls_md_info_t)},
	};
	API_VOID_DISPATCH(mbedtls_md_info_from_type, MBEDTLS_SFN_ID);

	if (out_vec[0].len == 0) {
		mbedtls_free(md_info);
		md_info =  NULL;
	}

	return md_info;
}

int mbedtls_md(const mbedtls_md_info_t *md_info, const unsigned char *input, size_t ilen,
			   unsigned char *output)
{
	nsc_invec in_vec[] = {
		{.base = md_info, .len = sizeof(mbedtls_md_info_t)},
		{.base = input, .len = ilen},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
	};

	return API_INT_DISPATCH(mbedtls_md, MBEDTLS_SFN_ID);
}
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_AES_C)
void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
	if (ctx == NULL)
		return;

	memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
	if (ctx == NULL)
		return;

	mbedtls_platform_zeroize(ctx, sizeof(mbedtls_aes_context));
}
#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_aes_context)},
		{.base = &mode, .len = length},
		{.base = &iv[0], .len = 16},
		{.base = input, .len = 0},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
	};

	return API_INT_DISPATCH(mbedtls_aes_crypt_cbc, MBEDTLS_SFN_ID);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_aes_context)},
		{.base = &mode, .len = length},
		{.base = &iv[0], .len = 16},
		{.base = input, .len = 0},
	};

	nsc_outvec out_vec[] = {
		{.base = output, .len = 0},
		{.base = iv_off, .len = 0},
	};

	return API_INT_DISPATCH(mbedtls_aes_crypt_cfb128, MBEDTLS_SFN_ID);
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_aes_context)},
		{.base = key, .len = keybits},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_aes_setkey_dec, MBEDTLS_SFN_ID);
}

int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
	nsc_invec in_vec[] = {
		{.base = ctx, .len = sizeof(mbedtls_aes_context)},
		{.base = key, .len = keybits},
	};
	return API_INT_DISPATCH_NO_OUTVEC(mbedtls_aes_setkey_enc, MBEDTLS_SFN_ID);
}
#endif /*MBEDTLS_AES_C*/

int mem_s2ns_copy(void *dest, void *src, size_t n)
{
	int result;

	nsc_invec in_vec[] = {
		{.base = src, .len = n},
	};
	nsc_outvec out_vec[] = {
		{.base = dest, .len = n},
	};
	result = API_INT_DISPATCH(mem_s2ns_copy, MBEDTLS_SFN_ID);
	return result;
}

#endif  // SSL_USE_MBEDTLS_NSC
