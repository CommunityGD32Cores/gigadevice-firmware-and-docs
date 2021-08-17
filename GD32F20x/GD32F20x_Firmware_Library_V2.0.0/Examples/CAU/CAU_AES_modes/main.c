/*!
    \file  main.c
    \brief CAU AES modes example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "main.h"
#include "gd32f207i_eval.h"

uint8_t encrypt_result[TEXT_SIZE];
uint8_t decrypt_result[TEXT_SIZE];                         
uint32_t i = 0;
                         
/* printf data by a block of 16 bytes */
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
    data_display(TEXT_SIZE, plaintext);
  
    /* encryption in ECB mode */
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        if(SUCCESS == cau_aes_ecb(CAU_ENCRYPT, key_select[i], keysize[i], plaintext, TEXT_SIZE, encrypt_result)){            
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  ECB :\r\n\r\n",keysize[i] );
            data_display(TEXT_SIZE, encrypt_result);
        }        
    } 

    /* decryption in ECB mode */
    /* ECB mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        if(SUCCESS == cau_aes_ecb(CAU_DECRYPT, key_select[i], keysize[i], ciphertext, TEXT_SIZE, decrypt_result)){            
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  ECB :\r\n\r\n",keysize[i] );
            data_display(TEXT_SIZE, decrypt_result);
        }        
    }
    
    /* encryption in CBC mode */
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        if(SUCCESS == cau_aes_cbc(CAU_ENCRYPT, key_select[i], keysize[i], vectors, plaintext, TEXT_SIZE, encrypt_result)){            
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CBC :\r\n\r\n",keysize[i] );
            data_display(TEXT_SIZE, encrypt_result);
        }        
    }

    /* decryption in CBC mode */
    /* CBC mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        cau_deinit();          
        if(SUCCESS == cau_aes_cbc(CAU_DECRYPT, key_select[i], keysize[i], vectors, ciphertext, TEXT_SIZE, decrypt_result)){            
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CBC :\r\n\r\n",keysize[i] );               
            data_display(TEXT_SIZE, decrypt_result);
        }

    }        

    /* encryption in CTR mode */
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){
        if(SUCCESS == cau_aes_ctr(CAU_ENCRYPT, key_select[i], keysize[i], vectors, plaintext, TEXT_SIZE, encrypt_result)){            
            printf(" \r\n\r\nEncrypted Data with AES %d  Mode  CTR :\r\n\r\n",keysize[i] );
            data_display(TEXT_SIZE, encrypt_result);
        }        
    }        

    /* decryption in CTR mode */
    /* CTR mode: AES 128 -> AES 192 -> AES 256 */
    for(i = 0; i < 3; i++){         
        if(SUCCESS == cau_aes_ctr(CAU_DECRYPT, key_select[i], keysize[i], vectors, ciphertext, TEXT_SIZE, decrypt_result)){            
            printf(" \r\n\r\nDecrypted Data with AES %d  Mode  CTR :\r\n\r\n",keysize[i] ); 
            data_display(TEXT_SIZE, decrypt_result);
        }
    }
        
    while(1){
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

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
