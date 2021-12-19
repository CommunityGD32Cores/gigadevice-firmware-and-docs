/*!
    \file    main.c
    \brief   CAU demo

    \version 2021-08-15, V1.0.0, demo for GD32L23x
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

#include "gd32l23x.h"
#include <stdio.h>
#include "gd32l233r_eval.h"
#include "main.h"

__attribute__((aligned(4)))
uint8_t encrypt_result[TEXT_SIZE];
__attribute__((aligned(4)))
uint8_t decrypt_result[TEXT_SIZE];
uint16_t algorithm_mode_used = 0;
uint32_t i = 0;

/* DES algorithm ECB mode demo */
static void des_ecb_demo(void);
/* DES algorithm CBC mode demo */
static void des_cbc_demo(void);
/* TDES algorithm ECB mode demo */
static void tdes_ecb_demo(void);
/* TDES algorithm CBC mode demo */
static void tdes_cbc_demo(void);
/* AES algorithm ECB mode demo */
static void aes_ecb_demo(void);
/* AES algorithm CBC mode demo */
static void aes_cbc_demo(void);
/* AES algorithm CTR mode demo */
static void aes_ctr_demo(void);
/* AES algorithm GCM mode demo */
static void aes_gcm_demo(void);
/* AES algorithm CCM mode demo */
static void aes_ccm_demo(void);
/* AES algorithm CFB mode demo */
static void aes_cfb_demo(void);
/* AES algorithm OFB mode demo */
static void aes_ofb_demo(void);
/* printf data by a block of 16 bytes */
static void data_display(uint32_t datalength, uint8_t *data);
/* display algorithm selection */
static void algorithm_choose_display(void);
/* display mode selection */
static void mode_choose_display(void);
/* algorithm selection */
static void algorithm_choose(void);
/* mode selection */
static void mode_choose(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM);

    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);

    while(1) {
        printf("\r\nPlain data :\r\n");
        data_display(TEXT_SIZE, plaintext);

        /* select algorithm */
        algorithm_choose_display();
        algorithm_choose();

        /* select mode */
        mode_choose_display();
        mode_choose();

        /* go to the corresponding encryption/decryption */
        switch(algorithm_mode_used) {
        /* when DES ECB mode is selected */
        case ALGORITHM_USE_DES | MODE_USE_ECB:
            des_ecb_demo();
            break;
        /* when DES CBC mode is selected */
        case ALGORITHM_USE_DES | MODE_USE_CBC:
            des_cbc_demo();
            break;
        /* when TDES ECB mode is selected */
        case ALGORITHM_USE_TDES | MODE_USE_ECB:
            tdes_ecb_demo();
            break;
        /* when TDES CBC mode is selected */
        case ALGORITHM_USE_TDES | MODE_USE_CBC:
            tdes_cbc_demo();
            break;
        /* when AES ECB mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_ECB:
            aes_ecb_demo();
            break;
        /* when AES CBC mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_CBC:
            aes_cbc_demo();
            break;
        /* when AES CTR mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_CTR:
            aes_ctr_demo();
            break;
        /* when AES GCM mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_GCM:
            aes_gcm_demo();
            break;
        /* when AES CCM mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_CCM:
            aes_ccm_demo();
            break;
        /* when AES CFB mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_CFB:
            aes_cfb_demo();
            break;
        /* when AES OFB mode is selected */
        case ALGORITHM_USE_AES | MODE_USE_OFB:
            aes_ofb_demo();
            break;
        default:
            printf("algorithm or mode error! \r\n");
            break;
        }
        printf("\r\n Example restarted...\n ");
        algorithm_mode_used = 0;
    }
}

/*!
    \brief      DES algorithm ECB mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void des_ecb_demo(void)
{
    cau_parameter_struct text;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* encryption in DES ECB mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = des_key;
    text.input     = plaintext;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_des_ecb(&text, encrypt_result)) {
        printf(" \r\n Encrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, encrypt_result);
    }

    /* decryption in DES ECB mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = des_key;
    text.input     = encrypt_result;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_des_ecb(&text, decrypt_result)) {
        printf(" \r\n Decrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, decrypt_result);
    }
}

/*!
    \brief      DES algorithm CBC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void des_cbc_demo(void)
{
    cau_parameter_struct text;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* encryption in DES CBC mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = des_key;
    text.iv        = vectors;
    text.input     = plaintext;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_des_cbc(&text, encrypt_result)) {
        printf(" \r\n Encrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, encrypt_result);
    }

    /* decryption in DES CBC mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = des_key;
    text.iv        = vectors;
    text.input     = encrypt_result;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_des_cbc(&text, decrypt_result)) {
        printf(" \r\n Decrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, decrypt_result);
    }
}

/*!
    \brief      TDES algorithm ECB mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tdes_ecb_demo(void)
{
    cau_parameter_struct text;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* encryption in TDES ECB mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = tdes_key;
    text.input     = plaintext;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_tdes_ecb(&text, encrypt_result)) {
        printf(" \r\n Encrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, encrypt_result);
    }

    /* decryption in TDES ECB mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = tdes_key;
    text.input     = encrypt_result;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_tdes_ecb(&text, decrypt_result)) {
        printf(" \r\n Decrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, decrypt_result);
    }
}

/*!
    \brief      TDES algorithm CBC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tdes_cbc_demo(void)
{
    cau_parameter_struct text;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* encryption in TDES CBC mode */
    text.alg_dir   = CAU_ENCRYPT;
    text.key       = tdes_key;
    text.iv        = vectors;
    text.input     = plaintext;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_tdes_cbc(&text, encrypt_result)) {
        printf(" \r\n Encrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, encrypt_result);
    }

    /* decryption in TDES CBC mode */
    text.alg_dir   = CAU_DECRYPT;
    text.key       = tdes_key;
    text.iv        = vectors;
    text.input     = encrypt_result;
    text.in_length = TEXT_SIZE;
    if(SUCCESS == cau_tdes_cbc(&text, decrypt_result)) {
        printf(" \r\n Decrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, decrypt_result);
    }
}

/*!
    \brief      AES algorithm ECB mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_ecb_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in ECB mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_ecb(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  ECB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, encrypt_result);
        }

        /* decryption in ECB mode */
        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.input     = encrypt_result;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_ecb(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  ECB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }
}

/*!
    \brief      AES algorithm CBC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_cbc_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in CBC mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_cbc(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CBC :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, encrypt_result);
        }

        /* decryption in CBC mode */
        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = encrypt_result;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_cbc(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CBC :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }
}

/*!
    \brief      AES algorithm CTR mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_ctr_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in CTR mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_ctr(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CTR :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, encrypt_result);
        }

        /* decryption in CTR mode */
        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = encrypt_result;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_ctr(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CTR :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }
}

/*!
    \brief      AES algorithm GCM mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_gcm_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;
    uint8_t gcm_tag[GCM_TAG_SIZE];

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* GCM mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in GCM mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir     = CAU_ENCRYPT;
        text.key         = key_addr;
        text.key_size    = key_size;
        text.iv          = vectors;
        text.iv_size     = IV_SIZE;
        text.input       = plaintext;
        text.in_length   = TEXT_SIZE;
        text.aad         = aadmessage;
        text.aad_size    = AAD_SIZE;
        if(SUCCESS == cau_aes_gcm(&text, encrypt_result, gcm_tag)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  GCM :\r\n\r\n", keysize[i]);
            printf("encrypted data:\r\n");
            data_display(TEXT_SIZE, encrypt_result);
            printf("tag:\r\n");
            data_display(GCM_TAG_SIZE, gcm_tag);
        }

        /* decryption in GCM mode */
        text.alg_dir     = CAU_DECRYPT;
        text.key         = key_addr;
        text.key_size    = key_size;
        text.iv          = vectors;
        text.iv_size     = IV_SIZE;
        text.input       = encrypt_result;
        text.in_length   = TEXT_SIZE;
        text.aad         = aadmessage;
        text.aad_size    = AAD_SIZE;
        if(SUCCESS == cau_aes_gcm(&text, decrypt_result, gcm_tag)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  GCM :\r\n\r\n", keysize[i]);
            printf("decrypted data:\r\n");
            data_display(TEXT_SIZE, decrypt_result);
            printf("tag:\r\n");
            data_display(GCM_TAG_SIZE, gcm_tag);
        }
    }
}

/*!
    \brief      AES algorithm CCM mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_ccm_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;
    uint8_t ccm_tag[CCM_TAG_SIZE];
    uint8_t aad_buf[AAD_SIZE + 21];

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* CCM mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in CCM mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir     = CAU_ENCRYPT;
        text.key         = key_addr;
        text.key_size    = key_size;
        text.iv          = ccm_vectors;
        text.iv_size     = CCM_IV_SIZE;
        text.input       = plaintext;
        text.in_length   = TEXT_SIZE;
        text.aad         = aadmessage;
        text.aad_size    = AAD_SIZE;
        if(SUCCESS == cau_aes_ccm(&text, encrypt_result, ccm_tag, CCM_TAG_SIZE, (uint8_t *)aad_buf)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CCM :\r\n\r\n", keysize[i]);
            printf("encrypted data:\r\n");
            data_display(TEXT_SIZE, encrypt_result);
            printf("tag:\r\n");
            data_display(CCM_TAG_SIZE, ccm_tag);
        }

        /* decryption in CCM mode */
        text.alg_dir     = CAU_DECRYPT;
        text.key         = key_addr;
        text.key_size    = key_size;
        text.iv          = ccm_vectors;
        text.iv_size     = CCM_IV_SIZE;
        text.input       = encrypt_result;
        text.in_length   = TEXT_SIZE;
        text.aad         = aadmessage;
        text.aad_size    = AAD_SIZE;
        if(SUCCESS == cau_aes_ccm(&text, decrypt_result, ccm_tag, CCM_TAG_SIZE, (uint8_t *)aad_buf)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CCM :\r\n\r\n", keysize[i]);
            printf("decrypted data:\r\n");
            data_display(TEXT_SIZE, decrypt_result);
            printf("tag:\r\n");
            data_display(CCM_TAG_SIZE, ccm_tag);
        }
    }
}

/*!
    \brief      AES algorithm CFB mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_cfb_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* CFB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in CFB mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.iv_size   = IV_SIZE;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_cfb(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CFB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, encrypt_result);
        }

        /* decryption in CFB mode */
        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.iv_size   = IV_SIZE;
        text.input     = encrypt_result;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_cfb(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CFB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }
}

/*!
    \brief      AES algorithm OFB mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void aes_ofb_demo(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;

    /* reset the CAU peripheral */
    cau_deinit();

    cau_struct_para_init(&text);
    /* OFB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        /* encryption in OFB mode */
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.iv_size   = IV_SIZE;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_ofb(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  OFB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, encrypt_result);
        }

        /* decryption in OFB mode */
        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.iv_size   = IV_SIZE;
        text.input     = encrypt_result;
        text.in_length = TEXT_SIZE;
        if(SUCCESS == cau_aes_ofb(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  OFB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }
}

/*!
    \brief      printf data by a block of 16 bytes
    \param[in]  datalength: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength, uint8_t *data)
{
    uint32_t i = 0, count = 0;

    for(i = 0; i < datalength; i++) {
        printf("0x%02X ", data[i]);
        count++;

        /* display every 16 bytes in a block */
        if(16 == count) {
            count = 0;
            printf("  [Block %d] \r\n", i / 16);
        }
    }
}

/*!
    \brief      display algorithm selection
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void algorithm_choose_display(void)
{
    printf("========Choose CAU algorithm=======\r\n");
    printf("1: DES algorithm \r\n");
    printf("2: TDES algorithm \r\n");
    printf("3: AES algorithm \r\n");
    printf("        \r\n");
}

/*!
    \brief      display mode selection
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mode_choose_display(void)
{
    printf("========Choose CAU mode=======\r\n");
    printf("1: ECB mode \r\n");
    printf("2: CBC mode \r\n");
    printf("3: CTR mode only when choose AES algorithm  \r\n");
    printf("4: GCM mode only when choose AES algorithm  \r\n");
    printf("5: CCM mode only when choose AES algorithm  \r\n");
    printf("6: CFB mode only when choose AES algorithm  \r\n");
    printf("7: OFB mode only when choose AES algorithm  \r\n");
    printf("        \r\n");
}

/*!
    \brief      algorithm selection
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void algorithm_choose(void)
{
    uint8_t temp;
    uint8_t flag;

    flag = 1U;
    do {
        /* wait until an algorithm is chosen */
        while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE)) {
        }
        temp = usart_data_receive(EVAL_COM);
        switch(temp) {
        /* choose '1' to use DES algorithm */
        case '1':
            printf("You choose to use DES algorithm \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_DES;
            break;
        /* choose '2' to use TDES algorithm */
        case '2':
            printf("You choose to use TDES algorithm \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_TDES;
            break;
        /* choose '3' to use AES algorithm */
        case '3':
            printf("You choose to use AES algorithm \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_AES;
            break;
        default:
            printf("Choose error: please choose again! \r\n");
            flag = 0U;
        }
    } while(0U == flag);
}

/*!
    \brief      mode selection
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mode_choose(void)
{
    uint8_t temp;
    uint8_t flag;

    flag = 1U;
    do {
        /* wait until a mode is chosen */
        while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE)) {
        }
        temp = usart_data_receive(EVAL_COM);
        switch(temp) {
        /* choose '1' to use ECB mode */
        case '1':
            printf("You choose to use ECB mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_ECB;
            break;
        /* choose '2' to use CBC mode */
        case '2':
            printf("You choose to use CBC mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_CBC;
            break;
        /* choose '3' to use CTR mode */
        case '3':
            printf("You choose to use CTR mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_CTR;
            break;
        /* choose '4' to use GCM mode */
        case '4':
            printf("You choose to use GCM mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_GCM;
            break;
        /* choose '5' to use CCM mode */
        case '5':
            printf("You choose to use CCM mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_CCM;
            break;
        /* choose '6' to use CFB mode */
        case '6':
            printf("You choose to use CFB mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_CFB;
            break;
        /* choose '7' to use OFB mode */
        case '7':
            printf("You choose to use OFB mode \r\n");
            flag = 1U;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_OFB;
            break;
        default:
            printf("Choose error: please choose again! \r\n");
            flag = 0U;
        }
    } while(0U == flag);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

    return ch;
}
