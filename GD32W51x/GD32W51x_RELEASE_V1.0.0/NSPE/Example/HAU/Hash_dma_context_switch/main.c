/*!
    \file    main.c
    \brief   HASH context switch using DMA example

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
#include "main.h"

#define TRANSFER_STEP_SIZE     152U    /* must be multiple of 4 bytes */
#define HAU_FIFO_DEPTH         16U

hau_context_parameter_struct  context_para;
hau_digest_parameter_struct   digest_para;
uint32_t g_hau_nwif = 0, g_total_size = 0;
__IO uint32_t g_dma_size = 0;

/* HASH calculation initialization */
void hash_calculate_sha224_sha256(uint32_t algo);
/* DMA transfer configuration */
void hash_dma_config(uint32_t input, uint32_t size);

/* task 1 HASH digest computation process */
void task_1_process(void);
/* task 2 HASH digest computation insert during task 1 process */
void task_2_insert(void);
/* save context when a higher priority task comes */
void task_context_save(void);
/* task 2 HASH digest computation process */
void task_2_hash_sha256_dma(void);
/* restore context when a higher priority task finishes */
void task_context_restore(void);
/* printf data in words */
static void data_display(uint32_t datalength);

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

    /* task 1 HASH digest computation process */
    task_1_process();

    while(1){
    }
}

/*!
    \brief      task 1 HASH digest computation process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_1_process(void)
{
    uint32_t insert_flag = 1;
    uint32_t t_size = 0, task_1_flag = 0, multi_single = 0;
    uint8_t *t_input = NULL;  
    __IO uint16_t num_last_valid = 0;
    uint32_t meg_len = 0;    

    meg_len = sizeof(message_input_task_1);
    /* number of valid bits in last word of the message */
    num_last_valid = 8U * (uint16_t)(meg_len % 4U);
    
    /* ========================SHA224 calculation======================== */
    /* hash SHA224 initialization */
    hash_calculate_sha224_sha256(HAU_ALGO_SHA224); 

    /* multiple DMA transmission */
    multi_single = MULTIPLE_DMA_NO_DIGEST; 
    while(0 == task_1_flag){
        /* update the input data pointer */
        t_input = message_input_task_1 + g_total_size;
        /* multiple DMA transmission size every time */
        t_size = TRANSFER_STEP_SIZE;
        /* pre-calculate the transmitted size after the DMA transmission of this cycle */
        g_total_size += t_size;
        /* judge if this cycle is the end cycle of multiple DMA transmission */
        if(g_total_size >= sizeof(message_input_task_1)){
            task_1_flag = 1;
            /* calculate the exact rest size for transmission */
            t_size = sizeof(message_input_task_1) + TRANSFER_STEP_SIZE - g_total_size;
            /* the end cycle should set MDS=0 to automatically set CALEN=1 */
            multi_single = SINGLE_DMA_AUTO_DIGEST;
        }

        /* DMA transmission configuration */
        hau_multiple_single_dma_config(multi_single);
        /* configure the number of valid bits in last word of the message */
        hau_last_word_validbits_num_config((uint32_t)num_last_valid);
        hash_dma_config((uint32_t)t_input, t_size);
        
        /* a high priority task 2 is coming */
        if(1 == insert_flag){
            /* switch context and handle task 2 */
            task_2_insert();
            insert_flag = 0;
            /* re-configure the flag to 0 to aviod the interruption happened in the end cycle */
            task_1_flag = 0;
        }else{
            /* wait until the last transfer from OUT FIFO */
            while(!dma_flag_get(DMA1, DMA_CH7, DMA_FLAG_FTF)){
            }
            /* wait until the busy flag is reset */
            while(hau_flag_get(HAU_FLAG_BUSY)){
            }
        }
    }

    /* read HASH SHA224 digest */
    hau_digest_read(&digest_para);
    printf("  \r\n\r\nSHA224 message digest (224 bits):\r\n\r\n");    
    data_display(28/4);
}

/*!
    \brief      HASH calculation initialization
    \param[in]  algo: algorithm selection
      \arg        HAU_ALGO_SHA224
      \arg        HAU_ALGO_SHA256
    \param[out] none
    \retval     none
*/
void hash_calculate_sha224_sha256(uint32_t algo)
{
    hau_init_parameter_struct hau_initpara;

    /* HAU peripheral initialization */
    hau_deinit();
    hau_init_struct_para_init(&hau_initpara);
    hau_initpara.algo = algo;
    hau_initpara.mode = HAU_MODE_HASH;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    hau_init(&hau_initpara);
}

/*!
    \brief      DMA transfer configuration
    \param[in]  input: pointer to data to transmit 
    \param[in]  size: length of the data in bytes
    \param[out] none
    \retval     none
*/
void hash_dma_config(uint32_t input, uint32_t size)
{   
    dma_multi_data_parameter_struct dma_init_parameter;
    
    g_dma_size = 0;
    if(size % 4){
        g_dma_size++;
    }
    g_dma_size += (size/4);

    g_hau_nwif = hau_infifo_words_num_get();

    /* DMA configuration */
    dma_deinit(DMA1, DMA_CH7);    
    dma_multi_data_para_struct_init(&dma_init_parameter);
    dma_init_parameter.periph_addr = (uint32_t)(&HAU_DI);
    dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr = (uint32_t)input;
    dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.periph_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.critical_value = DMA_FIFO_4_WORD;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number = g_dma_size;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA1,DMA_CH7, &dma_init_parameter);

    dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI2);    

    /* enable DMA transfer */
    dma_channel_enable(DMA1, DMA_CH7);
    hau_dma_enable(); 
}

/*!
    \brief      task 2 HASH digest computation insert during task 1 process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_2_insert(void)
{
    task_context_save();
    task_2_hash_sha256_dma();
    task_context_restore();
}    

/*!
    \brief      save context when a higher priority task comes
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_context_save(void)
{
    uint32_t remaining_size = 0;
    uint32_t new_hau_nwif = 0;

    /* wait for BUSY=0 */
    while(SET == hau_flag_get(HAU_FLAG_BUSY));
    /* if CCF=1, no need to save context */
    if(SET == hau_flag_get(HAU_FLAG_CALCULATION_COMPLETE)){
        return;
    }

    /* disable dma transmission */
    dma_channel_disable(DMA1, DMA_CH7);
    dma_flag_clear(DMA1, DMA_CH7, DMA_FLAG_FTF);
    dma_flag_clear(DMA1, DMA_CH7, DMA_FLAG_HTF);
    dma_flag_clear(DMA1, DMA_CH7, DMA_FLAG_FEE);
    dma_flag_clear(DMA1, DMA_CH7, DMA_FLAG_SDE); 
    dma_flag_clear(DMA1, DMA_CH7, DMA_FLAG_TAE);
    hau_dma_disable();

    /* wait for BUSY=0 */
    while(SET == hau_flag_get(HAU_FLAG_BUSY));
    /* if CCF=1, no need to save context */
    if(SET == hau_flag_get(HAU_FLAG_CALCULATION_COMPLETE)){
        return;
    }
    
    /* save interrupt position */
    remaining_size = dma_transfer_number_get(DMA1, DMA_CH7);
    new_hau_nwif = (hau_infifo_words_num_get()+1)% HAU_FIFO_DEPTH;
    /* synchronous HAU and DMA */
    if(new_hau_nwif != ((g_dma_size - remaining_size + g_hau_nwif) % HAU_FIFO_DEPTH)){
        /* error */
        if(0 != remaining_size){
            gd_eval_led_on(LED1);
            while(1);
        }
        remaining_size--;
    }
    g_total_size -= 4*remaining_size;
    /* the length already transferred is multiple of 4 bytes */
    if(0 != (g_total_size%4)){
        g_total_size = (g_total_size/4 + 1) * 4;
    }

    /* save HAU context */
    hau_context_struct_para_init(&context_para);
    hau_context_save(&context_para);
}    

/*!
    \brief      task 2 HASH digest computation process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_2_hash_sha256_dma(void)
{
    __IO uint16_t num_last_valid = 0;
    uint32_t meg_len = 0;
    
    meg_len = sizeof(message_input_task_2);
    /* number of valid bits in last word of the message */
    num_last_valid = 8U * (uint16_t)(meg_len  % 4U);

    /* ========================SHA224 calculation======================== */
    /* hash SHA256 initialization */
    hash_calculate_sha224_sha256(HAU_ALGO_SHA256);

    /* single DMA transmission configuration */
    hau_multiple_single_dma_config(SINGLE_DMA_AUTO_DIGEST);
    /* configure the number of valid bits in last word of the key */
    hau_last_word_validbits_num_config((uint32_t)num_last_valid);
    hash_dma_config((uint32_t)message_input_task_2, meg_len);
    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA1, DMA_CH7, DMA_FLAG_FTF)){
    }
    /* wait until the busy flag is reset */
    while(hau_flag_get(HAU_FLAG_BUSY)){
    }

    hau_digest_read(&digest_para);
    printf("  \r\n\r\nSHA256 message digest (256 bits):\r\n\r\n");    
    data_display(32/4);
}    

/*!
    \brief      restore context when a higher priority task finishes
    \param[in]  none
    \param[out] none
    \retval     none
*/
void task_context_restore(void)
{
    /* restore HAU context */
    hau_context_restore(&context_para);
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
