/*!
    \file    main.c
    \brief   PKCAU scalar multiplication

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

#define PMUL_X_SIZE     32
#define PMUL_Y_SIZE     32
#define PMUL_K_SIZE     32
#define PMUL_RES_X_SIZE 32
#define PMUL_RES_Y_SIZE 32

/* point scalar multiplication of normal mode */
/* point x */
uint8_t ec_pmul_x[PMUL_X_SIZE]           = {0x8b, 0xd2, 0xae, 0xb9, 0xcb, 0x7e, 0x57, 0xcb, 0x2c, 0x4b, 0x48, 0x2f, 0xfc, 0x81, 0xb7, 0xaf,
                                            0xb9, 0xde, 0x27, 0xe1, 0xe3, 0xbd, 0x23, 0xc2, 0x3a, 0x44, 0x53, 0xbd, 0x9a, 0xce, 0x32, 0x62};
/* point y */
uint8_t ec_pmul_y[PMUL_Y_SIZE]           = {0x54, 0x7e, 0xf8, 0x35, 0xc3, 0xda, 0xc4, 0xfd, 0x97, 0xf8, 0x46, 0x1a, 0x14, 0x61, 0x1d, 0xc9,
                                            0xc2, 0x77, 0x45, 0x13, 0x2d, 0xed, 0x8e, 0x54, 0x5c, 0x1d, 0x54, 0xc7, 0x2f, 0x04, 0x69, 0x97};
/* scalar multiplier k */
uint8_t ec_pmul_k[PMUL_K_SIZE]           = {0xd1, 0x68, 0xfd, 0xb8, 0x1b, 0x8c, 0xe9, 0xcd, 0xbf, 0xe2, 0xa1, 0xe0, 0x9f, 0x02, 0x73, 0x42,
                                            0xb0, 0x57, 0x03, 0x45, 0x03, 0x4d, 0x94, 0x24, 0x85, 0xf7, 0xab, 0xb6, 0x46, 0x2f, 0x2b, 0x93};
/* result point x */
uint8_t expected_res_x[PMUL_RES_X_SIZE]  = {0x68, 0x67, 0x2b, 0x1a, 0xc1, 0x87, 0xe4, 0x76, 0x83, 0x20, 0xee, 0xbb, 0xb6, 0x8, 0xc8, 0x99,
                                            0x94, 0xd8, 0x8b, 0x77, 0xd2, 0x6f, 0x52, 0xa2, 0xaf, 0x12, 0x79, 0x29, 0xba, 0x56, 0x5, 0x46};
/* result point y */
uint8_t expected_res_y[PMUL_RES_Y_SIZE]  = {0x10, 0x5b, 0x69, 0x3c, 0x14, 0xcb, 0x45, 0x17, 0xa9, 0x89, 0x67, 0x65, 0x6d, 0x1a, 0xaf, 0xa7,
                                            0x1c, 0xa9, 0x5d, 0xa2, 0x7f, 0x7b, 0x44, 0xf9, 0x47, 0x0a, 0x53, 0x79, 0xb8, 0x12, 0x89, 0x0b};
/* curve modulus */
static const uint8_t brainpoolp256r1_p[] = {0xa9, 0xfb, 0x57, 0xdb, 0xa1, 0xee, 0xa9, 0xbc, 0x3e, 0x66, 0x0a, 0x90, 0x9d, 0x83, 0x8d, 0x72,
                                            0x6e, 0x3b, 0xf6, 0x23, 0xd5, 0x26, 0x20, 0x28, 0x20, 0x13, 0x48, 0x1d, 0x1f, 0x6e, 0x53, 0x77};

/* coefficient a */
static const uint8_t brainpoolp256r1_a[] = {0x7d, 0x5a, 0x09, 0x75, 0xfc, 0x2c, 0x30, 0x57, 0xee, 0xf6, 0x75, 0x30, 0x41, 0x7a, 0xff, 0xe7,
                                            0xfb, 0x80, 0x55, 0xc1, 0x26, 0xdc, 0x5c, 0x6c, 0xe9, 0x4a, 0x4b, 0x44, 0xf3, 0x30, 0xb5, 0xd9};

/* configure parameters of scalar multiplication */
void pkcau_scalar_multi_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t ec_pmul_res_x[PMUL_RES_X_SIZE] = {0};
    uint8_t ec_pmul_res_y[PMUL_RES_Y_SIZE] = {0};
    
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
    /* configure scalar multiplication operation */
    pkcau_scalar_multi_config();
    /* wait for PKCAU operation completed */
    while(SET != pkcau_flag_get(PKCAU_INT_FLAG_END));
    /* read results from RAM address */
    pkcau_memread(0x55C, ec_pmul_res_x, PMUL_RES_X_SIZE);
    pkcau_memread(0x5B0, ec_pmul_res_y, PMUL_RES_Y_SIZE);
    /* clear end flag */
    pkcau_flag_clear(PKCAU_INT_FLAG_END);
    /* if success, LED1 is on */
    if(memcmp(ec_pmul_res_x, expected_res_x, PMUL_RES_X_SIZE) || memcmp(ec_pmul_res_y, expected_res_y, PMUL_RES_Y_SIZE)){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_on(LED1);
    }

    while(1){
    }
}

/*!
    \brief      configure parameters of scalar multiplication
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pkcau_scalar_multi_config(void)
{
    /* ECC curve parameter structure */
    pkcau_ec_group_parameter_struct pkcau_curve_group;
    /* point parameter structure */
    pkcau_point_parameter_struct pkcau_point_parameter;

    /* initialize the ECC curve parameter and point parameter structure */
    pkcau_ec_group_struct_para_init(&pkcau_curve_group);
    pkcau_point_struct_para_init(&pkcau_point_parameter);
    
    /* initialize the input ECC curve parameter */
    pkcau_curve_group.modulus_p     = (uint8_t *)brainpoolp256r1_p;
    pkcau_curve_group.coff_a        = (uint8_t *)brainpoolp256r1_a;
    pkcau_curve_group.modulus_p_len = sizeof(brainpoolp256r1_p);
    pkcau_curve_group.a_sign        = 0;
    pkcau_curve_group.multi_k       = ec_pmul_k;
    pkcau_curve_group.k_len         = PMUL_K_SIZE;
    /* initialize the input point parameter */
    pkcau_point_parameter.point_x = ec_pmul_x;
    pkcau_point_parameter.point_y = ec_pmul_y;
    /* execute scalar multiplication operation */
    pkcau_point_mul_operation(&pkcau_point_parameter, &pkcau_curve_group, PKCAU_MODE_ECC_MUL);
}
