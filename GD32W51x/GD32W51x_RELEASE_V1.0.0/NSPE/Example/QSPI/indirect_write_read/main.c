/*!
    \file    main.c
    \brief   QSPI FLASH example
    
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

#include <stdio.h>
#include "gd32w51x.h"
#include "gd32w515p_eval.h"

#define WRITE_ENABLE_CMD                    0x06
#define PAGE_PROG_CMD                       0x02
#define READ_CMD                            0x03
#define SECTOR_ERASE_CMD                    0x20
#define READ_STATUS_REG1_CMD                0x05
#define READ_STATUS_REG2_CMD                0x35
#define BufferSize   (countof(Tx_Buffer) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))
uint8_t Tx_Buffer[] = "GD32W51x FLASH WRITE TEST!\r\n";
uint8_t Rx_Buffer[BufferSize];

void rcu_config(void);
void gpio_config(void);
void qspi_flash_init(void);
void qspi_send_command(uint32_t instruction, uint32_t address, uint32_t dummyCycles, uint32_t instructionMode, uint32_t addressMode, uint32_t addressSize, uint32_t dataMode);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length);
qspi_command_struct        qspi_cmd;
qspi_autopolling_struct    autopolling_cmd;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM0);

    /* peripheral clock enable */
    rcu_config();
    /* QSPI config */
    qspi_flash_init();

    /* Write enable */   
    qspi_cmd.instructionmode   = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction       = WRITE_ENABLE_CMD;
    qspi_cmd.addressmode       = QSPI_ADDRESS_NONE;
    qspi_cmd.alternatebytemode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_cmd.datamode          = QSPI_DATA_NONE;
    qspi_cmd.dummycycles       = 0;
    qspi_cmd.sioomode          = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&qspi_cmd);

    /* Configure automatic polling mode to wait for write enabling */
    autopolling_cmd.match           = 0x02;
    autopolling_cmd.mask            = 0x02;
    autopolling_cmd.matchmode       = QSPI_MATCH_MODE_AND;
    autopolling_cmd.statusbytessize = 1;
    autopolling_cmd.interval        = 0x10;
    autopolling_cmd.automaticstop   = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_cmd.instruction    = READ_STATUS_REG1_CMD;
    qspi_cmd.datamode       = QSPI_DATA_1_LINE;
    qspi_autopolling(&qspi_cmd, &autopolling_cmd);

    /* erase sector 0-0x1000 */
    qspi_send_command(SECTOR_ERASE_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);

    /* wait busy */
    qspi_cmd.instruction       = READ_STATUS_REG1_CMD;
    qspi_cmd.addressmode       = QSPI_ADDRESS_NONE;
    qspi_cmd.datamode          = QSPI_DATA_1_LINE;

    autopolling_cmd.match           = 0x00;
    autopolling_cmd.mask            = 0x01;
    autopolling_cmd.matchmode       = QSPI_MATCH_MODE_AND;
    autopolling_cmd.statusbytessize = 1;
    autopolling_cmd.interval        = 0x10;
    autopolling_cmd.automaticstop   = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_autopolling(&qspi_cmd, &autopolling_cmd);

     /* Write enable */   
    qspi_cmd.instructionmode   = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction       = WRITE_ENABLE_CMD;
    qspi_cmd.addressmode       = QSPI_ADDRESS_NONE;
    qspi_cmd.alternatebytemode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_cmd.datamode          = QSPI_DATA_NONE;
    qspi_cmd.dummycycles       = 0;
    qspi_cmd.sioomode          = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&qspi_cmd);

    /* Configure automatic polling mode to wait for write enabling */
    autopolling_cmd.match           = 0x02;
    autopolling_cmd.mask            = 0x02;
    autopolling_cmd.matchmode       = QSPI_MATCH_MODE_AND;
    autopolling_cmd.statusbytessize = 1;
    autopolling_cmd.interval        = 0x10;
    autopolling_cmd.automaticstop   = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_cmd.instruction    = READ_STATUS_REG1_CMD;
    qspi_cmd.datamode       = QSPI_DATA_1_LINE;

    qspi_autopolling(&qspi_cmd, &autopolling_cmd);

    /* write data */
    qspi_send_command(PAGE_PROG_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_DTLEN = BufferSize - 1;
    qspi_transmit(Tx_Buffer);

    /* wait busy */
    qspi_cmd.instruction       = READ_STATUS_REG1_CMD;
    qspi_cmd.addressmode       = QSPI_ADDRESS_NONE;
    qspi_cmd.datamode          = QSPI_DATA_1_LINE;

    autopolling_cmd.match           = 0x00;
    autopolling_cmd.mask            = 0x01;
    autopolling_cmd.matchmode       = QSPI_MATCH_MODE_AND;
    autopolling_cmd.statusbytessize = 1;
    autopolling_cmd.interval        = 0x10;
    autopolling_cmd.automaticstop   = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_autopolling(&qspi_cmd, &autopolling_cmd);

    /* read data */
    qspi_send_command(READ_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_DTLEN = BufferSize - 1;
    qspi_receive(Rx_Buffer);

    if(memory_compare(Rx_Buffer, Tx_Buffer, BufferSize)){
        printf("GD32W51x FLASH WRITE TEST SUCCESS!\r\n");
    }
    else{
        printf("GD32W51x FLASH WRITE TEST ERROR!\r\n");
    }
    while(1);
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_QSPI);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint8_t length) 
{
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}

/*!
    \brief      configure the QSPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_init(void)
{
    qspi_init_struct qspi_initstructure;

    /* QSPI GPIO config:SCK/PA9, NSS/PA10, IO0/PA11, IO1/PA12, IO2/PB3, IO3/PB4 */
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3 | GPIO_PIN_4);

    qspi_initstructure.clockmode = QSPI_CLOCK_MODE_0;
    qspi_initstructure.fifothreshold = 4;
    qspi_initstructure.sampleshift = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    qspi_initstructure.chipselecthightime = QSPI_CS_HIGH_TIME_2_CYCLE;
    qspi_initstructure.flashsize = 27;
    qspi_initstructure.prescaler = 4;
    qspi_init(&qspi_initstructure);
}

/*!
    \brief      send QSPI command
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_send_command(uint32_t instruction, uint32_t address, uint32_t dummyCycles, uint32_t instructionMode, uint32_t addressMode, uint32_t addressSize, uint32_t dataMode)
{
    qspi_cmd.instruction = instruction;
    qspi_cmd.instructionmode = instructionMode;
    qspi_cmd.address = address;
    qspi_cmd.addressmode = addressMode;
    qspi_cmd.addresssize = addressSize;
    qspi_cmd.alternatebytes = 0;
    qspi_cmd.alternatebytemode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_cmd.alternatebytessize = QSPI_ALTERNATE_BYTES_8_BITS;
    qspi_cmd.datamode = dataMode;
    qspi_cmd.nbdata = 1;
    qspi_cmd.dummycycles = dummyCycles;
    qspi_cmd.sioomode = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&qspi_cmd);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}
