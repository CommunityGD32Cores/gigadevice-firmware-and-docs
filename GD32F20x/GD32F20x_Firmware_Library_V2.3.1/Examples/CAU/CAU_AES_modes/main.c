/*!
    \file    main.c
    \brief   CAU AES modes example

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
    \version 2021-07-30, V2.3.0, firmware for GD32F20x
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
#include "gd32f20x_eval.h"

uint8_t key_128[16] = {0x2bU, 0x7eU, 0x15U, 0x16U, 0x28U, 0xaeU, 0xd2U, 0xa6U,
                       0xabU, 0xf7U, 0x15U, 0x88U, 0x09U, 0xcfU, 0x4fU, 0x3cU
                      };
uint8_t key_192[24] = {0x8eU, 0x73U, 0xb0U, 0xf7U, 0xdaU, 0x0eU, 0x64U, 0x52U,
                       0xc8U, 0x10U, 0xf3U, 0x2bU, 0x80U, 0x90U, 0x79U, 0xe5U,
                       0x62U, 0xf8U, 0xeaU, 0xd2U, 0x52U, 0x2cU, 0x6bU, 0x7bU
                      };
uint8_t key_256[32] = {0x60U, 0x3dU, 0xebU, 0x10U, 0x15U, 0xcaU, 0x71U, 0xbeU,
                       0x2bU, 0x73U, 0xaeU, 0xf0U, 0x85U, 0x7dU, 0x77U, 0x81U,
                       0x1fU, 0x35U, 0x2cU, 0x07U, 0x3bU, 0x61U, 0x08U, 0xd7U,
                       0x2dU, 0x98U, 0x10U, 0xa3U, 0x09U, 0x14U, 0xdfU, 0xf4U
                      };

uint8_t *key_select[3] = {key_128, key_192, key_256};
uint16_t keysize[3] = {128U, 192U, 256U};

uint8_t vectors[16] = {0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U,
                       0x08U, 0x09U, 0x0aU, 0x0bU, 0x0cU, 0x0dU, 0x0eU, 0x0fU
                      };
uint8_t plaintext[TEXT_SIZE] = {
    0x6bU, 0xc1U, 0xbeU, 0xe2U, 0x2eU, 0x40U, 0x9fU, 0x96U,
    0xe9U, 0x3dU, 0x7eU, 0x11U, 0x73U, 0x93U, 0x17U, 0x2aU,
    0xaeU, 0x2dU, 0x8aU, 0x57U, 0x1eU, 0x03U, 0xacU, 0x9cU,
    0x9eU, 0xb7U, 0x6fU, 0xacU, 0x45U, 0xafU, 0x8eU, 0x51U,
    0x30U, 0xc8U, 0x1cU, 0x46U, 0xa3U, 0x5cU, 0xe4U, 0x11U,
    0xe5U, 0xfbU, 0xc1U, 0x19U, 0x1aU, 0x0aU, 0x52U, 0xefU,
    0xf6U, 0x9fU, 0x24U, 0x45U, 0xdfU, 0x4fU, 0x9bU, 0x17U,
    0xadU, 0x2bU, 0x41U, 0x7bU, 0xe6U, 0x6cU, 0x37U, 0x10U
};
uint8_t ciphertext[TEXT_SIZE] = {
    0x76U, 0x49U, 0xabU, 0xacU, 0x81U, 0x19U, 0xb2U, 0x46U,
    0xceU, 0xe9U, 0x8eU, 0x9bU, 0x12U, 0xe9U, 0x19U, 0x7dU,
    0x50U, 0x86U, 0xcbU, 0x9bU, 0x50U, 0x72U, 0x19U, 0xeeU,
    0x95U, 0xdbU, 0x11U, 0x3aU, 0x91U, 0x76U, 0x78U, 0xb2U,
    0x73U, 0xbeU, 0xd6U, 0xb8U, 0xe3U, 0xc1U, 0x74U, 0x3bU,
    0x71U, 0x16U, 0xe6U, 0x9eU, 0x22U, 0x22U, 0x95U, 0x16U,
    0x3fU, 0xf1U, 0xcaU, 0xa1U, 0x68U, 0x1fU, 0xacU, 0x09U,
    0x12U, 0x0eU, 0xcaU, 0x30U, 0x75U, 0x86U, 0xe1U, 0xa7U
};
uint8_t encrypt_result[TEXT_SIZE];
uint8_t decrypt_result[TEXT_SIZE];
uint32_t i = 0U;

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
    cau_text_struct text;

    gd_eval_com_init(EVAL_COM1);
    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);

    printf("\r\nplain data :\r\n");
    data_display(TEXT_SIZE, plaintext);

    /* encryption in ECB mode */
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    for(i = 0U; i < 3U; i++) {
        if(SUCCESS == cau_aes_ecb(CAU_ENCRYPT, key_select[i], keysize[i], &text)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  ECB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
        }
    }

    /* decryption in ECB mode */
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    text.input = ciphertext;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    for(i = 0U; i < 3U; i++) {
        if(SUCCESS == cau_aes_ecb(CAU_DECRYPT, key_select[i], keysize[i], &text)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  ECB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
        }
    }

    /* encryption in CBC mode */
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    for(i = 0U; i < 3U; i++) {
        if(SUCCESS == cau_aes_cbc(CAU_ENCRYPT, key_select[i], keysize[i], vectors, &text)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CBC :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output = encrypt_result);
        }
    }

    /* decryption in CBC mode */
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    text.input = ciphertext;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    for(i = 0U; i < 3U; i++) {
        cau_deinit();
        if(SUCCESS == cau_aes_cbc(CAU_DECRYPT, key_select[i], keysize[i], vectors, &text)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CBC :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
        }
    }

    /* encryption in CTR mode */
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    for(i = 0U; i < 3U; i++) {
        if(SUCCESS == cau_aes_ctr(CAU_ENCRYPT, key_select[i], keysize[i], vectors, &text)) {
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CTR :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
        }
    }

    /* decryption in CTR mode */
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    text.input = ciphertext;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    for(i = 0U; i < 3U; i++) {
        if(SUCCESS == cau_aes_ctr(CAU_DECRYPT, key_select[i], keysize[i], vectors, &text)) {
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CTR :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
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
    uint32_t i = 0U, count = 0U;

    for(i = 0U; i < datalength; i++) {
        printf("0x%02X ", data[i]);
        count++;

        if(16U == count) {
            count = 0U;
            printf("  [Block %d] \r\n", i / 16);
        }
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
