/*!
    \file    main.c
    \brief   CAU TDESECB mode example

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
    \version 2021-07-30, V2.3.0, firmware for GD32F20x
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

#include "main.h"
#include "gd32f20x_eval.h"

/* 192 bit key */
uint32_t tdes_key[6] = {
    0x40414243U, 0x44454647U,     /* KEY1 */
    0x48494A4BU, 0x4C4D4E4FU,     /* KEY2 */
    0x50515253U, 0x54555657U
};    /* KEY3 */
uint32_t plaintext[DATA_SIZE] = {
    0x20212223U, 0x24252627U,     /* block 0 */
    0x28292a2bU, 0x2c2d2e2fU,     /* block 1 */
    0xFFEEDDCCU, 0xBBAA9988U,     /* block 2 */
    0x77665544U, 0x33221100U,     /* block 3 */
    0x10000000U, 0x20000000U
};    /* block 4 */
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

    while(1) {
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
    dma_init(DMA1, DMA_CH6, &dma_initpara);
    /* DMA1 channel6 mode configuration */
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
    dma_init(DMA1, DMA_CH5, &dma_initpara);
    /* DMA1 channel5 mode configuration */
    dma_circulation_disable(DMA1, DMA_CH5);
    dma_memory_to_memory_disable(DMA1, DMA_CH5);

    /* enable DMA transfer */
    dma_channel_enable(DMA1, DMA_CH5);
    dma_channel_enable(DMA1, DMA_CH6);

    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA1, DMA_CH5, DMA_FLAG_FTF)) {
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
    uint32_t i = 0U, count = 0U;

    for(i = 0U; i < datalength; i++) {
        printf("0x%08X ", data[i]);
        count++;

        if(2U == count) {
            count = 0U;
            printf(" [Block %d] \r\n", (i / 2));
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
