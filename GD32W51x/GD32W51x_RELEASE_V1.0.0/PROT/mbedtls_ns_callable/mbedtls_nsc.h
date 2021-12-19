#ifndef __MBEDTLS_NSC_H__
#define __MBEDTLS_NSC_H__

/**
 * A read-only input memory region provided to an RoT Service.
 */
typedef struct nsc_invec {
    const void *base;           /*!< the start address of the memory buffer */
    size_t len;                 /*!< the size in bytes                      */
} nsc_invec;

/**
 * A writable output memory region provided to an RoT Service.
 */
typedef struct nsc_outvec {
    void *base;                 /*!< the start address of the memory buffer */
    size_t len;                 /*!< the size in bytes                      */
} nsc_outvec;

#define __mbedtls_secure_gateway_attributes__ \
    __attribute__((cmse_nonsecure_entry, noinline, section("SFN")))

#define MBEDTLS_INT_FUNCTION(NSC_NAME)           \
int mbedtls_##NSC_NAME##_nsc(nsc_invec in_vec[], \
                            size_t in_len,       \
                            nsc_outvec out_vec[],\
                            size_t out_len)

#define MBEDTLS_VOID_FUNCTION(NSC_NAME)                           \
void mbedtls_##NSC_NAME##_nsc(nsc_invec in_vec[],                 \
                                            size_t in_len,        \
                                            nsc_outvec out_vec[], \
                                            size_t out_len)

#define MBEDTLS_ATTR_INT_FUNCTION(NSC_NAME) \
__mbedtls_secure_gateway_attributes__       \
MBEDTLS_INT_FUNCTION(NSC_NAME)

#define MBEDTLS_ATTR_VOID_FUNCTION(NSC_NAME) \
__mbedtls_secure_gateway_attributes__        \
MBEDTLS_VOID_FUNCTION(NSC_NAME)

#if defined(MBEDTLS_CIPHER_C)
MBEDTLS_INT_FUNCTION(mbedtls_cipher_auth_decrypt);

#if defined(MBEDTLS_CIPHER_MODE_AEAD)
MBEDTLS_INT_FUNCTION(mbedtls_cipher_auth_encrypt);
#endif
MBEDTLS_INT_FUNCTION(mbedtls_cipher_crypt);
MBEDTLS_VOID_FUNCTION(mbedtls_cipher_free);
MBEDTLS_VOID_FUNCTION(mbedtls_cipher_info_from_type);

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
MBEDTLS_INT_FUNCTION(mbedtls_cipher_set_padding_mode);
#endif

MBEDTLS_INT_FUNCTION(mbedtls_cipher_setkey);
MBEDTLS_INT_FUNCTION(mbedtls_cipher_setup);

#endif /*MBEDTLS_CIPHER_C*/

#if defined(MBEDTLS_DHM_C)
MBEDTLS_INT_FUNCTION(mbedtls_dhm_calc_secret);
MBEDTLS_VOID_FUNCTION(mbedtls_dhm_free);
MBEDTLS_INT_FUNCTION(mbedtls_dhm_make_public);
MBEDTLS_INT_FUNCTION(mbedtls_dhm_read_params);
MBEDTLS_INT_FUNCTION(mbedtls_dhm_make_params);
MBEDTLS_INT_FUNCTION(mbedtls_dhm_read_public);
MBEDTLS_INT_FUNCTION(mbedtls_dhm_set_group);
#endif /* MBEDTLS_DHM_C */

#if defined(MBEDTLS_ECDH_C)
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_calc_secret);
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_get_params);
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_make_public);
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_read_params);
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_make_params);
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_read_public);
MBEDTLS_INT_FUNCTION(mbedtls_ecdh_setup);
MBEDTLS_VOID_FUNCTION(mbedtls_ecdh_free);
MBEDTLS_VOID_FUNCTION(mbedtls_ecdh_init);
#endif /*MBEDTLS_ECDH_C*/

#if defined(MBEDTLS_ECP_C)

#endif /* MBEDTLS_ECP_C */


#if defined(MBEDTLS_PK_C)
MBEDTLS_INT_FUNCTION(mbedtls_pk_can_do);
MBEDTLS_VOID_FUNCTION(mbedtls_pk_free);
MBEDTLS_INT_FUNCTION(mbedtls_pk_encrypt);
MBEDTLS_INT_FUNCTION(mbedtls_pk_sign_restartable);
MBEDTLS_INT_FUNCTION(mbedtls_pk_verify_restartable);
MBEDTLS_INT_FUNCTION(mbedtls_pk_decrypt);
MBEDTLS_VOID_FUNCTION(mbedtls_pk_get_bitlen);
MBEDTLS_INT_FUNCTION(mbedtls_pk_debug);
MBEDTLS_INT_FUNCTION(mbedtls_pk_parse_subpubkey);
MBEDTLS_INT_FUNCTION(mbedtls_pk_parse_key);
MBEDTLS_VOID_FUNCTION(mbedtls_pk_get_grp_id);
#endif /* MBEDTLS_PK_C */

#if defined(MBEDTLS_BIGNUM_C)
MBEDTLS_VOID_FUNCTION(mbedtls_mpi_free);
MBEDTLS_INT_FUNCTION(mbedtls_mpi_copy);
MBEDTLS_VOID_FUNCTION(mbedtls_mpi_size);
MBEDTLS_INT_FUNCTION(mbedtls_mpi_read_binary);
MBEDTLS_INT_FUNCTION(mbedtls_mpi_read_string);
#endif /* MBEDTLS_BIGNUM_C */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_verify_info);
#if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_check_extended_key_usage);
#endif
#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_check_key_usage);
#endif
MBEDTLS_VOID_FUNCTION(mbedtls_x509_crt_free);
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_parse_der_nocopy);
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_verify_restartable);
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_info);
MBEDTLS_INT_FUNCTION(mbedtls_x509_crt_parse);
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_MD_C)
MBEDTLS_VOID_FUNCTION(mbedtls_md_info_from_type);
MBEDTLS_INT_FUNCTION(mbedtls_md);
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_AES_C)
MBEDTLS_INT_FUNCTION(mbedtls_aes_crypt_cbc);
MBEDTLS_INT_FUNCTION(mbedtls_aes_crypt_cfb128);
MBEDTLS_INT_FUNCTION(mbedtls_aes_setkey_dec);
MBEDTLS_INT_FUNCTION(mbedtls_aes_setkey_enc);
#endif

MBEDTLS_INT_FUNCTION(mem_s2ns_copy);

/*secure and non-secure region may call the following function*/

int mbedtls_random_nsc(void *p_rng, unsigned char *output, size_t output_len);


int mbedtls_verify_nsc(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags);

//void rom_set_mutex_func_nsc(int tolock, void *func);

#endif /*__MBEDTLS_NSC_H__*/