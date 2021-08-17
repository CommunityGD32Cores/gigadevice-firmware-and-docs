/*!
    \file    main.c
    \brief   CAU DES/TDES modes example

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

/* key size 64 bytes */
uint8_t des_key[8] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6}; 
/* key : 192 bits  */
uint8_t tdes_key[24] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF, 
                        0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10, 
                        0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67};
uint8_t plaintext[DATA_SIZE] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
/* initialization vector */
uint8_t vectors[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
uint8_t encrypt_result[DATA_SIZE];
uint8_t decrypt_result[DATA_SIZE];

/* printf data by a block of 8 bytes */
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
    data_display(DATA_SIZE, plaintext);


    /* encryption in DES ECB mode */
    text.input = plaintext;
    text.in_length = DATA_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_des_ecb(CAU_ENCRYPT, des_key, &text)){
        printf(" \r\n encrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, text.output);      
    }

    /* decryption in DES ECB mode */
    text.input = encrypt_result;
    text.in_length = DATA_SIZE;
    text.output = decrypt_result;
    if(SUCCESS == cau_des_ecb(CAU_DECRYPT, des_key, &text)){
        printf(" \r\n decrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, text.output);
    }
    
    /* encryption in DES CBC mode */
    text.input = plaintext;
    text.in_length = DATA_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_des_cbc(CAU_ENCRYPT, des_key, vectors, &text)){
        printf(" \r\n encrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, text.output);      
    }

    /* decryption in DES CBC mode */
    text.input = encrypt_result;
    text.in_length = DATA_SIZE;
    text.output = decrypt_result;
    if(SUCCESS == cau_des_cbc(CAU_DECRYPT, des_key, vectors, &text)){
        printf(" \r\n decrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, text.output);
    }    

    
    /* encryption in TDES ECB mode */
    text.input = plaintext;
    text.in_length = DATA_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_tdes_ecb(CAU_ENCRYPT, tdes_key, &text)){
        printf(" \r\n encrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, text.output);      
    }

    /* decryption in TDES ECB mode */
    text.input = encrypt_result;
    text.in_length = DATA_SIZE;
    text.output = decrypt_result;
    if(SUCCESS == cau_tdes_ecb(CAU_DECRYPT, tdes_key, &text)){
        printf(" \r\n decrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, text.output);
    }    
    
    /* encryption in TDES CBC mode */
    text.input = plaintext;
    text.in_length = DATA_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_tdes_cbc(CAU_ENCRYPT, tdes_key, vectors, &text)){
        printf(" \r\n encrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, text.output);      
    }

    /* decryption in TDES CBC mode */
    text.input = encrypt_result;
    text.in_length = DATA_SIZE;
    text.output = decrypt_result;
    if(SUCCESS == cau_tdes_cbc(CAU_DECRYPT, tdes_key, vectors, &text)){
        printf(" \r\n decrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, text.output);
    } 
    
    while(1){
    }  
}

/*!
    \brief      printf data by a block of 8 bytes
    \param[in]  datalength: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength, uint8_t *data)
{
    uint32_t i =0, count = 0;

    for(i = 0; i < datalength; i++){    
        printf("0x%02X ", data[i]);
        count++;

        if(8 == count){        
            count = 0;
            printf("  [Block %d] \r\n", i/8);
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
