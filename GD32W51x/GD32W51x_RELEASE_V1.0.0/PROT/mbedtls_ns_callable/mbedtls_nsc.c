#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "string.h"

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
#include "rom_export.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif
#include <stdio.h>

#if defined(MBEDTLS_CIPHER_C)
/*
 * Packet-oriented decryption for AEAD modes
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_cipher_auth_decrypt)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;
    const unsigned char *iv = in_vec[1].base;
    size_t iv_len = in_vec[1].len;
    const unsigned char *ad = in_vec[2].base;
    size_t ad_len = in_vec[2].len;
    const unsigned char *input = in_vec[3].base;
    size_t ilen = in_vec[3].len;
    const unsigned char *tag = in_vec[4].base;
    size_t tag_len = in_vec[4].len;
    unsigned char *output = out_vec[0].base;
    size_t *olen = &out_vec[0].len;

    return mbedtls_cipher_auth_decrypt(ctx,
                                       iv, iv_len,
                                       ad, ad_len,
                                       input, ilen,
                                       output, olen,
                                       tag, tag_len);
}

#if defined(MBEDTLS_CIPHER_MODE_AEAD)
/*
 * Packet-oriented encryption for AEAD modes
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_cipher_auth_encrypt)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;
    const unsigned char *iv = in_vec[1].base;
    size_t iv_len = in_vec[1].len;
    const unsigned char *ad = in_vec[2].base;
    size_t ad_len = in_vec[2].len;
    const unsigned char *input = in_vec[3].base;
    size_t ilen = in_vec[3].len;
    unsigned char *tag = (unsigned char *)in_vec[4].base;
    size_t tag_len = in_vec[4].len;
    unsigned char *output = out_vec[0].base;
    size_t *olen = &out_vec[0].len;

    return mbedtls_cipher_auth_encrypt(ctx,
                                iv, iv_len,
                                ad, ad_len,
                                input, ilen,
                                output, olen,
                                tag, tag_len);
}
#endif

/*
 * Packet-oriented wrapper for non-AEAD modes
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_cipher_crypt)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;
    const unsigned char *iv = in_vec[1].base;
    size_t iv_len = in_vec[1].len;
    const unsigned char *input = in_vec[2].base;
    size_t ilen = in_vec[2].len;
    unsigned char *output = out_vec[0].base;
    size_t *olen = &out_vec[0].len;

    return mbedtls_cipher_crypt(ctx,
                                iv, iv_len,
                                input, ilen,
                                output, olen);
}

MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_cipher_free)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;

    mbedtls_cipher_free(ctx);
}

MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_cipher_info_from_type)
{
    const mbedtls_cipher_type_t cipher_type = *(mbedtls_cipher_type_t *)in_vec[0].base;
    const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_type(cipher_type);

    if (cipher_info != NULL) {
        memcpy(out_vec[0].base, cipher_info, sizeof(mbedtls_cipher_info_t));
        out_vec[0].len = sizeof(mbedtls_cipher_info_t);
    } else {
        out_vec[0].len = 0;
    }
}

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_cipher_set_padding_mode)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;
    mbedtls_cipher_padding_t mode = *((mbedtls_cipher_padding_t *)in_vec[1].base);

    return mbedtls_cipher_set_padding_mode(ctx, mode);
}
#endif


MBEDTLS_ATTR_INT_FUNCTION(mbedtls_cipher_setkey)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;
    const unsigned char *key = (const unsigned char *)in_vec[1].base;
    int key_bitlen = in_vec[1].len;
    const mbedtls_operation_t operation = *((mbedtls_operation_t *)in_vec[2].base);

    return mbedtls_cipher_setkey(ctx, key, key_bitlen, operation);
}


MBEDTLS_ATTR_INT_FUNCTION(mbedtls_cipher_setup)
{
    mbedtls_cipher_context_t *ctx = (mbedtls_cipher_context_t *)in_vec[0].base;
    mbedtls_cipher_info_t *cipher_info = (mbedtls_cipher_info_t *)in_vec[1].base;

    return mbedtls_cipher_setup(ctx, cipher_info);
}

#endif /*MBEDTLS_CIPHER_C*/

#if defined(MBEDTLS_DHM_C)
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_dhm_calc_secret)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    unsigned char *output = (unsigned char *)out_vec[0].base;
    size_t output_size = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[1].base;
    void* p_rng = (void *)in_vec[2].base;

    return mbedtls_dhm_calc_secret(ctx, output, output_size, &out_vec[0].len, f_rng, p_rng);
}
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_dhm_free)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    mbedtls_dhm_free(ctx);
}
/*
 * Create own private value X and export G^X
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_dhm_make_public)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    unsigned char *output = (unsigned char *)out_vec[0].base;
    size_t olen = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[1].base;
    int x_size = in_vec[1].len;
    void* p_rng = (void *)in_vec[2].base;

    return mbedtls_dhm_make_public(ctx, x_size, output, olen, f_rng, p_rng);
}

/*
 * Parse the ServerKeyExchange parameters
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_dhm_read_params)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    unsigned char **p = (unsigned char **)in_vec[1].base;
    const unsigned char *end = (const unsigned char *)in_vec[2].base;

    return mbedtls_dhm_read_params(ctx, p, end);
}
/*
 * Setup and write the ServerKeyExchange parameters
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_dhm_make_params)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    unsigned char *output = (unsigned char *)out_vec[0].base;
    size_t olen = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[1].base;
    int x_size = in_vec[1].len;
    void* p_rng = (void *)in_vec[2].base;

    return mbedtls_dhm_make_params(ctx, x_size, output, &out_vec[0].len, f_rng, p_rng);
}

/*
 * Import the peer's public value G^Y
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_dhm_read_public)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    const unsigned char *input = (unsigned char *)in_vec[1].base;
    size_t ilen = in_vec[1].len;

    return mbedtls_dhm_read_public(ctx, input, ilen);
}

/*
 * Set prime modulus and generator
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_dhm_set_group)
{
    mbedtls_dhm_context *ctx = (mbedtls_dhm_context *)in_vec[0].base;
    const mbedtls_mpi *P = (mbedtls_mpi *)in_vec[1].base;
    const mbedtls_mpi *G = (mbedtls_mpi *)in_vec[1].base;

    return mbedtls_dhm_set_group(ctx, P, G);
}
#endif /* MBEDTLS_DHM_C */

#if defined(MBEDTLS_ECDH_C)
/*
 * Derive and export the shared secret
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_calc_secret)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    unsigned char *buf = (unsigned char *)out_vec[0].base;
    size_t blen = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[1].base;
    void* p_rng = (void *)in_vec[2].base;

    return mbedtls_ecdh_calc_secret(ctx, &out_vec[0].len, buf, blen, f_rng, p_rng);
}
/*
 * Get parameters from a keypair
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_get_params)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    const mbedtls_ecp_keypair *key = (mbedtls_ecp_keypair *)in_vec[1].base;
    mbedtls_ecdh_side side = *(mbedtls_ecdh_side *)in_vec[2].base;

    return mbedtls_ecdh_get_params(ctx, key, side);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_make_public)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    unsigned char *buf = (unsigned char *)out_vec[0].base;
    size_t blen = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[1].base;
    void* p_rng = (void *)in_vec[2].base;

    return mbedtls_ecdh_make_public(ctx, &out_vec[0].len, buf, blen, f_rng, p_rng);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_read_params)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    const unsigned char **buf = (const unsigned char **)in_vec[1].base;
    const unsigned char *end = (const unsigned char *)in_vec[2].base;

    return mbedtls_ecdh_read_params(ctx, buf, end);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_make_params)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    unsigned char *buf = (unsigned char *)out_vec[0].base;
    size_t blen = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[1].base;
    void* p_rng = (void *)in_vec[2].base;

    return mbedtls_ecdh_make_params(ctx, &out_vec[0].len, buf, blen, f_rng, p_rng);
}

/*
 * Parse and import the client's public value
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_read_public)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    const unsigned char *buf = (unsigned char *)in_vec[1].base;
    size_t blen = in_vec[1].len;

    return mbedtls_ecdh_read_public(ctx, buf, blen);
}

/*
 * Setup context
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_ecdh_setup)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    mbedtls_ecp_group_id grp_id = *(mbedtls_ecp_group_id *)in_vec[1].base;

    return mbedtls_ecdh_setup(ctx, grp_id);
}

/*
 * Free context
 */
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_ecdh_free)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    mbedtls_ecdh_free(ctx);
}

/*
 * Initialize context
 */
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_ecdh_init)
{
    mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)in_vec[0].base;
    mbedtls_ecdh_init(ctx);
}
#endif /*MBEDTLS_ECDH_C*/

#if defined(MBEDTLS_PK_C)

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_can_do)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    mbedtls_pk_type_t type = *(mbedtls_pk_type_t *)in_vec[1].base;
    return mbedtls_pk_can_do(ctx, type);
}

MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_pk_free)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    mbedtls_pk_free(ctx);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_encrypt)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    const unsigned char *input = (const unsigned char *)in_vec[1].base;
    size_t ilen = in_vec[1].len;
    unsigned char *output = (unsigned char *)out_vec[0].base;
    size_t osize = out_vec[0].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[2].base;
    void* p_rng = (void *)in_vec[3].base;

    return mbedtls_pk_encrypt(ctx, input, ilen, output, &out_vec[0].len, osize, f_rng, p_rng);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_sign_restartable)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    mbedtls_md_type_t md_alg = *(mbedtls_md_type_t *)in_vec[1].base;
    const unsigned char *hash = (const unsigned char *)in_vec[2].base;
    size_t hash_len = in_vec[2].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[3].base;
    void* p_rng = (void *)in_vec[4].base;
    mbedtls_pk_restart_ctx *rs_ctx = (mbedtls_pk_restart_ctx *)in_vec[5].base;
    unsigned char * sig = out_vec[0].base;
    size_t *sig_len = &out_vec[0].len;

    return mbedtls_pk_sign_restartable(ctx, md_alg, hash, hash_len, sig, sig_len, f_rng, p_rng, rs_ctx);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_verify_restartable)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    mbedtls_md_type_t md_alg = *(mbedtls_md_type_t *)in_vec[1].base;
    const unsigned char *hash = (const unsigned char *)in_vec[2].base;
    size_t hash_len = in_vec[2].len;
    unsigned char *sig = (unsigned char *)in_vec[3].base;
    size_t sig_len = in_vec[3].len;
    mbedtls_pk_restart_ctx *rs_ctx = (mbedtls_pk_restart_ctx *)in_vec[4].base;

    return mbedtls_pk_verify_restartable(ctx, md_alg, hash, hash_len, sig, sig_len, rs_ctx);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_decrypt)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    const unsigned char *input = (const unsigned char *)in_vec[1].base;
    size_t ilen = in_vec[1].len;
    int (*f_rng)(void *, unsigned char *, size_t) = (int (*)(void *, unsigned char *, size_t))in_vec[2].base;
    void* p_rng = (void *)in_vec[3].base;

    unsigned char * output = out_vec[0].base;
    size_t osize = out_vec[0].len;
    size_t *olen = &out_vec[0].len;

    return mbedtls_pk_decrypt(ctx, input, ilen, output, olen, osize, f_rng, p_rng);
}

MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_pk_get_bitlen)
{
    const mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;

    out_vec[0].len = mbedtls_pk_get_bitlen(ctx);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_debug)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    mbedtls_pk_debug_item *items = (mbedtls_pk_debug_item *)in_vec[1].base;

    return mbedtls_pk_debug(ctx, items);
}

MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_pk_get_grp_id)
{
    mbedtls_pk_context *ctx = (mbedtls_pk_context *)in_vec[0].base;
    mbedtls_ecp_group_id *gid = (mbedtls_ecp_group_id *)out_vec[0].base;

    *gid = ((mbedtls_ecp_keypair *)ctx->pk_ctx)->grp.id;
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_parse_subpubkey)
{
    unsigned char **p = (unsigned char **)in_vec[0].base;
    unsigned char *end = (unsigned char *)in_vec[1].base;
    mbedtls_pk_context *pk = (mbedtls_pk_context *)in_vec[2].base;

    return mbedtls_pk_parse_subpubkey(p, end, pk);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_pk_parse_key)
{
    mbedtls_pk_context *pk = (mbedtls_pk_context *)in_vec[0].base;
    const unsigned char *key = (const unsigned char *)in_vec[1].base;
    size_t keylen = in_vec[1].len;
    const unsigned char *pwd = (const unsigned char *)in_vec[2].base;
    size_t pwdlen = in_vec[2].len;

    return mbedtls_pk_parse_key(pk, key, keylen, pwd, pwdlen);
}

#endif /* MBEDTLS_PK_C */


#if defined(MBEDTLS_BIGNUM_C)

/*
 * Unallocate one MPI
 */
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_mpi_free)
{
    mbedtls_mpi *X = (mbedtls_mpi *)in_vec[0].base;
    mbedtls_mpi_free(X);
}
/*
 * Copy the contents of Y into X
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_mpi_copy)
{
    mbedtls_mpi *X = (mbedtls_mpi *)in_vec[0].base;
    mbedtls_mpi *Y = (mbedtls_mpi *)out_vec[0].base;

    return mbedtls_mpi_copy(X, Y);
}

/*
 * Return the total size in bytes
 */
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_mpi_size)
{
    size_t len;
    mbedtls_mpi *X = (mbedtls_mpi *)in_vec[0].base;

    len = mbedtls_mpi_size(X);

    out_vec[0].len = len;
}

/*
 * Import X from unsigned binary data, big endian
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_mpi_read_binary)
{
    mbedtls_mpi *X = (mbedtls_mpi *)in_vec[0].base;
    const unsigned char *buf = (const unsigned char *)in_vec[1].base;
    size_t buflen = in_vec[1].len;

    return mbedtls_mpi_read_binary(X, buf, buflen);
}

/*
 * Import from an ASCII string
 */
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_mpi_read_string)
{
    mbedtls_mpi *X = (mbedtls_mpi *)in_vec[0].base;
    const char *s = (const char *)in_vec[1].base;
    int radix = in_vec[1].len;

    return mbedtls_mpi_read_string(X, radix, s);
}
#endif /* MBEDTLS_BIGNUM_C */


#if defined(MBEDTLS_X509_CRT_PARSE_C)

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_verify_info)
{
    char *buf = (char *)out_vec[0].base;
    size_t size = out_vec[0].len;
    const char * prefix = (const char *)in_vec[0].base;
    uint32_t flags = *(uint32_t *)in_vec[1].base;

    return mbedtls_x509_crt_verify_info(buf, size, prefix, flags);
}

#if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_check_extended_key_usage)
{
    const mbedtls_x509_crt *crt = (const mbedtls_x509_crt *)in_vec[0].base;
    const char *usage_oid = (const char *)in_vec[1].base;
    size_t usage_len = in_vec[1].len;
    return mbedtls_x509_crt_check_extended_key_usage(crt, usage_oid, usage_len);
}
#endif

#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_check_key_usage)
{
    const mbedtls_x509_crt *crt = (const mbedtls_x509_crt *)in_vec[0].base;
    unsigned int usage = *(unsigned int *)in_vec[1].base;

    return mbedtls_x509_crt_check_key_usage(crt, usage);
}
#endif

/*
 * Unallocate all certificate data
 */
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_x509_crt_free)
{
    mbedtls_x509_crt *crt = (mbedtls_x509_crt *)in_vec[0].base;

    mbedtls_x509_crt_free(crt);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_parse_der_nocopy)
{
    mbedtls_x509_crt *chain = (mbedtls_x509_crt *)in_vec[0].base;
    const unsigned char *buf = (const unsigned char *)in_vec[1].base;
    size_t buflen = in_vec[1].len;

    return mbedtls_x509_crt_parse_der_nocopy(chain, buf, buflen);
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
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_verify_restartable)
{

    mbedtls_x509_crt *crt = (mbedtls_x509_crt *)in_vec[0].base;
    mbedtls_x509_crt *trust_ca = (mbedtls_x509_crt *)in_vec[1].base;
    mbedtls_x509_crl *ca_crl = (mbedtls_x509_crl *)in_vec[2].base;
    const mbedtls_x509_crt_profile *profile = (const mbedtls_x509_crt_profile *)in_vec[3].base;
    const char *cn = (const char *)in_vec[4].base;
    uint32_t *flags = (uint32_t *)in_vec[5].base;
    int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *) = (int (*)(void *, mbedtls_x509_crt *, int, uint32_t *))in_vec[6].base;
    void *p_vrfy = (void *)in_vec[7].base;
    mbedtls_x509_crt_restart_ctx *rs_ctx = (mbedtls_x509_crt_restart_ctx *)in_vec[8].base;

    return mbedtls_x509_crt_verify_restartable(crt,
                                        trust_ca,
                                        ca_crl,
                                        profile,
                                        cn, flags,
                                        f_vrfy,
                                        p_vrfy,
                                        rs_ctx);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_info)
{
    const mbedtls_x509_crt *crt = (const mbedtls_x509_crt *)in_vec[0].base;
    const char *prefix = (const char *)in_vec[1].base;
    char *buf = (char *)out_vec[0].base;
    size_t size = out_vec[0].len;

    return mbedtls_x509_crt_info(buf, size, prefix, crt);
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_x509_crt_parse)
{
    mbedtls_x509_crt *chain = (mbedtls_x509_crt *)in_vec[0].base;
    const unsigned char *buf = (const unsigned char *)in_vec[1].base;
    size_t buflen = in_vec[1].len;

    return mbedtls_x509_crt_parse(chain, buf, buflen);
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_MD_C)
MBEDTLS_ATTR_VOID_FUNCTION(mbedtls_md_info_from_type)
{
    const mbedtls_md_type_t md_type = *(mbedtls_md_type_t *)in_vec[0].base;
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_type);

    if (md_info != NULL) {
        memcpy(out_vec[0].base, md_info, sizeof(mbedtls_md_info_t));
        out_vec[0].len = sizeof(mbedtls_md_info_t);
    } else {
        out_vec[0].len = 0;
    }
}

MBEDTLS_ATTR_INT_FUNCTION(mbedtls_md)
{
    const mbedtls_md_info_t *md_info = (const mbedtls_md_info_t *)in_vec[0].base;
    const unsigned char *input = (const unsigned char *)in_vec[1].base;
    size_t ilen = in_vec[1].len;
    unsigned char *output = (unsigned char *)out_vec[0].base;

    return mbedtls_md(md_info, input, ilen, output);
}
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_AES_C)
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_aes_crypt_cbc)
{
    mbedtls_aes_context *ctx = (mbedtls_aes_context *)in_vec[0].base;
    int mode = *(int *)in_vec[1].base;
    size_t length = in_vec[1].len;
    unsigned char *iv = (unsigned char *)in_vec[2].base;
    const unsigned char *input = (const unsigned char *)in_vec[3].base;
    unsigned char *output = (unsigned char *)out_vec[0].base;

    return mbedtls_aes_crypt_cbc(ctx, mode, length, iv, input, output);
}
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_aes_crypt_cfb128)
{
    mbedtls_aes_context *ctx = (mbedtls_aes_context *)in_vec[0].base;
    int mode = *(int *)in_vec[1].base;
    size_t length = in_vec[1].len;
    unsigned char *iv = (unsigned char *)in_vec[2].base;
    const unsigned char *input = (const unsigned char *)in_vec[3].base;
    unsigned char *output = (unsigned char *)out_vec[0].base;
    size_t *iv_off = (size_t *)out_vec[1].base;
    return mbedtls_aes_crypt_cfb128(ctx, mode, length, iv_off, iv, input, output);
}
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_aes_setkey_dec)
{
    mbedtls_aes_context *ctx = (mbedtls_aes_context *)in_vec[0].base;
    const unsigned char *key = (const unsigned char *)in_vec[1].base;
    unsigned int keybits = (unsigned int)in_vec[1].len;

    return mbedtls_aes_setkey_dec(ctx, key, keybits);
}
MBEDTLS_ATTR_INT_FUNCTION(mbedtls_aes_setkey_enc)
{
    mbedtls_aes_context *ctx = (mbedtls_aes_context *)in_vec[0].base;
    const unsigned char *key = (const unsigned char *)in_vec[1].base;
    unsigned int keybits = (unsigned int)in_vec[1].len;

    return mbedtls_aes_setkey_enc(ctx, key, keybits);
}
#endif

MBEDTLS_ATTR_INT_FUNCTION(mem_s2ns_copy)
{
    void *dest = out_vec[0].base;
    size_t n = out_vec[0].len;
    const void *src = in_vec[0].base;

    if (dest == NULL || src == NULL || n == 0)
        return -1;

    memcpy(dest, src, n);

    return 0;
}



__mbedtls_secure_gateway_attributes__
int mbedtls_random_nsc(void *p_rng, unsigned char *output, size_t output_len)
{
    return rom_rand(output, output_len);
}

__mbedtls_secure_gateway_attributes__
int mbedtls_verify_nsc(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    char *buf = mbedtls_calloc(1, 1000);
    ((void) data);

    printf("Verify requested for (Depth %d):\n", depth);

    if (buf != NULL) {
        mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
        printf("%s", buf);
        mbedtls_free(buf);
    } else {
        printf("Verify crt info allocated failed!\n");
    }

    if(((*flags) & MBEDTLS_X509_BADCERT_EXPIRED) != 0)
        printf("server certificate has expired\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_REVOKED) != 0)
        printf("  ! server certificate has been revoked\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0)
        printf("  ! CN mismatch\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0)
        printf("  ! self-signed or not signed by a trusted CA\n");

    if(((*flags) & MBEDTLS_X509_BADCRL_NOT_TRUSTED) != 0)
        printf("  ! CRL not trusted\n");

    if(((*flags) & MBEDTLS_X509_BADCRL_EXPIRED) != 0)
        printf("  ! CRL expired\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_OTHER) != 0)
        printf("  ! other (unknown) flag\n");

    if((*flags) == 0)
        printf("  Certificate verified without error flags\n");

    return(0);
}
