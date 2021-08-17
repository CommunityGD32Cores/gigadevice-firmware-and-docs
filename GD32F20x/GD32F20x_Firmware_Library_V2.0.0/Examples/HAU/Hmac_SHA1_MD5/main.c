/*!
    \file  main.c
    \brief Hmac SHA1 MD5 example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "main.h"
#include "gd32f207i_eval.h"

static uint8_t md5_output[16];
static uint8_t sha1_output[20];

/* printf data in bytes */
static void data_display(uint32_t datalength, uint8_t *data);

uint32_t i = 0, meg_len = 0, key_len = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{          
    gd_eval_com_init(EVAL_COM1);
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU); 
     
    meg_len = sizeof(message_input);
    key_len = sizeof(key);
    printf("\r\nmessage to be hashed:\r\n\r\n");
    for(i = 0; i < meg_len ; i++){
        printf("%c", message_input[i]);
    } 
    
    /* HMAC SHA-1 digest computation */
    hau_hmac_sha_1((uint8_t*)key, key_len,(uint8_t *)message_input, meg_len, sha1_output);
    printf(" \r\n\r\nSHA1 message digest (160 bits):\r\n\r\n");
    data_display(20, sha1_output);

    /* HMAC MD5 digest computation */
    hau_hmac_md5((uint8_t*)key, key_len,(uint8_t *)message_input, meg_len, md5_output);
    printf(" \r\n\r\nMD5 Message Digest (128 bits):\r\n\r\n");
    data_display(16, md5_output);
    
    while(1){
    }  
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

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
