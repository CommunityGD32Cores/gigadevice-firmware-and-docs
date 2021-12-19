/*!
    \file    main.c
    \brief   HASH SHA224 SHA256 dma example

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

#include "stdio.h"
#include "gd32w515p_eval.h"

__attribute__((aligned(4)))
const static uint8_t message_input[]=
              {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
               0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
               0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
               0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
               0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
               0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
               0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
               0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
               0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
               0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
               0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
               0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
               0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
               0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
               0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
               0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
               0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
               0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
               0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
               0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
               0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
               0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
               0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
               0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
               0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
               0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
               0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
               0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
               0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
               0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
               0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
               };    

/* hash SHA224/SHA256 digest computation */
static void hash_sha224_sha256_dma(uint32_t algo);
/* printf data in words */
static void data_display(uint32_t datalength);

uint32_t i = 0, len = 0;
hau_digest_parameter_struct   digest_para;

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
    /* Enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);

    len = sizeof(message_input);  
    printf("\r\nmessage to be hashed:\r\n\r\n");
    for(i = 0; i < len ; i++){
        printf("%c", message_input[i]);
    }

    /* SHA-224 digest computation */
    hash_sha224_sha256_dma(HAU_ALGO_SHA224);
    printf("  \r\n\r\nSHA224 message digest (224 bits):\r\n\r\n");
    data_display(28/4);

    /* SHA-256 digest computation */
    hash_sha224_sha256_dma(HAU_ALGO_SHA256);
    printf("  \r\n\r\nSHA256 message digest (256 bits):\r\n\r\n");
    data_display(32/4);

    while(1){
    }
}

/*!
    \brief      hash SHA224/SHA256 digest computation
    \param[in]  algo: algorithm direction
      \arg        HAU_ALGO_SHA224
      \arg        HAU_ALGO_SHA256
    \param[out] none
    \retval     none
*/
static void hash_sha224_sha256_dma(uint32_t algo)
{
    dma_multi_data_parameter_struct dma_init_parameter;
    hau_init_parameter_struct hau_initpara;
    __IO uint16_t num_last_valid = 0;
    __IO uint32_t meg_burst = 0;  

    /* HAU configuration */
    hau_deinit();
    hau_init_struct_para_init(&hau_initpara);    
    hau_initpara.algo = algo;
    hau_initpara.mode = HAU_MODE_HASH;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    hau_init(&hau_initpara);

    if(len % 4){
        meg_burst++;
    }
    meg_burst += (len/4);
    num_last_valid = 8 * (len % 4);
    /* configure the number of valid bits in last word of the message */
    hau_last_word_validbits_num_config(num_last_valid);
      
    /* DMA configuration */
    dma_deinit(DMA1, DMA_CH7);
    dma_multi_data_para_struct_init(&dma_init_parameter);
    dma_init_parameter.periph_addr = (uint32_t)(&HAU_DI);
    dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr = (uint32_t)message_input;
    dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.periph_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.critical_value = DMA_FIFO_4_WORD;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number = meg_burst;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA1,DMA_CH7, &dma_init_parameter);

    dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI2);
    
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH7);
    
    hau_dma_enable();

    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA1, DMA_CH7, DMA_FLAG_FTF)){
    }

    /* wait until the busy flag is reset */
    while(hau_flag_get(HAU_FLAG_BUSY)){
    }
    
    hau_digest_read(&digest_para);
}

/*!
    \brief      printf data in words
    \param[in]  datalength: length of the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength)
{
    uint32_t j = 0;

    for(j = 0; j < datalength; j++){    
        printf("0x%x    \r\n", digest_para.out[j]);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}
