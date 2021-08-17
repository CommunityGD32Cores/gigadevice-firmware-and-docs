/*!
    \file  main.c
    \brief SHA224 SHA256 dma example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "main.h"
#include "gd32f207i_eval.h"

static uint8_t message_input[]= 
"The GD32 F2 series is the result of a perfect symbiosis of the real-time control capabilities of \
an MCU and the signal processing performance of a DSP, and thus complements the GD32 portfolio with a new class of devices, digital signal controllers (DSC).";


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
    gd_eval_com_init(EVAL_COM1);
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU);
    /* Enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);

    len = sizeof(message_input) - 1;  
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
    dma_parameter_struct dma_initpara;
    hau_init_parameter_struct hau_initpara;
    __IO uint16_t num_last_valid = 0;
    __IO uint32_t meg_burst = 0;  

    /* HAU configuration */  
    hau_deinit();
    hau_initpara.algo = algo;
    hau_initpara.mode = HAU_MODE_HASH;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    if(HAU_ALGO_SHA256 == algo){
        hau_initpara.keytype = HAU_KEY_SHORTER_64;    
    }
    hau_init(&hau_initpara);

    if(len % 4){
        meg_burst++;
    }
    meg_burst += (len/4);
    num_last_valid = 8 * (len % 4);
    /* configure the number of valid bits in last word of the message */
    hau_last_word_validbits_num_config(num_last_valid);    
      

    /* DMA configuration */
    dma_deinit(DMA1, DMA_CH6);
    /* DMA1 channel6 for CAU_IN configuration */
    dma_initpara.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_initpara.memory_addr = (uint32_t)message_input;
    dma_initpara.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_initpara.number = meg_burst;
    dma_initpara.periph_addr = HAU_DI_REG_ADDR;
    dma_initpara.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_initpara.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA1, DMA_CH6, dma_initpara);
    /* DMA1 channel 5 mode configuration */
    dma_circulation_disable(DMA1, DMA_CH6);
    dma_memory_to_memory_disable(DMA1, DMA_CH6);

    dma_interrupt_enable(DMA1, DMA_CH6, DMA_INT_FTF);
    hau_dma_enable();
    /* enable DMA transfer */
    dma_channel_enable(DMA1, DMA_CH6);

    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA1, DMA_CH6, DMA_FLAG_FTF)){
    }

    /* wait until the busy flag is reset */
    while(!hau_flag_get(HAU_FLAG_BUSY)){
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
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
