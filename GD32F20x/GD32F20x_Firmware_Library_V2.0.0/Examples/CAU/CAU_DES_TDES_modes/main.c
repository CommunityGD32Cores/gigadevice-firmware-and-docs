/*!
    \file  main.c
    \brief CAU DES/TDES modes example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "main.h"
#include "gd32f207i_eval.h"

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
    gd_eval_com_init(EVAL_COM1);
    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);
               
    printf("\r\nplain data :\r\n");
    data_display(DATA_SIZE, plaintext);


    /* encryption in DES ECB mode */
    if(SUCCESS == cau_des_ecb(CAU_ENCRYPT, des_key, plaintext, DATA_SIZE, encrypt_result)){
        printf(" \r\n encrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);      
    }

    /* decryption in DES ECB mode */
    if(SUCCESS == cau_des_ecb(CAU_DECRYPT, des_key, encrypt_result, DATA_SIZE, decrypt_result)){
        printf(" \r\n decrypted data with DES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
    }
    
    /* encryption in DES CBC mode */
    if(SUCCESS == cau_des_cbc(CAU_ENCRYPT, des_key, vectors, plaintext, DATA_SIZE, encrypt_result)){
        printf(" \r\n encrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);      
    }

    /* decryption in DES CBC mode */
    if(SUCCESS == cau_des_cbc(CAU_DECRYPT, des_key, vectors, encrypt_result, DATA_SIZE, decrypt_result)){
        printf(" \r\n decrypted data with DES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
    }    

    
    /* encryption in TDES ECB mode */
    if(SUCCESS == cau_tdes_ecb(CAU_ENCRYPT, tdes_key, plaintext, DATA_SIZE, encrypt_result)){
        printf(" \r\n encrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);      
    }

    /* decryption in TDES ECB mode */
    if(SUCCESS == cau_tdes_ecb(CAU_DECRYPT, tdes_key, encrypt_result, DATA_SIZE, decrypt_result)){
        printf(" \r\n decrypted data with TDES  Mode  ECB :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
    }    
    
    /* encryption in TDES CBC mode */
    if(SUCCESS == cau_tdes_cbc(CAU_ENCRYPT, tdes_key, vectors, plaintext, DATA_SIZE, encrypt_result)){
        printf(" \r\n encrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, encrypt_result);      
    }

    /* decryption in TDES CBC mode */
    if(SUCCESS == cau_tdes_cbc(CAU_DECRYPT, tdes_key, vectors, encrypt_result, DATA_SIZE, decrypt_result)){
        printf(" \r\n decrypted data with TDES  Mode  CBC :\r\n\r\n");
        data_display(DATA_SIZE, decrypt_result);
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
