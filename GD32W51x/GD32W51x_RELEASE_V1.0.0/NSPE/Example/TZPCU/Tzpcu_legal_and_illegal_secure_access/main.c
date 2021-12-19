/*!
    \file    main.c
    \brief   TZPCU legal/illegal secure access, secure code with TZEN = 1

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
#include <stdio.h>
#include "gd32w515p_eval.h"

#define SRAM1_ADDR_S        SRAM1_BASE_S        /* SRAM1 secure address */
#define SRAM1_ADDR_NS       SRAM1_BASE_NS       /* SRAM1 non-secure address */

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t temp;

    gd_eval_com_init(EVAL_COM0);
    rcu_periph_clock_enable(RCU_TZPCU);

    /* configure SRAM1 block 0 to secure */
    tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, 0, BLOCK_SECURE_ACCESS_MODE_SEC);
    printf("the first word of SRAM1 value: 0x%08x\r\n\r\n", *(uint32_t*)SRAM1_ADDR_S);

    printf("==========TZSPC legal/illegal secure access example=========\n");
    printf("\n==========TZSPC legal/illegal secure access memory example=========\n");

    /* legal secure write access to SRAM1 region when this region is secure */
    *(uint32_t*)SRAM1_ADDR_S = 0xFFAABBCC;
    printf("legal secure write access complete, set the first word of SRAM1 value to 0xFFAABBCC. \r\n\r\n");

    /* illegal non-secure write access to SRAM1 region when this region is secure */
    *((uint32_t*)SRAM1_BASE_NS) = 0x11223344;
    printf("illegal non-secure write access complete, set the first word of SRAM1 value to 0x11223344. \r\n\r\n");

    /* legal secure read access to SRAM1 region when this region is secure */
    temp = *(uint32_t*)SRAM1_ADDR_S;
    printf("legal secure read the first word of SRAM1 value: 0x%08x\n", temp);

    /* illegal non-secure read access to SRAM1 region when this region is secure */
    temp = *(uint32_t*)SRAM1_ADDR_NS;
    printf("illegal non-secure read the first word of SRAM1 value: 0x%08x\n", temp);

    /* configure SRAM1 block 0 to non-secure */
    tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, 0, BLOCK_SECURE_ACCESS_MODE_NSEC);

    /* illegal secure write access to SRAM1 region when this region is secure */
    *((uint32_t*)SRAM1_ADDR_S) = 0xFFAABBCC;
    printf("illegal secure write access complete, set the first word of SRAM1 value to 0xFFAABBCC. \r\n\r\n");

    /* legal non-secure write access to SRAM1 region when this region is secure */
    *(uint32_t*)SRAM1_ADDR_NS = 0x11223344;
    printf("legal non secure write access complete, set the first word of SRAM1 value to 0x11223344. \r\n\r\n");

    /* illegal secure read access to SRAM1 region when this region is non-secure */
    temp = *(uint32_t*)SRAM1_ADDR_S;
    printf("illegal secure read sram1 value: 0x%08x\n", temp);

    /* legal non-secure read access to SRAM1 region when this region is non-secure */
    temp = *(uint32_t*)SRAM1_ADDR_NS;
    printf("legal read sram1 value: 0x%08x\n", temp);

    while(1){
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}