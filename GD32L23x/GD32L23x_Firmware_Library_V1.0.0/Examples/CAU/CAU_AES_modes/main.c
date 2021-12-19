/*!
    \file    main.c
    \brief   CAU AES modes example

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

#include "main.h"
#include "gd32l233r_eval.h"

uint8_t key_128[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                       0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                      };
uint8_t key_192[24] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                       0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                       0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
                      };
uint8_t key_256[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                       0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                       0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                       0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
                      };

uint8_t *key_select[3] = {key_128, key_192, key_256};
uint16_t keysize[3] = {128, 192, 256};

uint8_t vectors[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                      };

__attribute__((aligned(4)))
uint8_t plaintext[TEXT_SIZE] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

__attribute__((aligned(4)))
uint8_t ciphertext[TEXT_SIZE] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
    0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09,
    0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};
uint8_t encrypt_result[TEXT_SIZE];
uint8_t decrypt_result[TEXT_SIZE];
uint32_t i = 0;

/* printf data by a block of 16 bytes */
static void data_display(uint32_t datalength, uint8_t *data);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    cau_parameter_struct text;
    uint8_t *key_addr;
    uint32_t key_size;

    gd_eval_com_init(EVAL_COM);
    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);

    printf("\r\nplain data :\r\n");
    data_display(TEXT_SIZE, plaintext);

    /* deinitialize CAU */
    cau_deinit();
    cau_struct_para_init(&text);
    /* encryption in ECB mode */
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;

        if(SUCCESS == cau_aes_ecb(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  ECB :\r\n\r\n", key_size);
            data_display(TEXT_SIZE, encrypt_result);
        }
    }

    /* decryption in ECB mode */
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.input     = ciphertext;
        text.in_length = TEXT_SIZE;

        if(SUCCESS == cau_aes_ecb(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  ECB :\r\n\r\n", key_size);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }

    /* encryption in CBC mode */
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;

        if(SUCCESS == cau_aes_cbc(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CBC :\r\n\r\n", key_size);
            data_display(TEXT_SIZE, encrypt_result);
        }
    }

    /* decryption in CBC mode */
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = ciphertext;
        text.in_length = TEXT_SIZE;

        if(SUCCESS == cau_aes_cbc(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CBC :\r\n\r\n", key_size);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }

    /* encryption in CTR mode */
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_ENCRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = plaintext;
        text.in_length = TEXT_SIZE;

        if(SUCCESS == cau_aes_ctr(&text, encrypt_result)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CTR :\r\n\r\n", key_size);
            data_display(TEXT_SIZE, encrypt_result);
        }
    }

    /* decryption in CTR mode */
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++) {
        key_addr = key_select[i];
        key_size = keysize[i];

        text.alg_dir   = CAU_DECRYPT;
        text.key       = key_addr;
        text.key_size  = key_size;
        text.iv        = vectors;
        text.input     = ciphertext;
        text.in_length = TEXT_SIZE;

        if(SUCCESS == cau_aes_ctr(&text, decrypt_result)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CTR :\r\n\r\n", key_size);
            data_display(TEXT_SIZE, decrypt_result);
        }
    }

    while(1) {
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

        if(16 == count) {
            count = 0;
            printf("  [Block %d] \r\n", i / 16);
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
