/*!
    \file    main.h
    \brief   the header of main

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
#include "stdio.h"

#ifndef MAIN_H
#define MAIN_H

/* plain text size in bytes, it must be a multiple of 16(128 bits = 16 bytes) for AES algorithm */
#define TEXT_SIZE                   64
#define IV_SIZE                     16
#define AAD_SIZE                    16
#define GCM_TAG_SIZE                16
#define CCM_IV_SIZE                 8
#define CCM_TAG_SIZE                6
#define ECB                         1
#define CBC                         2
#define CTR                         3
#define DES                         "DES"
#define TDES                        "TDES"
#define AES128                      "AES128"
#define AES192                      "AES192"
#define AES256                      "AES256"

#define ALGORITHM_USE_DES           0x01
#define ALGORITHM_USE_TDES          0x02
#define ALGORITHM_USE_AES           0x04
#define MODE_USE_ECB                0x10
#define MODE_USE_CBC                0x20
#define MODE_USE_CTR                0x40
#define MODE_USE_GCM                0x80
#define MODE_USE_CCM                0x100
#define MODE_USE_CFB                0x200
#define MODE_USE_OFB                0x400

/* key with 128 bits */
uint8_t aes_key_128[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                           0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                          };

/* key with 192 bits */
uint8_t aes_key_192[24] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                           0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                           0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
                          };

/* key with 256 bits */
uint8_t aes_key_256[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                           0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                           0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                           0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
                          };

/* key selection */
uint8_t *key_select[3] = {aes_key_128, aes_key_192, aes_key_256};
/* key size selection */
uint16_t keysize[3] = {128, 192, 256};

/* key with 64 bits for DES */
uint8_t des_key[8] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6};

/* key with 192 bits for TDES */
uint8_t tdes_key[24] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
                        0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67
                       };
/* vectors with 128 bits */
uint8_t vectors[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                      };

/* vectors for CCM mode */
uint8_t ccm_vectors[CCM_IV_SIZE] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

/* message to be encrypted */
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

/* AAD message for GCM and CCM mode */
uint8_t aadmessage[AAD_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                               };

#endif /* MAIN_H */
