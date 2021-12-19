/*!
    \file    mbl_qspi_flash.c
    \brief   Non-secure MBL qspi flash file for GD32W51x WiFi SDK

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

#include "gd32w51x.h"
#include "mbl_qspi_flash.h"

/*!
    \brief      configure qspi flash gpio
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_3 | GPIO_PIN_4);
}

/*!
    \brief      enable qspi flash write
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_write_enable(void)
{
    qspi_command_struct sCommand;
    qspi_autopolling_struct sConfig;

    /* Enable write operations ------------------------------------------ */
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = WRITE_ENABLE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    qspi_command(&sCommand);

    /* Configure automatic polling mode to wait for write enabling ---- */
    sConfig.match                = 0x02;
    sConfig.mask                 = 0x02;
    sConfig.matchmode            = QSPI_MATCH_MODE_AND;
    sConfig.statusbytessize      = 1;
    sConfig.interval             = 0x10;
    sConfig.automaticstop        = QSPI_AUTOMATIC_STOP_ENABLE;

    sCommand.instruction         = READ_STATUS_REG1_CMD;
    sCommand.datamode            = QSPI_DATA_1_LINE;

    qspi_autopolling(&sCommand, &sConfig);
}

/*!
    \brief      configure automatic polling mode to wait for memory ready
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_autopolling_ready(void)
{
    qspi_command_struct sCommand;
    qspi_autopolling_struct sConfig;

    /* Configure automatic polling mode to wait for memory ready ------ */
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = READ_STATUS_REG1_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.match                = 0x00;
    sConfig.mask                 = 0x01;
    sConfig.matchmode            = QSPI_MATCH_MODE_AND;
    sConfig.statusbytessize      = 1;
    sConfig.interval             = 0x10;
    sConfig.automaticstop        = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_autopolling(&sCommand, &sConfig);
}

/*!
    \brief      configure qspi flash
    \param[in]  clock_prescaler: prescaler of qspi clock
    \param[out] none
    \retval     none
*/
void qspi_flash_config(uint32_t clock_prescaler)
{
    qspi_init_struct Init;
    qspi_command_struct sCommand;
    uint8_t mode;

    // rcu_periph_clock_enable(RCU_GTZC);
    // tzgpc_tzspc_peripheral_attributes_config(TZGPC_PERIPH_QSPI_FLASHREG, TZGPC_TZSPC_PERIPH_SEC);

    qspi_flash_gpio_config();

    // qspi_deinit();
    rcu_periph_clock_enable(RCU_QSPI);

    Init.prescaler               = clock_prescaler;  /* QSPI clock = AHBCLK/(ClockPrescaler+1) */
    Init.fifothreshold           = 4;
    Init.sampleshift             = QSPI_SAMPLE_SHIFTING_HALFCYCLE;  // QSPI_SAMPLE_SHIFTING_NONE;
    Init.flashsize               = 23;  /* 2^(FlashSize+1) ***** number of address bits = FlashSize + 1*/
    Init.chipselecthightime      = QSPI_CS_HIGH_TIME_1_CYCLE;
    Init.clockmode               = QSPI_CLOCK_MODE_0;
    qspi_init(&Init);

    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = 0;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    qspi_flash_write_enable();

    sCommand.instruction         = READ_STATUS_REG_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 1;
    qspi_command(&sCommand);
    qspi_receive(&mode);

    if (mode & 0x02) {
        /* quad mode, do nothing */
    } else {
        /* enable quad mode */
        mode |= 0x02;
        sCommand.instruction = WRITE_STATUS_REG_CMD;
        sCommand.addressmode = QSPI_ADDRESS_NONE;
        sCommand.address     = 0;
        sCommand.datamode    = QSPI_DATA_1_LINE;
        sCommand.nbdata      = 1;
        sCommand.dummycycles = 0;
        qspi_command(&sCommand);
        qspi_transmit(&mode);
        qspi_flash_autopolling_ready();
    }
}

/*!
    \brief      erase qspi flash sector
    \param[in]  address: qspi flash address
    \param[out] none
    \retval     0
*/
int32_t qspi_flash_sector_erase(uint32_t address)
{
    qspi_command_struct sCommand;

    qspi_flash_write_enable();
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = SECTOR_ERASE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_flash_autopolling_ready();

    return 0;
}

/*!
    \brief      erase qspi flash full chip
    \param[in]  none
    \param[out] none
    \retval     0
*/
int32_t qspi_flash_chip_erase(void)
{
    qspi_command_struct sCommand;

    qspi_flash_write_enable();
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = CHIP_ERASE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_8_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_flash_autopolling_ready();

    return 0;
}

/*!
    \brief      read qspi flash
    \param[in]  adress: flash's internal address to read from
    \param[out] data: pointer to the buffer that receives the data read from the flash
    \param[in]  size: size of data read from flash
    \retval     0
*/
int32_t qspi_flash_read(uint32_t address, void *data, uint32_t size)
{
    qspi_command_struct sCommand;

    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = QUAD_OUT_FAST_READ_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_24_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 8;
    sCommand.datamode            = QSPI_DATA_4_LINES;
    sCommand.nbdata              = size;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_receive(data);

    return 0;
}

/*!
    \brief      program qspi flash page
    \param[in]  adress: flash's internal address to write to
    \param[in]  data: pointer to the buffer that receives the data read from the flash
    \param[in]  size: size of data read from flash
    \param[out] none
    \retval     0
*/
int32_t qspi_flash_page_program(uint32_t address, const uint8_t *data, uint32_t size)
{
    qspi_command_struct sCommand;

    /* Writing Sequence 4 Line------------------------------------------ */
    qspi_flash_write_enable();
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = QUAD_IN_FAST_PROG_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_4_LINES;
    sCommand.nbdata              = size;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_transmit((uint8_t *)data);
    qspi_flash_autopolling_ready();

    return 0;
}
