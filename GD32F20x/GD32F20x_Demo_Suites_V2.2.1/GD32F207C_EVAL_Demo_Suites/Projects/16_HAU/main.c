/*!
    \file  main.c
    \brief HAU demo

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

static uint8_t md5_output[16];
static uint8_t sha1_output[20];
static uint8_t sha224_output[28];
static uint8_t sha256_output[32];
uint32_t i = 0, len = 0, key_len = 0;
uint8_t algorithm_mode_used = 0;

/* SHA-1 algorithm HASH mode demo */
void sha1_hash_demo(void);
/* SHA-224 algorithm HASH mode demo */
void sha224_hash_demo(void);
/* SHA-256 algorithm HASH mode demo */
void sha256_hash_demo(void);
/* MD5 algorithm HASH mode demo */
void md5_hash_demo(void);
/* SHA-1 algorithm HMAC mode demo */
void sha1_hmac_demo(void);
/* SHA-224 algorithm HMAC mode demo */
void sha224_hmac_demo(void);
/* SHA-256 algorithm HMAC mode demo */
void sha256_hmac_demo(void);
/* MD5 algorithm HMAC mode demo */
void md5_hmac_demo(void);
/* printf data in bytes */
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
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU);
  
    len = sizeof(message_input) - 1;
    key_len = sizeof(key) - 1;
  
    while(1){
        printf("\r\nmessage to be hashed:\r\n\r\n");
        for(i = 0; i < len ; i++){
            printf("%c", message_input[i]);
        }
      
        algorithm_choose_display();
        algorithm_choose();
        
        mode_choose_display();
        mode_choose();
        
        switch(algorithm_mode_used){
            case ALGORITHM_USE_SHA1 | MODE_USE_HASH:
                sha1_hash_demo();
                break;
            case ALGORITHM_USE_SHA224 | MODE_USE_HASH:
                sha224_hash_demo();
                break;
            case ALGORITHM_USE_SHA256 | MODE_USE_HASH:
                sha256_hash_demo();
                break;
            case ALGORITHM_USE_MD5 | MODE_USE_HASH:
                md5_hash_demo();
                break;
            case ALGORITHM_USE_SHA1 | MODE_USE_HMAC:
                sha1_hmac_demo();
                break;
            case ALGORITHM_USE_SHA224 | MODE_USE_HMAC:
                sha224_hmac_demo();
                break;
            case ALGORITHM_USE_SHA256 | MODE_USE_HMAC:
                sha256_hmac_demo();
                break;
            case ALGORITHM_USE_MD5 | MODE_USE_HMAC:
                md5_hmac_demo();
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
    \brief      SHA-1 algorithm HASH mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sha1_hash_demo(void)
{
    /* SHA-1 digest computation */
    hau_hash_sha_1((uint8_t *)message_input, len, sha1_output);  

    printf("  \r\n\r\nmessage digest with SHA-1  Mode  HASH (160 bits):\r\n\r\n");
    data_display(20, sha1_output);
}

/*!
    \brief      SHA-224 algorithm HASH mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sha224_hash_demo(void)
{
    /* SHA-224 digest computation */
    hau_hash_sha_224((uint8_t *)message_input, len, sha224_output);

    printf("  \r\n\r\nmessage digest with SHA-224  Mode  HASH (224 bits):\r\n\r\n");
    data_display(28, sha224_output);
} 

/*!
    \brief      SHA-256 algorithm HASH mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sha256_hash_demo(void)
{
    /* SHA-256 digest computation */
    hau_hash_sha_256((uint8_t *)message_input, len, sha256_output);

    printf("  \r\n\r\nmessage digest with SHA-256  Mode  HASH (256 bits):\r\n\r\n");
    data_display(32, sha256_output);
}

/*!
    \brief      MD5 algorithm HASH mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void md5_hash_demo(void)
{
    /* MD5 digest computation */
    hau_hash_md5((uint8_t *)message_input, len, md5_output);

    printf("  \r\n\r\nmessage digest with MD5  Mode  HASH (128 bits):\r\n\r\n");
    data_display(16, md5_output);
}

/*!
    \brief      SHA-1 algorithm HMAC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sha1_hmac_demo(void)
{
    /* SHA-1 digest computation */
    hau_hmac_sha_1((uint8_t*)key, key_len, (uint8_t *)message_input, len, sha1_output);            

    printf("  \r\n\r\nmessage digest with SHA-1  Mode  HMAC (160 bits):\r\n\r\n");
    data_display(20, sha1_output);
}

/*!
    \brief      SHA-224 algorithm HMAC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sha224_hmac_demo(void)
{
    /* SHA-224 digest computation */
    hau_hmac_sha_224((uint8_t*)key, key_len, (uint8_t *)message_input, len, sha224_output);  

    printf("  \r\n\r\nmessage digest with SHA-224  Mode  HMAC (224 bits):\r\n\r\n");
    data_display(28, sha224_output);
}

/*!
    \brief      SHA-256 algorithm HMAC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sha256_hmac_demo(void)
{
    /* SHA-256 digest computation */
    hau_hmac_sha_256((uint8_t*)key, key_len, (uint8_t *)message_input, len, sha256_output);  

    printf("  \r\n\r\nmessage digest with SHA-256  Mode  HMAC (256 bits):\r\n\r\n");
    data_display(32, sha256_output);
}

/*!
    \brief      MD5 algorithm HMAC mode demo
    \param[in]  none
    \param[out] none
    \retval     none
*/
void md5_hmac_demo(void)
{
    /* MD5 digest computation */
    hau_hmac_md5((uint8_t*)key, key_len, (uint8_t *)message_input, len, md5_output);  
         
    printf("  \r\n\r\nmessage digest with MD5  Mode  HMAC (128 bits):\r\n\r\n");
    data_display(16, md5_output);
} 

/*!
    \brief      printf data in bytes
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

        if(4 == count){        
            count = 0;
            printf("\r\n");
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
    printf("========Choose HAU algorithm=======\r\n");
    printf("1: SHA1 algorithm \r\n");
    printf("2: SHA224 algorithm \r\n");
    printf("3: SHA256 algorithm \r\n");    
    printf("4: MD5 algorithm \r\n");  
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
    printf("========Choose HAU mode=======\r\n");
    printf("1: HASH mode \r\n");
    printf("2: HMAC mode \r\n");
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
            printf("You choose to use SHA1 algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_SHA1;
            break;
        case '2':
            printf("You choose to use SHA224 algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_SHA224; 
            break;
        case '3':
            printf("You choose to use SHA256 algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_SHA256; 
            break;
         case '4':
            printf("You choose to use MD5 algorithm \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | ALGORITHM_USE_MD5; 
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
            printf("You choose to use HASH mode \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_HASH; 
            break;
        case '2':
            printf("You choose to use HMAC mode \r\n");
            flag = TRUE;
            algorithm_mode_used = algorithm_mode_used | MODE_USE_HMAC; 
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
