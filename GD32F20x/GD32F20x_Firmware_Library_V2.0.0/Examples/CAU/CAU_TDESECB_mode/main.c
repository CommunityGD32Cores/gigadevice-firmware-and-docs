/*!
    \file  main.c
    \brief CAU TDESECB mode example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/  

#include "main.h"
#include "gd32f207i_eval.h"

uint32_t encrypt_result[DATA_SIZE];
uint32_t decrypt_result[DATA_SIZE];

/* encrypt/decrypt data using TDES */
static void tdes_dma(uint32_t mode, uint32_t *source, uint32_t *dest);
/* printf data by a block of 8 bytes */
static void data_display(uint32_t datalength, uint32_t *data);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    gd_eval_com_init(EVAL_COM1);

    printf(" plain data :\r\n");
    data_display(DATA_SIZE, plaintext);

    /* encrypt */
    tdes_dma(CAU_ENCRYPT, plaintext, encrypt_result);
    printf(" \r\nTDES encrypted data:\r\n\r\n"); 
    data_display(DATA_SIZE, encrypt_result);  

    /* decrypt */
    tdes_dma(CAU_DECRYPT, encrypt_result, decrypt_result); 
    printf(" \r\nTDES decrypted data:\r\n\r\n"); 
    data_display(DATA_SIZE, decrypt_result);

    while(1){
    }
}

/*!
    \brief      encrypt/decrypt data using TDES
    \param[in]  mode: algorithm direction
      \arg        CAU_ENCRYPT: encrypt
      \arg        CAU_DECRYPT: decrypt
    \param[in]  source: pointer to the source address
    \param[in]  dest: pointer to the dest address
    \param[out] none
    \retval     none
*/
static void tdes_dma(uint32_t mode, uint32_t *source, uint32_t *dest)
{
    cau_key_parameter_struct key_initpara;
    dma_parameter_struct dma_initpara;

    /* enable CAU clock */
    rcu_periph_clock_enable(RCU_CAU);
    /* Enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);

    cau_fifo_flush(); 
  
    /* key structure initialization */
    cau_key_parameter_init(&key_initpara);
    key_initpara.key_1_high = tdes_key[0];
    key_initpara.key_1_low  = tdes_key[1]; 
    key_initpara.key_2_high = tdes_key[2];
    key_initpara.key_2_low  = tdes_key[3];
    key_initpara.key_3_high = tdes_key[4];
    key_initpara.key_3_low  = tdes_key[5];
    /* key initialization */
    cau_key_init(&key_initpara);

    /* initialize the CAU peripheral */
    cau_init(mode, CAU_MODE_TDES_ECB, CAU_SWAPPING_32BIT);       
    cau_enable();     

    /* enable the CAU DMA interface */
    cau_dma_enable(CAU_DMA_INFIFO | CAU_DMA_OUTFIFO);

    /* DMA configuration */
    dma_deinit(DMA1, DMA_CH5);
    dma_deinit(DMA1, DMA_CH6);
    /* DMA1 channel6 for CAU_IN configuration */
    dma_initpara.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_initpara.memory_addr = (uint32_t)source;
    dma_initpara.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_initpara.number = DATA_SIZE;
    dma_initpara.periph_addr = CAU_DI_REG_ADDR;
    dma_initpara.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_initpara.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA1, DMA_CH6, dma_initpara);
    /* DMA1 channel5 mode configuration */
    dma_circulation_disable(DMA1, DMA_CH6);
    dma_memory_to_memory_disable(DMA1, DMA_CH6);

    /* DMA1 channel5 for CAU_OUT configuration */
    dma_initpara.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_initpara.memory_addr = (uint32_t)dest;
    dma_initpara.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_initpara.number = DATA_SIZE;
    dma_initpara.periph_addr = CAU_DO_REG_ADDR;
    dma_initpara.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_initpara.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA1, DMA_CH5, dma_initpara);
    /* DMA1 channel5 mode configuration */
    dma_circulation_disable(DMA1, DMA_CH5);
    dma_memory_to_memory_disable(DMA1, DMA_CH5);
   
    /* enable DMA transfer */
    dma_channel_enable(DMA1, DMA_CH5);
    dma_channel_enable(DMA1, DMA_CH6);

    /* wait until the last transfer from OUT FIFO */
    while (!dma_flag_get(DMA1, DMA_CH5, DMA_FLAG_FTF)){
    }

    dma_flag_clear(DMA1, DMA_CH5, DMA_FLAG_G);
    dma_flag_clear(DMA1, DMA_CH6, DMA_FLAG_G);

    cau_disable();
    cau_dma_disable(CAU_DMA_INFIFO | CAU_DMA_OUTFIFO);
    dma_channel_disable(DMA1, DMA_CH5);
    dma_channel_disable(DMA1, DMA_CH6);
}

/*!
    \brief      printf data by a block of 8 bytes
    \param[in]  datalength: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
static void data_display(uint32_t datalength, uint32_t *data)
{
    uint32_t i =0, count = 0;

    for(i = 0; i < datalength; i++){    
        printf("0x%08X ", data[i]);
        count++;

        if(2 == count){        
            count = 0;
            printf("  [Block %d] \r\n", (i/2));
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
