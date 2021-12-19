#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
#include "mbedtls/x509_crt.h"
#endif


#if defined(MBEDTLS_X509_CRT_PARSE_C)

const mbedtls_x509_crt_profile mbedtls_x509_crt_profile_default =
	{
// #if defined(MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_CERTIFICATES)
		/* Allow SHA-1 (weak, but still safe in controlled environments) */
		MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA1) |
// #endif
			/* Only SHA-2 hashes */
			MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA224) |
			MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256) |
			MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384) |
			MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA512),
		0xFFFFFFF, /* Any PK alg    */
		0xFFFFFFF, /* Any curve     */
		1024,  // 2048,
};
#endif