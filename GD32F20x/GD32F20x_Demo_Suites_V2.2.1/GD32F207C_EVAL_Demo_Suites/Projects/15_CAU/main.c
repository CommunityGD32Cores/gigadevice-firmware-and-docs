/*!
    \file  main.c
    \brief CAU demo

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2018-10-31, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
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
#include "gd32f207c_eval.h"

uint8_t encrypt_result[TEXT_SIZE];
uint8_t decrypt_result[TEXT_SIZE];
uint8_t algorithm_mode_used = 0;
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
/* printf data by a block of 16 bytes */
static void data_display(uint32_t datalength, uint8_t *data);
/* display algorithm choose */
static void algorithm_choose_display(void);
/* display mode choose */
static void mode_choose_display(void);
/* wait till a character is received by the USART */
static void algorithm_choose(void);
/* wait till a character is received by the USART */
static void mode_choose(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{       
    gd_eval_com_init(EVAL_COM0);
  
    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);
  
    while(1){ 
        printf("\r\nPlain data :\r\n");
        data_display(TEXT_SIZE, plaintext);
        
        algorithm_choose_display();
        algorithm_choose();
        
        mode_choose_display();
        mode_choose();
        
        switch(algorithm_mode_used){
            case ALGORITHM_USE_DES | MODE_USE_ECB:
                des_ecb_demo();
                break;
            case ALGORITHM_USE_DES | MODE_USE_CBC:
                des_cbc_demo();
                break;
            case ALGORITHM_USE_TDES | MODE_USE_ECB:
                tdes_ecb_demo();
                break;
            case ALGORITHM_USE_TDES | MODE_USE_CBC:
                tdes_cbc_demo();
                break;
            case ALGORITHM_USE_AES | MODE_USE_ECB:
                aes_ecb_demo();
                break;
            case ALGORITHM_USE_AES | MODE_USE_CBC:
                aes_cbc_demo();
                break;
            case ALGORITHM_USE_AES | MODE_USE_CTR:
                aes_ctr_demo();
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
    cau_text_struct text;
    
    /* encryption in DES ECB mode */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_des_ecb(CAU_ENCRYPT, des_key, &text)){
        printf(" \r\n Encrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);      
    }

    text.input = encrypt_result;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    /* decryption in DES ECB mode */
    if(SUCCESS == cau_des_ecb(CAU_DECRYPT, des_key, &text)){
        printf(" \r\n Decrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);
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
    cau_text_struct text;
    
    /* encryption in DES CBC mode */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_des_cbc(CAU_ENCRYPT, des_key, vectors, &text)){
        printf(" \r\n Encrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);      
    }

    /* decryption in DES CBC mode */
    text.input = encrypt_result;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    if(SUCCESS == cau_des_cbc(CAU_DECRYPT, des_key, vectors, &text)){
        printf(" \r\n Decrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);
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
    cau_text_struct text;
    /* encryption in TDES ECB mode */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_tdes_ecb(CAU_ENCRYPT, tdes_key, &text)){
        printf(" \r\n Encrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);      
    }

    text.input = encrypt_result;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    /* decryption in TDES ECB mode */
    if(SUCCESS == cau_tdes_ecb(CAU_DECRYPT, tdes_key, &text)){
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
    cau_text_struct text;
    /* encryption in TDES CBC mode */
    text.input = plaintext;
    text.in_length = TEXT_SIZE;
    text.output = encrypt_result;
    if(SUCCESS == cau_tdes_cbc(CAU_ENCRYPT, tdes_key, vectors, &text)){
        printf(" \r\n Encrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);      
    }

    text.input = encrypt_result;
    text.in_length = TEXT_SIZE;
    text.output = decrypt_result;
    /* decryption in TDES CBC mode */
    if(SUCCESS == cau_tdes_cbc(CAU_DECRYPT, tdes_key, vectors, &text)){
        printf(" \r\n Decrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(TEXT_SIZE, text.output);
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
    cau_text_struct text;
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){      
        /* encryption in ECB mode */
        text.input = plaintext;
        text.in_length = TEXT_SIZE;
        text.output = encrypt_result;
        if(SUCCESS == cau_aes_ecb(CAU_ENCRYPT, key_select[i], keysize[i], &text)){
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  ECB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
        }        

        text.input = encrypt_result;
        text.in_length = TEXT_SIZE;
        text.output = decrypt_result;
        /* decryption in ECB mode */
        if(SUCCESS == cau_aes_ecb(CAU_DECRYPT, key_select[i], keysize[i], &text)){
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  ECB :\r\n\r\n", keysize[i]);
            data_display(TEXT_SIZE, text.output);
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
    cau_text_struct text;
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        /* encryption in CBC mode */ 
        text.input = plaintext;
        text.in_length = TEXT_SIZE;
        text.output = encrypt_result;        
        if(SUCCESS == cau_aes_cbc(CAU_ENCRYPT, key_select[i], keysize[i], vectors, &text)){            
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CBC :\r\n\r\n",keysize[i] );
            data_display(TEXT_SIZE, text.output);
        }        

        text.input = encrypt_result;
        text.in_length = TEXT_SIZE;
        text.output = decrypt_result;
        /* decryption in CBC mode */          
        if(SUCCESS == cau_aes_cbc(CAU_DECRYPT, key_select[i], keysize[i], vectors, &text)){            
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CBC :\r\n\r\n",keysize[i] );               
            data_display(TEXT_SIZE, text.output);
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
    cau_text_struct text;
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        /* encryption in CTR mode */  
        text.input = plaintext;
        text.in_length = TEXT_SIZE;
        text.output = encrypt_result;        
        if(SUCCESS == cau_aes_ctr(CAU_ENCRYPT, key_select[i], keysize[i], vectors, &text)){            
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CTR :\r\n\r\n",keysize[i] );
            data_display(TEXT_SIZE, text.output);
        }       

        text.input = encrypt_result;
        text.in_length = TEXT_SIZE;
        text.output = decrypt_result;
        /* decryption in CTR mode */      
        if(SUCCESS == cau_aes_ctr(CAU_DECRYPT, key_select[i], keysize[i], vectors, &text)){
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CTR :\r\n\r\n",keysize[i] ); 
            data_display(TEXT_SIZE, text.output);
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
    uint32_t i =0, count = 0;

    for(i = 0; i < datalength; i++){    
        printf("0x%02X ", data[i]);
        count++;

        if(16 == count){        
            count = 0;
            printf("  [Block %d] \r\n", i/16);
        }
    }
}

/*!
    \brief      display algorithm choose
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
    \brief      display mode choose
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
    printf("        \r\n");
}

/*!
    \brief      wait till a character is received by the USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void algorithm_choose(void)
{
    uint8_t temp; 
    uint8_t flag;
    
    flag = TRUE;   
    do{
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE)){
        }
        temp = usart_data_receive(EVAL_COM0);
        switch(temp){
        case '1':
            printf("You choose to use DES algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_DES;
            break;
        case '2':
            printf("You choose to use TDES algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_TDES; 
            break;
        case '3':
            printf("You choose to use AES algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_AES; 
            break;
        default:
            printf("Choose error: please choose again! \r\n");
            flag = FALSE;
        }
    }while(FALSE == flag);
}

/*!
    \brief      wait till a character is received by the USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mode_choose(void)
{
    uint8_t temp; 
    uint8_t flag;
    
    flag = TRUE;   
    do{
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE)){
        }
        temp = usart_data_receive(EVAL_COM0);
        switch(temp){
        case '1':
            printf("You choose to use ECB mode \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_ECB; 
            break;
        case '2':
            printf("You choose to use CBC mode \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_CBC; 
            break;
        case '3':
            printf("You choose to use CTR mode \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_CTR; 
            break;
        default:
            printf("Choose error: please choose again! \r\n");
            flag = FALSE;
        }
    }while(FALSE == flag);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}
