/*!
    \file    main.c
    \brief   PKCAU ECSDA verification

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

/* ECC verify r */
uint8_t ecc_verify_r[]    = {0x3b, 0x66, 0x19, 0xa7, 0x0c, 0x3c, 0x7a, 0x12, 0xdc, 0xd1, 0x1a, 0x8f, 0xfa, 0x3c, 0x8f, 0xa0, 
                             0x37, 0x49, 0xde, 0x4c, 0x14, 0x89, 0xaf, 0xe9, 0xae, 0x7b, 0x75, 0x9b, 0x1b, 0xe4, 0x0f, 0xd5};

/* ECC verify s */
uint8_t ecc_verify_s[]    = {0x84, 0xaa, 0x9a, 0x05, 0x0a, 0x7f, 0xea, 0xa2, 0x8d, 0x4c, 0xc0, 0x52, 0x33, 0x43, 0x12, 0xde, 
                             0xb4, 0x26, 0xda, 0x4e, 0x7a, 0x5a, 0xc5, 0x9a, 0xa7, 0xc8, 0x85, 0x05, 0xc2, 0x91, 0xd3, 0x94};

/* ECC verify point x */
uint8_t ecc_verify_x[]    = {0x8c, 0x71, 0xd2, 0xd9, 0x73, 0x30, 0x14, 0xf4, 0x8d, 0xf0, 0x47, 0xc6, 0xa5, 0xf8, 0x6a, 0x62,
                             0x58, 0x69, 0xa8, 0x67, 0xa3, 0x0a, 0xa3, 0xa0, 0xd8, 0x9f, 0x6a, 0x58, 0x53, 0x8d, 0x08, 0x80};
/* ECC verify point y */
uint8_t ecc_verify_y[]    = {0x3e, 0x92, 0x50, 0x99, 0x13, 0xab, 0xf2, 0xbd, 0x97, 0x69, 0xdb, 0x0d, 0xe8, 0x5e, 0xf6, 0x32,
                             0x9f, 0xaa, 0xea, 0xea, 0x9a, 0x01, 0x8a, 0x9a, 0x6e, 0x84, 0xd2, 0x8d, 0x62, 0xde, 0xcc, 0x94};
/* hash */
uint8_t ecc_verify_hash[] = {0x46, 0xf8, 0x9d, 0x29, 0xc8, 0x8b, 0x5d, 0x44, 0xda, 0x2b, 0x54, 0x73, 0x58, 0x25, 0xf1, 0xf6, 
                             0x1d, 0x6d, 0x9b, 0x39, 0x40, 0x46, 0x64, 0xa0 ,0x3a, 0x3d, 0x08, 0x5c, 0x45, 0xac, 0xa6, 0xfe};

/* curve modulus p */
static const uint8_t brainpoolp256r1_p[]  = {0xa9, 0xfb, 0x57, 0xdb, 0xa1, 0xee, 0xa9, 0xbc, 0x3e, 0x66, 0x0a, 0x90, 0x9d, 0x83, 
                                             0x8d, 0x72, 0x6e, 0x3b, 0xf6, 0x23, 0xd5, 0x26, 0x20, 0x28, 0x20, 0x13, 0x48, 0x1d, 
                                             0x1f, 0x6e, 0x53, 0x77};
/* coefficient a */
static const uint8_t brainpoolp256r1_a[]  = {0x7d, 0x5a, 0x09, 0x75, 0xfc, 0x2c, 0x30, 0x57, 0xee, 0xf6, 0x75, 0x30, 0x41, 0x7a, 
                                             0xff, 0xe7, 0xfb, 0x80, 0x55, 0xc1, 0x26, 0xdc, 0x5c, 0x6c, 0xe9, 0x4a, 0x4b, 0x44, 
                                             0xf3, 0x30, 0xb5, 0xd9};
/* base point x */
static const uint8_t brainpoolp256r1_gx[] = {0x8b, 0xd2, 0xae, 0xb9, 0xcb, 0x7e, 0x57, 0xcb, 0x2c, 0x4b, 0x48, 0x2f, 0xfc, 0x81, 
                                             0xb7, 0xaf, 0xb9, 0xde, 0x27, 0xe1, 0xe3, 0xbd, 0x23, 0xc2, 0x3a, 0x44, 0x53, 0xbd, 
                                             0x9a, 0xce, 0x32, 0x62};
/* base point y */
static const uint8_t brainpoolp256r1_gy[] = {0x54, 0x7e, 0xf8, 0x35, 0xc3, 0xda, 0xc4, 0xfd, 0x97, 0xf8, 0x46, 0x1a, 0x14, 0x61, 
                                             0x1d, 0xc9, 0xc2, 0x77, 0x45, 0x13, 0x2d, 0xed, 0x8e, 0x54, 0x5c, 0x1d, 0x54, 0xc7, 
                                             0x2f, 0x4, 0x69, 0x97};
/* order n */
static const uint8_t brainpoolp256r1_n[]  = {0xa9, 0xfb, 0x57, 0xdb, 0xa1, 0xee, 0xa9, 0xbc, 0x3e, 0x66, 0x0a, 0x90, 0x9d, 0x83, 
                                             0x8d, 0x71, 0x8c, 0x39, 0x7a, 0xa3, 0xb5, 0x61, 0xa6, 0xf7, 0x90, 0x1e, 0x0e, 0x82, 
                                             0x97, 0x48, 0x56, 0xa7};

/* configure parameters of ECDSA verification */
void pkcau_ecdsa_verify_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t verify_res = 0;
    
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
    /* configure ECDSA verification operation */
    pkcau_ecdsa_verify_config();
    /* wait for PKCAU operation completed */
    while(SET != pkcau_flag_get(PKCAU_INT_FLAG_END));
    /* read results from RAM address */
    pkcau_memread(0x5B0, &verify_res, 1);
    /* clear end flag */
    pkcau_flag_clear(PKCAU_INT_FLAG_END);
    /* if success, verification valid */
    if(!verify_res){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_on(LED2);
    }

    while(1){
    }
}

/*!
    \brief      configure parameters of ECDSA verification
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pkcau_ecdsa_verify_config(void)
{
    /* ECC curve parameter structure */
    pkcau_ec_group_parameter_struct pkcau_curve_group;
    /* hash parameter structure */
    pkcau_hash_parameter_struct pkcau_hash_parameter;
    /* signature parameter structure */
    pkcau_signature_parameter_struct pkcau_signature_parameter;
    /* point parameter structure */
    pkcau_point_parameter_struct pkcau_point_parameter;

    /* initialize the ECC curve parameter, hash parameter, point parameter and signature parameter structure */
    pkcau_ec_group_struct_para_init(&pkcau_curve_group);
    pkcau_hash_struct_para_init(&pkcau_hash_parameter);
    pkcau_point_struct_para_init(&pkcau_point_parameter);
    pkcau_signature_struct_para_init(&pkcau_signature_parameter);
    
    /* initialize the input ECC signature parameters */
    pkcau_signature_parameter.sign_r = (uint8_t *)ecc_verify_r;
    pkcau_signature_parameter.sign_s = (uint8_t *)ecc_verify_s;
    
    /* initialize the input point parameters */
    pkcau_point_parameter.point_x = ecc_verify_x;
    pkcau_point_parameter.point_y = ecc_verify_y;

    /* initialize the input ECC curve parameters */
    pkcau_curve_group.modulus_p     = (uint8_t *)brainpoolp256r1_p;
    pkcau_curve_group.coff_a        = (uint8_t *)brainpoolp256r1_a;
    pkcau_curve_group.a_sign        = 0;
    pkcau_curve_group.base_point_x  = (uint8_t *)brainpoolp256r1_gx;
    pkcau_curve_group.base_point_y  = (uint8_t *)brainpoolp256r1_gy;
    pkcau_curve_group.order_n       = (uint8_t *)brainpoolp256r1_n,
    pkcau_curve_group.modulus_p_len = sizeof(brainpoolp256r1_p);
    pkcau_curve_group.order_n_len   = sizeof(brainpoolp256r1_n);
    
    /* initialize the input hash parameters */
    pkcau_hash_parameter.hash_z     = (uint8_t *)ecc_verify_hash;
    pkcau_hash_parameter.hash_z_len = sizeof(ecc_verify_hash);
    
    /* execute ECDSA verification operation */
    pkcau_ecdsa_verification_operation(&pkcau_point_parameter, &pkcau_hash_parameter, &pkcau_signature_parameter, &pkcau_curve_group);
}
