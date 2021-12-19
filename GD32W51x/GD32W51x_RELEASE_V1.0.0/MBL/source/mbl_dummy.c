/*!
    \file    mbl_dummy.c
    \brief   MBL dummy for GD32W51x WiFi SDK

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

#include "mbl_includes.h"

/* Initial attestation private key in raw format, without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MUST present on the device-
 */
const uint8_t initial_attestation_private_key[] =
{
    0xA9, 0xB4, 0x54, 0xB2, 0x6D, 0x6F, 0x90, 0xA4,
    0xEA, 0x31, 0x19, 0x35, 0x64, 0xCB, 0xA9, 0x1F,
    0xEC, 0x6F, 0x9A, 0x00, 0x2A, 0x7D, 0xC0, 0x50,
    0x4B, 0x92, 0xA1, 0x93, 0x71, 0x34, 0x58, 0x5F
};

/* Initial attestation x-coordinate of the public key in raw format,
 * without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MIGHT be present on the device.
 */
const uint8_t initial_attestation_public_x_key[] =
{
    0x79, 0xEB, 0xA9, 0x0E, 0x8B, 0xF4, 0x50, 0xA6,
    0x75, 0x15, 0x76, 0xAD, 0x45, 0x99, 0xB0, 0x7A,
    0xDF, 0x93, 0x8D, 0xA3, 0xBB, 0x0B, 0xD1, 0x7D,
    0x00, 0x36, 0xED, 0x49, 0xA2, 0xD0, 0xFC, 0x3F
};

/* Initial attestation y-coordinate of the public key in raw format,
 * without any encoding.
 * It belongs to the ECDSA P-256 curve.
 * It MIGHT be present on the device.
 */
const uint8_t initial_attestation_public_y_key[] =
{
    0xBF, 0xCD, 0xFA, 0x89, 0x56, 0xB5, 0x68, 0xBF,
    0xDB, 0x86, 0x73, 0xE6, 0x48, 0xD8, 0xB5, 0x8D,
    0x92, 0x99, 0x55, 0xB1, 0x4A, 0x26, 0xC3, 0x08,
    0x0F, 0x34, 0x11, 0x7D, 0x97, 0x1D, 0x68, 0x64
};

/* Hash (SHA256) of initial attestation public key.
 * Byte string representation of ECC public key according to
 * psa_export_public_key() in interface/include/psa/crypto.h:
 * 0x04 || X_coord || Y_coord
 */
const uint8_t initial_attestation_raw_public_key_hash[] =
{
    0xfa, 0x58, 0x75, 0x5f, 0x65, 0x86, 0x27, 0xce,
    0x54, 0x60, 0xf2, 0x9b, 0x75, 0x29, 0x67, 0x13,
    0x24, 0x8c, 0xae, 0x7a, 0xd9, 0xe2, 0x98, 0x4b,
    0x90, 0x28, 0x0e, 0xfc, 0xbc, 0xb5, 0x02, 0x48
};

const uint8_t implementation_id[] =
{
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
    0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
};

/*!
    \brief      generate boot seed
    \param[in]  none
    \param[out] boot_seed: pointer to boot seed
    \retval     none
*/
void boot_seed_gen(uint8_t *boot_seed)
{
#if 0
    int i;

    for (i = 0; i < BOOT_SEED_SIZE; i += 4) {
        *(uint32_t *)boot_seed = rom_rand();
    }
#else
    int ret = rom_rand(boot_seed, BOOT_SEED_SIZE);
    if (ret < 0)
        rom_trace_ex(ROM_ERR, "Get boot seed failed.\r\n");
#endif
}

/*!
    \brief      generate implementation id
    \param[in]  none
    \param[out] impl_id: pointer to implementation id
    \retval     none
*/
void implement_id_gen(uint8_t *impl_id)
{
    memcpy(impl_id, implementation_id, IMPLEMENTATION_ID_MAX_SIZE);
}

/*!
    \brief      generate instance id
    \param[in]  none
    \param[out] inst_id: pointer to instance id
    \retval     none
*/
void instance_id_gen(uint8_t *inst_id)
{
    memcpy(inst_id, initial_attestation_raw_public_key_hash, INSTANCE_ID_MAX_SIZE);
}
