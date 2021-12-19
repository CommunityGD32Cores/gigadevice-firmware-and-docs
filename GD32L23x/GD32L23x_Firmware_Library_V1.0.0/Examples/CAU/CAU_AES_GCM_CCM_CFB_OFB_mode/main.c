/*!
    \file    main.c
    \brief   CAU gcm, ccm, cfb, ofb example

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "gd32l233r_eval.h"
#include <stdio.h>

#define KEY_SIZE        128
#define AAD_SIZE     16
#define PLAINTEXT_SIZE  64
#define CIPHERTEXT_SIZE 64
#define TEXT_SIZE       64
#define IV_SIZE         16
#define CCM_IV_SIZE     8
#define CCM_TAG_SIZE    6
#define GCM_TAG_SIZE    16

__attribute__((aligned(4)))
const uint8_t key_128[KEY_SIZE] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                   0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                                  };

__attribute__((aligned(4)))
const uint8_t ccm_key_128[KEY_SIZE] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                                       0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
                                      };

__attribute__((aligned(4)))
const uint8_t vectors[IV_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                                 };

__attribute__((aligned(4)))
const uint8_t ccm_vectors[CCM_IV_SIZE] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

__attribute__((aligned(4)))
const uint8_t aadmessage[AAD_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                                     };

__attribute__((aligned(4)))
uint8_t plaintext[PLAINTEXT_SIZE] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                                     0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                                     0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
                                     0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                                     0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
                                     0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                                     0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
                                     0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
                                    };

__attribute__((aligned(4)))
uint8_t ciphertext_gcm[CIPHERTEXT_SIZE] = {0x3B, 0x3F, 0xD9, 0x2E, 0xB7, 0x2D, 0xAD, 0x20, 0x33, 0x34, 0x49, 0xF8, 0xE8, 0x3C, 0xFB, 0x4A,
                                           0x01, 0x0C, 0x04, 0x19, 0x99, 0xE0, 0x3F, 0x36, 0x44, 0x86, 0x24, 0x48, 0x3E, 0x58, 0x2D, 0x0E,
                                           0xA6, 0x22, 0x93, 0xCF, 0xA6, 0xDF, 0x74, 0x53, 0x5C, 0x35, 0x41, 0x81, 0x16, 0x87, 0x74, 0xDF,
                                           0x2D, 0x55, 0xA5, 0x47, 0x06, 0x27, 0x3C, 0x50, 0xD7, 0xB4, 0xF8, 0xA8, 0xCD, 0xDC, 0x6E, 0xD7
                                          };

__attribute__((aligned(4)))
uint8_t ciphertext_ccm[CIPHERTEXT_SIZE] = {0x99, 0x41, 0x6c, 0x21, 0x5b, 0x8f, 0xe6, 0xd3, 0xc9, 0x0e, 0x23, 0xa8, 0x58, 0x83, 0x60, 0x38,
                                           0xcd, 0xd2, 0xba, 0xcf, 0x0a, 0xfb, 0x2c, 0xb0, 0xc1, 0x0c, 0x16, 0x29, 0xd9, 0x3e, 0xd7, 0x36,
                                           0x70, 0xa8, 0xf6, 0xd5, 0x1f, 0xcc, 0x2f, 0x2d, 0x77, 0xca, 0x86, 0xea, 0x3b, 0x25, 0x91, 0x6c,
                                           0xec, 0x2c, 0xeb, 0xde, 0x42, 0x55, 0x40, 0xa9, 0xf3, 0xb5, 0x5f, 0xf7, 0x90, 0x74, 0xee, 0x14
                                          };

__attribute__((aligned(4)))
uint8_t ciphertext_cfb[CIPHERTEXT_SIZE] = {0x3b, 0x3f, 0xd9, 0x2e, 0xb7, 0x2d, 0xad, 0x20, 0x33, 0x34, 0x49, 0xf8, 0xe8, 0x3c, 0xfb, 0x4a,
                                           0xc8, 0xa6, 0x45, 0x37, 0xa0, 0xb3, 0xa9, 0x3f, 0xcd, 0xe3, 0xcd, 0xad, 0x9f, 0x1c, 0xe5, 0x8b,
                                           0x26, 0x75, 0x1f, 0x67, 0xa3, 0xcb, 0xb1, 0x40, 0xb1, 0x80, 0x8c, 0xf1, 0x87, 0xa4, 0xf4, 0xdf,
                                           0xc0, 0x4b, 0x05, 0x35, 0x7c, 0x5d, 0x1c, 0x0e, 0xea, 0xc4, 0xc6, 0x6f, 0x9f, 0xf7, 0xf2, 0xe6
                                          };

__attribute__((aligned(4)))
uint8_t ciphertext_ofb[CIPHERTEXT_SIZE] = {0x3b, 0x3f, 0xd9, 0x2e, 0xb7, 0x2d, 0xad, 0x20, 0x33, 0x34, 0x49, 0xf8, 0xe8, 0x3c, 0xfb, 0x4a,
                                           0x77, 0x89, 0x50, 0x8d, 0x16, 0x91, 0x8f, 0x03, 0xf5, 0x3c, 0x52, 0xda, 0xc5, 0x4e, 0xd8, 0x25,
                                           0x97, 0x40, 0x05, 0x1e, 0x9c, 0x5f, 0xec, 0xf6, 0x43, 0x44, 0xf7, 0xa8, 0x22, 0x60, 0xed, 0xcc,
                                           0x30, 0x4c, 0x65, 0x28, 0xf6, 0x59, 0xc7, 0x78, 0x66, 0xa5, 0x10, 0xd9, 0xc1, 0xd6, 0xae, 0x5e
                                          };

__attribute__((aligned(4)))
uint8_t encrypt_result[TEXT_SIZE];

__attribute__((aligned(4)))
uint8_t decrypt_result[TEXT_SIZE];

__attribute__((aligned(4)))
uint8_t ccm_tag[CCM_TAG_SIZE]; /* used for storing computed MAC (TAG) */

__attribute__((aligned(4)))
uint8_t gcm_tag[GCM_TAG_SIZE];

uint8_t aad_buf[AAD_SIZE + 21]; /* buffer used to append the aad. aad_buf[] size must be equal to AAD_SIZE + 21 */

void cau_aes_encrypt(void);
void cau_aes_decrypt(void);
void cau_parameter_printf(char *name, uint8_t *data, uint32_t data_size);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    rcu_periph_clock_enable(RCU_CAU);
    gd_eval_com_init(EVAL_COM);

    /* print input parameters */
    cau_parameter_printf("plain text", (uint8_t *)plaintext, PLAINTEXT_SIZE);

    /* encryption in GCM, CCM, CFB, OFB mode */
    cau_aes_encrypt();

    /* decryption in GCM, CCM, CFB, OFB mode */
    cau_aes_decrypt();

    while(1) {
    }
}

/*!
    \brief      encryption in GCM, CCM, CFB, OFB mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cau_aes_encrypt(void)
{
    cau_parameter_struct cau_gcm_parameter;
    cau_parameter_struct cau_ccm_parameter;
    cau_parameter_struct cau_cfb_parameter;
    cau_parameter_struct cau_ofb_parameter;

    /* --------- encryption in GCM mode --------- */
    cau_deinit();
    cau_gcm_parameter.alg_dir     = CAU_ENCRYPT;
    cau_gcm_parameter.key         = (uint8_t *)key_128;
    cau_gcm_parameter.key_size    = KEY_SIZE;
    cau_gcm_parameter.iv          = (uint8_t *)vectors;
    cau_gcm_parameter.iv_size     = IV_SIZE;
    cau_gcm_parameter.input       = (uint8_t *)plaintext;
    cau_gcm_parameter.in_length   = PLAINTEXT_SIZE;
    cau_gcm_parameter.aad         = (uint8_t *)aadmessage;
    cau_gcm_parameter.aad_size    = AAD_SIZE;
    cau_aes_gcm(&cau_gcm_parameter, encrypt_result, gcm_tag);
    printf("\r\nencryption in GCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
    cau_parameter_printf("tag", gcm_tag, GCM_TAG_SIZE);

    /* --------- encryption in CCM mode --------- */
    cau_deinit();
    cau_ccm_parameter.alg_dir     = CAU_ENCRYPT;
    cau_ccm_parameter.key         = (uint8_t *)ccm_key_128;
    cau_ccm_parameter.key_size    = KEY_SIZE;
    cau_ccm_parameter.iv          = (uint8_t *)ccm_vectors;
    cau_ccm_parameter.iv_size     = CCM_IV_SIZE;
    cau_ccm_parameter.input       = (uint8_t *)plaintext;
    cau_ccm_parameter.in_length   = PLAINTEXT_SIZE;
    cau_ccm_parameter.aad         = (uint8_t *)aadmessage;
    cau_ccm_parameter.aad_size    = AAD_SIZE;
    cau_aes_ccm(&cau_ccm_parameter, encrypt_result, ccm_tag, CCM_TAG_SIZE, (uint8_t *)aad_buf);
    printf("\r\nencryption in CCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
    cau_parameter_printf("tag", ccm_tag, CCM_TAG_SIZE);
    printf("\r\n");

    /* --------- encryption in CFB mode --------- */
    cau_deinit();
    cau_cfb_parameter.alg_dir   = CAU_ENCRYPT;
    cau_cfb_parameter.key       = (uint8_t *)key_128;
    cau_cfb_parameter.key_size  = KEY_SIZE;
    cau_cfb_parameter.iv        = (uint8_t *)vectors;
    cau_cfb_parameter.iv_size   = IV_SIZE;
    cau_cfb_parameter.input     = (uint8_t *)plaintext;
    cau_cfb_parameter.in_length = PLAINTEXT_SIZE;
    cau_aes_cfb(&cau_cfb_parameter, encrypt_result);
    printf("\r\nencryption in CFB mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);

    /* --------- encryption in OFB mode --------- */
    cau_deinit();
    cau_ofb_parameter.alg_dir   = CAU_ENCRYPT;
    cau_ofb_parameter.key       = (uint8_t *)key_128;
    cau_ofb_parameter.key_size  = KEY_SIZE;
    cau_ofb_parameter.iv        = (uint8_t *)vectors;
    cau_ofb_parameter.iv_size   = IV_SIZE;
    cau_ofb_parameter.input     = (uint8_t *)plaintext;
    cau_ofb_parameter.in_length = PLAINTEXT_SIZE;
    cau_aes_ofb(&cau_ofb_parameter, encrypt_result);
    printf("\r\nencryption in OFB mode\r\n");
    /* print output parameters */
    cau_parameter_printf("encrypted data", encrypt_result, PLAINTEXT_SIZE);
}

/*!
    \brief      decryption in GCM, CCM, CFB, OFB mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cau_aes_decrypt(void)
{
    cau_parameter_struct cau_gcm_parameter;
    cau_parameter_struct cau_ccm_parameter;
    cau_parameter_struct cau_cfb_parameter;
    cau_parameter_struct cau_ofb_parameter;

    /* --------- decryption in GCM mode --------- */
    cau_deinit();
    cau_gcm_parameter.alg_dir     = CAU_DECRYPT;
    cau_gcm_parameter.key         = (uint8_t *)key_128;
    cau_gcm_parameter.key_size    = KEY_SIZE;
    cau_gcm_parameter.iv          = (uint8_t *)vectors;
    cau_gcm_parameter.iv_size     = IV_SIZE;
    cau_gcm_parameter.input       = (uint8_t *)ciphertext_gcm;
    cau_gcm_parameter.in_length   = CIPHERTEXT_SIZE;
    cau_gcm_parameter.aad         = (uint8_t *)aadmessage;
    cau_gcm_parameter.aad_size    = AAD_SIZE;
    cau_aes_gcm(&cau_gcm_parameter, decrypt_result, gcm_tag);
    printf("\r\ndecryption in GCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
    cau_parameter_printf("tag", gcm_tag, GCM_TAG_SIZE);

    /* --------- decryption in CCM mode --------- */
    cau_deinit();
    cau_ccm_parameter.alg_dir     = CAU_DECRYPT;
    cau_ccm_parameter.key         = (uint8_t *)ccm_key_128;
    cau_ccm_parameter.key_size    = KEY_SIZE;
    cau_ccm_parameter.iv          = (uint8_t *)ccm_vectors;
    cau_ccm_parameter.iv_size     = CCM_IV_SIZE;
    cau_ccm_parameter.input       = (uint8_t *)ciphertext_ccm;
    cau_ccm_parameter.in_length   = CIPHERTEXT_SIZE;
    cau_ccm_parameter.aad         = (uint8_t *)aadmessage;
    cau_ccm_parameter.aad_size    = AAD_SIZE;
    cau_aes_ccm(&cau_ccm_parameter, decrypt_result, ccm_tag, CCM_TAG_SIZE, (uint8_t *)aad_buf);
    printf("\r\ndecryption in CCM mode\r\n");
    /* print output parameters */
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
    cau_parameter_printf("tag", ccm_tag, CCM_TAG_SIZE);
    printf("\r\n");

    /* --------- decryption in CFB mode --------- */
    cau_deinit();
    cau_cfb_parameter.alg_dir   = CAU_DECRYPT;
    cau_cfb_parameter.key       = (uint8_t *)key_128;
    cau_cfb_parameter.key_size  = KEY_SIZE;
    cau_cfb_parameter.iv        = (uint8_t *)vectors;
    cau_cfb_parameter.iv_size   = IV_SIZE;
    cau_cfb_parameter.input     = (uint8_t *)ciphertext_cfb;
    cau_cfb_parameter.in_length = CIPHERTEXT_SIZE;
    cau_aes_cfb(&cau_cfb_parameter, decrypt_result);
    printf("\r\ndecryption in CFB mode\r\n");
    /* print output parameters */
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);

    /* --------- decryption in OFB mode --------- */
    cau_deinit();
    cau_ofb_parameter.alg_dir   = CAU_DECRYPT;
    cau_ofb_parameter.key       = (uint8_t *)key_128;
    cau_ofb_parameter.key_size  = KEY_SIZE;
    cau_ofb_parameter.iv        = (uint8_t *)vectors;
    cau_ofb_parameter.iv_size   = IV_SIZE;
    cau_ofb_parameter.input     = (uint8_t *)ciphertext_ofb;
    cau_ofb_parameter.in_length = CIPHERTEXT_SIZE;
    cau_aes_ofb(&cau_ofb_parameter, decrypt_result);
    printf("\r\ndecryption in OFB mode\r\n");
    cau_parameter_printf("decrypted data", decrypt_result, CIPHERTEXT_SIZE);
}

/*!
    \brief      print cau parameter
    \param[in]  name: parameter name
    \param[in]  data: parameter raw data
    \param[in]  data_size: raw data size
    \param[out] none
    \retval     none
*/
void cau_parameter_printf(char *name, uint8_t *data, uint32_t data_size)
{
    uint32_t i = 0, count = 0;

    printf("%s:\n", name);
    for(i = 0; i < data_size; i++) {
        printf("0x%02X ", data[i]);
        count++;

        if(16 == count) {
            count = 0;
            printf(" [Block %d] \r\n", i / 16);
        }
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

    return ch;
}
