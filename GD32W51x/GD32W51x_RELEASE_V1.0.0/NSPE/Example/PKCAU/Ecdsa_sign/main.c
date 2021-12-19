/*!
    \file    main.c
    \brief   PKCAU ECSDA sign

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

#include "gd32w51x.h"
#include <string.h>
#include <stdio.h>
#include "gd32w515p_eval.h"

#define DATA_SIZE 14

/* private key d */
uint8_t d[DATA_SIZE]          = {0x59,0xdb,0xa3,0x8d,0x4e,0xc6,0x0b,0x20,0x05,0xde,0xf5,0x20,0x49,0x59};
/* integer d */
uint8_t k[DATA_SIZE]          = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05};
/* hash value */
uint8_t hash[DATA_SIZE]       = {0x22,0xa3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4e,0xc6,0x0b,0x20};
/* expected r */
uint8_t expected_r[DATA_SIZE] = {0x31,0x8f,0xa3,0xef,0xf4,0x42,0x67,0x72,0x50,0xc0,0xd6,0x32,0xbc,0xf9};
/* expected s */
uint8_t expected_s[DATA_SIZE] = {0x21,0xef,0x85,0x13,0xc5,0xa2,0x8f,0x3d,0x1f,0xbd,0x27,0x7a,0x14,0xde};
/* result r */
uint8_t r[DATA_SIZE]          = {0};
/* result s */
uint8_t s[DATA_SIZE]          = {0};

/* elliptic curve parameters */
uint8_t secp112r2_p[]  = {0xdb,0x7c,0x2a,0xbf,0x62,0xe3,0x5e,0x66,0x80,0x76,0xbe,0xad,0x20,0x8b};
uint8_t secp112r2_a[]  = {0x61,0x27,0xc2,0x4c,0x05,0xf3,0x8a,0x0a,0xaa,0xf6,0x5c,0x0e,0xf0,0x2c};
uint8_t secp112r2_b[]  = {0x51,0xde,0xf1,0x81,0x5d,0xb5,0xed,0x74,0xfc,0xc3,0x4c,0x85,0xd7,0x90};
uint8_t secp112r2_gx[] = {0x4b,0xa3,0x0a,0xb5,0xe8,0x92,0xb4,0xe1,0x64,0x9d,0xd0,0x92,0x86,0x43};
uint8_t secp112r2_gy[] = {0xad,0xcd,0x46,0xf5,0x88,0x2e,0x37,0x47,0xde,0xf3,0x6e,0x95,0x6e,0x97};
uint8_t secp112r2_n[]  = {0x36,0xdf,0x0a,0xaf,0xd8,0xb8,0xd7,0x59,0x7c,0xa1,0x05,0x20,0xd0,0x4b};

/* ECC curve parameter structure */
pkcau_ec_group_parameter_struct pkcau_curve_group;

/* configure parameters of ECDSA sign */
void pkcau_ecdsa_sign_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* turn off LED1 and LED2 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    /* enable PKCAU clock */
    rcu_periph_clock_enable(RCU_PKCAU);
    /* reset PKCAU */
    pkcau_deinit();
    /* enable PKCAU */
    pkcau_enable();
    /* wait for PKCAU busy flag to reset */ 
    while(RESET != pkcau_flag_get(PKCAU_FLAG_BUSY));
    /* configure ECDSA sign operation */
    pkcau_ecdsa_sign_config();
    /* wait for PKCAU operation completed */
    while(SET != pkcau_flag_get(PKCAU_FLAG_END));
    /* read results from RAM address */
    pkcau_memread(0x700, r, pkcau_curve_group.modulus_p_len);
    pkcau_memread(0x754, s, pkcau_curve_group.modulus_p_len);
    /* clear end flag */
    pkcau_flag_clear(PKCAU_FLAG_END);
    /* if success, LED1 is on */
    if(memcmp(r, expected_r, DATA_SIZE) || memcmp(s, expected_s, DATA_SIZE)){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_on(LED1);
    }

    while(1){
    }
}

/*!
    \brief      configure parameters of ECDSA sign
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pkcau_ecdsa_sign_config(void)
{
    pkcau_hash_parameter_struct pkcau_hash_parameter;

    /* initialize the ECC curve parameter and hash parameter structure */
    pkcau_ec_group_struct_para_init(&pkcau_curve_group);
    pkcau_hash_struct_para_init(&pkcau_hash_parameter);
    
    /* initialize the input ECC curve parameter */
    pkcau_curve_group.modulus_p     = secp112r2_p;
    pkcau_curve_group.coff_a        = secp112r2_a;
    pkcau_curve_group.a_sign        = 0;
    pkcau_curve_group.base_point_x  = secp112r2_gx;
    pkcau_curve_group.base_point_y  = secp112r2_gy;
    pkcau_curve_group.order_n       = secp112r2_n,
    pkcau_curve_group.modulus_p_len = sizeof(secp112r2_p);
    pkcau_curve_group.order_n_len   = sizeof(secp112r2_n);
    
    /* initialize the input hash parameter */
    pkcau_hash_parameter.hash_z     = hash;
    pkcau_hash_parameter.hash_z_len = DATA_SIZE;
    /* execute ECDSA sign operation */
    pkcau_ecdsa_sign_operation(d, k, &pkcau_hash_parameter, &pkcau_curve_group);
}
