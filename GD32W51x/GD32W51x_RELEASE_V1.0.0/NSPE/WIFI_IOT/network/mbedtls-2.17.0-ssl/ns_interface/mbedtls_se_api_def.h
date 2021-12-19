/*!
    \file    mbedtls_se_api_def.h
    \brief   MbesTLS SE API definition for GD32W51x WiFi SDK

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

#ifndef __MBEDTLS_SE_API_DEF_H
#define __MBEDTLS_SE_API_DEF_H

#define MBEDTLS_GD_FUNCTION_NAME(x) gd__##x

// #if defined(MBEDTLS_CIPHER_C)
#define mbedtls_cipher_auth_decrypt \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_auth_decrypt)

// #if defined(MBEDTLS_CIPHER_MODE_AEAD)
#define mbedtls_cipher_auth_encrypt \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_auth_encrypt)
// #endif
#define mbedtls_cipher_crypt \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_crypt)
#define mbedtls_cipher_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_free)
#define mbedtls_cipher_info_from_type \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_info_from_type)

// #if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
#define mbedtls_cipher_set_padding_mode \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_set_padding_mode)
// #endif

#define mbedtls_cipher_setkey \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_setkey)
#define mbedtls_cipher_setup \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_setup)

#define mbedtls_cipher_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_cipher_init)

// #endif /*MBEDTLS_CIPHER_C*/

// #if defined(MBEDTLS_DHM_C)
#define mbedtls_dhm_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_init)
#define mbedtls_dhm_calc_secret \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_calc_secret)
#define mbedtls_dhm_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_free)
#define mbedtls_dhm_make_public \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_make_public)
#define mbedtls_dhm_read_params \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_read_params)
#define mbedtls_dhm_make_params \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_make_params)
#define mbedtls_dhm_read_public \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_read_public)
#define mbedtls_dhm_set_group \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_dhm_set_group)
// #endif /* MBEDTLS_DHM_C */

// #if defined(MBEDTLS_ECDH_C)
#define mbedtls_ecdh_calc_secret \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_calc_secret)
#define mbedtls_ecdh_get_params \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_get_params)
#define mbedtls_ecdh_make_public \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_make_public)
#define mbedtls_ecdh_read_params \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_read_params)
#define mbedtls_ecdh_make_params \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_make_params)
#define mbedtls_ecdh_read_public \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_read_public)
#define mbedtls_ecdh_setup \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_setup)
#define mbedtls_ecdh_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_free)
#define mbedtls_ecdh_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecdh_init)
// #endif /*MBEDTLS_ECDH_C*/

// #if defined(MBEDTLS_PK_C)
#define mbedtls_pk_can_do \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_can_do)
#define mbedtls_pk_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_free)
#define mbedtls_pk_encrypt \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_encrypt)
#define mbedtls_pk_sign_restartable \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_sign_restartable)
#define mbedtls_pk_verify_restartable \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_verify_restartable)
#define mbedtls_pk_decrypt \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_decrypt)
#define mbedtls_pk_get_bitlen \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_get_bitlen)
#define mbedtls_pk_debug \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_debug)
#define mbedtls_pk_parse_subpubkey \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_parse_subpubkey)
#define mbedtls_pk_parse_key \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_parse_key)
#define mbedtls_pk_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_init)
#define mbedtls_pk_sign \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_sign)
#define mbedtls_pk_verify \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_pk_verify)

// #endif /* MBEDTLS_PK_C */

// #if defined(MBEDTLS_BIGNUM_C)
#define mbedtls_mpi_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_mpi_free)
#define mbedtls_mpi_copy \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_mpi_copy)
#define mbedtls_mpi_size \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_mpi_size)
#define mbedtls_mpi_read_binary \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_mpi_read_binary)
#define mbedtls_mpi_read_string \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_mpi_read_string)
// #endif /* MBEDTLS_BIGNUM_C */

// #if defined(MBEDTLS_X509_CRT_PARSE_C)
#define mbedtls_x509_crt_verify_info \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_verify_info)

// #if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
#define mbedtls_x509_crt_check_extended_key_usage \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_check_extended_key_usage)
// #endif

// #if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
#define mbedtls_x509_crt_check_key_usage \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_check_key_usage)
// #endif
#define mbedtls_x509_crt_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_free)
#define mbedtls_x509_crt_parse_der_nocopy \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_parse_der_nocopy)
#define mbedtls_x509_crt_verify_restartable \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_verify_restartable)
#define mbedtls_x509_crt_info \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_info)
#define mbedtls_x509_crt_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_init)
#define mbedtls_x509_crt_profile_suiteb \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_profile_suiteb)
#define mbedtls_x509_crt_parse \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_x509_crt_parse)
// #endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_MD_C)
#define mbedtls_md_info_from_type \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_md_info_from_type)
#define mbedtls_md \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_md)
#endif /* MBEDTLS_MD_C */

#define mbedtls_ecp_grp_id_list \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecp_grp_id_list)
#define mbedtls_ecp_curve_info_from_grp_id \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecp_curve_info_from_grp_id)
#define mbedtls_ecp_curve_info_from_tls_id \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecp_curve_info_from_tls_id)
#define mbedtls_ecp_curve_list \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ecp_curve_list)

#define mbedtls_ssl_ciphersuite_from_id \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_ciphersuite_from_id)
#define mbedtls_ssl_ciphersuite_uses_psk \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_ciphersuite_uses_psk)
#define ciphersuite_preference_default \
	MBEDTLS_GD_FUNCTION_NAME(ciphersuite_preference_default)
#define mbedtls_ssl_get_ciphersuite_name \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_get_ciphersuite_name)
#define mbedtls_ciphersuite_preference_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ciphersuite_preference_init)
#define mbedtls_ssl_get_ciphersuite_sig_alg \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_get_ciphersuite_sig_alg)
#define mbedtls_ssl_list_ciphersuites \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_list_ciphersuites)
#define mbedtls_ssl_ciphersuite_from_string \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_ciphersuite_from_string)
#define mbedtls_ssl_ciphersuite_uses_ec \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_ciphersuite_uses_ec)
#define mbedtls_ssl_get_ciphersuite_id \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_get_ciphersuite_id)
#define mbedtls_ssl_get_ciphersuite_sig_pk_alg \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_ssl_get_ciphersuite_sig_pk_alg)

#define mbedtls_platform_zeroize \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_platform_zeroize)

//#if defined(MBEDTLS_AES_C)
#define mbedtls_aes_init \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_aes_init)
#define mbedtls_aes_free \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_aes_free)
#define mbedtls_aes_crypt_cbc \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_aes_crypt_cbc)
#define mbedtls_aes_crypt_cfb128 \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_aes_crypt_cfb128)
#define mbedtls_aes_setkey_dec \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_aes_setkey_dec)
#define mbedtls_aes_setkey_enc \
	MBEDTLS_GD_FUNCTION_NAME(mbedtls_aes_setkey_enc)

//#endif

#endif  //  __MBEDTLS_SE_API_DEF_H
