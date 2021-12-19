/*!
    \file    gd32w51x_qspi.c
    \brief   QSPI driver

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

#include "gd32w51x_qspi.h"

/* configure QSPI functionalmode */
static void qspi_config(qspi_command_struct* cmd, uint32_t functionalmode);
/* configure QSPI functionalmode in FMC mode secure register */
static void qspi_config_fmc_s(qspi_command_struct* cmd, uint32_t functionalmode);

/*!
    \brief      initialize QSPI parameter
    \param[in]  qspi_struct: QSPI parameter initialization stuct members of the structure 
                            and the member values are shown as below:
                  clockmode: QSPI_CLOCK_MODE_0, QSPI_CLOCK_MODE_3
                  fifothreshold: between 1 and 16
                  sampleshift: QSPI_SAMPLE_SHIFTING_NONE, QSPI_SAMPLE_SHIFTING_HALFCYCLE, QSPI_SAMPLE_SHIFTING_ONECYCLE
                  flashsize: between 0 and 28
                  chipselecthightime: QSPI_CS_HIGH_TIME_1_CYCLE, QSPI_CS_HIGH_TIME_2_CYCLE, QSPI_CS_HIGH_TIME_3_CYCLE, 
                                      QSPI_CS_HIGH_TIME_4_CYCLE, QSPI_CS_HIGH_TIME_5_CYCLE, QSPI_CS_HIGH_TIME_6_CYCLE, 
                                      QSPI_CS_HIGH_TIME_7_CYCLE, QSPI_CS_HIGH_TIME_8_CYCLE
                  prescale: between 0 and 255
    \param[out] none
    \retval     none
*/
void qspi_init(qspi_init_struct* qspi_struct)
{
    uint32_t reg = 0U;
    reg = QSPI_CTL;
    reg &= ~(QSPI_CTL);
    reg |= ((qspi_struct->fifothreshold - 1U) << 8);
    QSPI_CTL = reg;
    /* Wait till BUSY flag reset */
    while((QSPI_CTL & QSPI_CTL_BUSY) != 0U){
    }

    /* Configure prescaler, sampleshift */
    reg = QSPI_CTL;
    reg &= ~((QSPI_CTL_PSC) | QSPI_SAMPLE_SHIFTING_HALFCYCLE);
    reg |= (qspi_struct->prescaler << 24) | qspi_struct->sampleshift;
    QSPI_CTL = reg;

    /* Configure flashsize, chipselecthightime, clockmode */
    reg = QSPI_DCFG;
    reg &= ~((QSPI_DCFG_FMSZ) | QSPI_CS_HIGH_TIME_8_CYCLE | QSPI_DCFG_CKMOD);
    reg |= ((qspi_struct->flashsize << 16) | qspi_struct->chipselecthightime | qspi_struct->clockmode);
    QSPI_DCFG = reg;

    QSPI_CTL  |= QSPI_CTL_QSPIEN;
    QSPI_STAT |= QSPI_STAT_TERRIE;
}

/*!
    \brief      QSPI command parameter
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[out] none
    \retval     none
*/
void qspi_command(qspi_command_struct* cmd)
{
    while((QSPI_CTL & QSPI_FLAG_BUSY) != 0U){
    }

    /* Call the configuration function */
    qspi_config(cmd, QSPI_INDIRECT_WRITE);

    if(cmd->datamode == QSPI_DATA_NONE){
        /* When there is no data phase, the transfer start as soon as the configuration is done 
        so wait until TC flag is set to go back in idle state */
        while((QSPI_STAT & QSPI_FLAG_TC) == 0U){
        }
        QSPI_STATC = QSPI_STATC_TCC;
    }
}

/*!
    \brief      configure QSPI functionalmode 
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[in]  functionalmode: QSPI functionalmode select
                only one parameter can be selected which is shown as below:
      \arg        QSPI_INDIRECT_WRITE
      \arg        QSPI_INDIRECT_READ
      \arg        QSPI_AUTO_POLLING
      \arg        QSPI_MEMORY_MAPPED
    \param[out] none
    \retval     none
*/
static void qspi_config(qspi_command_struct* cmd, uint32_t functionalmode)
{
    if((cmd->datamode != QSPI_DATA_NONE) && (functionalmode != QSPI_MEMORY_MAPPED)){
        /* Configure QSPI_DTLEN register with the number of data to read or write */
        QSPI_DTLEN = cmd->nbdata - 1U;
    }

    if(cmd->instructionmode != QSPI_INSTRUCTION_NONE){
        if(cmd->alternatebytemode != QSPI_ALTERNATE_BYTES_NONE){
            /* Configure QSPI_ALTE register with alternate bytes value */
            QSPI_ALTE = cmd->alternatebytes;

            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with instruction, address and alternate bytes ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytessize | cmd->alternatebytemode | 
                            cmd->addresssize | cmd->addressmode | cmd->instructionmode | 
                            cmd->instruction | functionalmode;

                if(functionalmode != QSPI_MEMORY_MAPPED ){
                    /* Configure QSPI_ADDR register with address value */
                    QSPI_ADDR = cmd->address;
                }
            }
            else{
                /*---- Command with instruction and alternate bytes ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytessize | cmd->alternatebytemode | 
                            cmd->addressmode | cmd->instructionmode | 
                            cmd->instruction | functionalmode;
            }
        }
        else{
            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with instruction and address ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytemode | cmd->addresssize | cmd->addressmode | 
                            cmd->instructionmode | cmd->instruction | functionalmode;

                if(functionalmode != QSPI_MEMORY_MAPPED){
                    /* Configure QSPI_ADDR register with address value */
                    QSPI_ADDR = cmd->address;
                }
            }
            else{
                /*---- Command with only instruction ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytemode | cmd->addressmode | cmd->instructionmode | 
                            cmd->instruction | functionalmode;
            }
        }
    }
    else{
        if(cmd->alternatebytemode != QSPI_ALTERNATE_BYTES_NONE){
            /* Configure QSPI_ALTE register with alternate bytes value */
            QSPI_ALTE = cmd->alternatebytes;

            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with address and alternate bytes ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytemode | cmd->alternatebytessize | cmd->addresssize | 
                            cmd->addressmode | cmd->instructionmode | functionalmode;

                if(functionalmode != QSPI_MEMORY_MAPPED){
                    /* Configure QSPI_ADDR register with address value */
                    QSPI_ADDR = cmd->address;
                }
            }
            else{
                /*---- Command with only alternate bytes ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytemode | cmd->alternatebytessize | 
                            cmd->addressmode | cmd->instructionmode | functionalmode;
            }
        }
        else{
            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with only address ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                            cmd->alternatebytemode | cmd->addresssize | 
                            cmd->addressmode | cmd->instructionmode | functionalmode;
                
                if(functionalmode != QSPI_MEMORY_MAPPED){
                    /* Configure QSPI_ADDR register with address value */
                    QSPI_ADDR = cmd->address;
                }
            }
            else{
                /*---- Command with only data phase ----*/
                if(cmd->datamode != QSPI_DATA_NONE){
                    /* Configure QSPI_TCFG register with all communications parameters */
                    QSPI_TCFG = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                cmd->alternatebytemode | cmd->addressmode | 
                                cmd->instructionmode | functionalmode;
                }
            }
        }
    }
}

/*!
    \brief      QSPI transmit data
    \param[in]  tdata: 8-bit data
    \param[out] none
    \retval     none
*/
void qspi_transmit(uint8_t *tdata)
{
    uint32_t TxCounter;
    TxCounter = QSPI_DTLEN + 1U;

    /* Configure QSPI: CFG register with functional as indirect write */
    QSPI_TCFG = (QSPI_TCFG & ~QSPI_MEMORY_MAPPED) | QSPI_INDIRECT_WRITE;

    while(TxCounter > 0U){
        while((QSPI_STAT & QSPI_FLAG_FT) == 0U){
        }
        *(uint8_t *)(QSPI + 0x00000020U) = *tdata++;
        TxCounter--;
    }
    while((QSPI_STAT & QSPI_FLAG_TC) == 0U){
    }
    QSPI_STATC = QSPI_STATC_TCC;
}

/*!
    \brief      QSPI receive data
    \param[in]  rdata: empty array
    \param[out] 8-bit data array
    \retval     none
*/
void qspi_receive(uint8_t *rdata)
{
    uint32_t RxCounter, addr;
    RxCounter = QSPI_DTLEN + 1U;
    addr = QSPI_ADDR;

    /* Configure QSPI: CFG register with functional as indirect read */
    QSPI_TCFG = (QSPI_TCFG & ~QSPI_MEMORY_MAPPED) | QSPI_INDIRECT_READ;
    /* Start the transfer by re-writing the address in AR register */
    QSPI_ADDR = addr;

    while(RxCounter > 0U){
        while((QSPI_STAT & (QSPI_FLAG_FT | QSPI_FLAG_TC)) == 0U){
        }
        *rdata++ = *(uint8_t *)(QSPI + 0x00000020U);
        RxCounter--;
    }
    while((QSPI_STAT & QSPI_FLAG_TC) == 0U){
    }
    QSPI_STATC = QSPI_STATC_TCC;
}

/*!
    \brief      configure QSPI autopolling mode
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[in]  cfg: autopolling struct initialization stuct members of the structure 
                             and the member values are shown as below:
                  match: between 0 and 0xFFFFFFFF
                  mask: between 0 and 0xFFFFFFFF
                  interval: between 0 and 0xFFFF
                  statusbytessize: between 1 and 4
                  matchmode: QSPI_MATCH_MODE_AND, QSPI_MATCH_MODE_OR
                  automaticstop: QSPI_AUTOMATIC_STOP_DISABLE, QSPI_AUTOMATIC_STOP_ENABLE
    \param[out] none
    \retval     none
*/
void qspi_autopolling(qspi_command_struct *cmd, qspi_autopolling_struct *cfg)
{
    /* Wait till BUSY flag reset */
    while((QSPI_CTL & QSPI_FLAG_BUSY) != 0U){
    }
    QSPI_STATMATCH = cfg->match;
    QSPI_STATMK = cfg->mask;
    QSPI_INTERVAL = cfg->interval;
    QSPI_CTL = (QSPI_CTL & (~(QSPI_CTL_SPS | QSPI_CTL_SPMOD))) | (cfg->matchmode | cfg->automaticstop);
    cmd->nbdata = cfg->statusbytessize;
    qspi_config(cmd, QSPI_AUTO_POLLING);

    while((QSPI_STAT & QSPI_FLAG_SM) == 0U){
    }
    QSPI_STATC = QSPI_STATC_SMC;
}

/*!
    \brief      configure QSPI memorymapped mode
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[in]  timeout: 0-0xFFFF
    \param[in]  toen: QSPI_TOEN_DISABLE, QSPI_TOEN_ENABLE
    \param[out] none
    \retval     none
*/
void qspi_memorymapped(qspi_command_struct *cmd, uint16_t timeout, uint32_t toen)
{
    while((QSPI_CTL & QSPI_FLAG_BUSY) != 0U){
    }

    /* Configure QSPI: CTL register with timeout counter enable */
    QSPI_CTL = (QSPI_CTL & ~QSPI_CTL_TMOUTEN) | toen;

    if(toen == QSPI_CTL_TMOUTEN){
        /* Configure QSPI: TOR register with the low-power timeout value */
        QSPI_TMOUT = timeout;

        /* Clear interrupt */
        QSPI_STATC = QSPI_STATC_TMOUTC;

        /* Enable the QSPI TimeOut Interrupt */
        QSPI_CTL |= QSPI_FLAG_TMOUTIE;
    }
    qspi_config(cmd, QSPI_MEMORY_MAPPED);
}

/*!
    \brief      abort the current transmission. 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_abort(void)
{
    QSPI_CTL |= QSPI_CTL_ABORT;
}



/* if qspi in FMC mode, use above functions to access secure region. */
/*!
    \brief      QSPI command parameter
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[out] none
    \retval     none
*/
void qspi_command_fmc_s(qspi_command_struct* cmd)
{
    while((QSPI_CTL & QSPI_FLAG_BUSY) != 0U){
    }

    /* Call the configuration function */
    qspi_config_fmc_s(cmd, QSPI_INDIRECT_WRITE);

    if(cmd->datamode == QSPI_DATA_NONE){
        /* When there is no data phase, the transfer start as soon as the configuration is done 
        so wait until TC flag is set to go back in idle state */
        while((QSPI_STAT_FMC_S & QSPI_FLAG_TC) == 0U){
        }
        QSPI_STATC_FMC_S = QSPI_STATC_TCC;
    }
}

/*!
    \brief      configure QSPI functionalmode in FMC mode secure register
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[in]  functionalmode: QSPI functionalmode select
                only one parameter can be selected which is shown as below:
      \arg        QSPI_INDIRECT_WRITE
      \arg        QSPI_INDIRECT_READ
      \arg        QSPI_AUTO_POLLING
      \arg        QSPI_MEMORY_MAPPED
    \param[out] none
    \retval     none
*/
static void qspi_config_fmc_s(qspi_command_struct* cmd, uint32_t functionalmode)
{
    if((cmd->datamode != QSPI_DATA_NONE) && (functionalmode != QSPI_MEMORY_MAPPED)){
        /* Configure QSPI_DTLEN register with the number of data to read or write */
        QSPI_DTLEN_FMC_S = cmd->nbdata - 1U;
    }

    if(cmd->instructionmode != QSPI_INSTRUCTION_NONE){
        if(cmd->alternatebytemode != QSPI_ALTERNATE_BYTES_NONE){
            /* Configure QSPI_ALTE register with alternate bytes value */
            QSPI_ALTE_FMC_S = cmd->alternatebytes;

            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with instruction, address and alternate bytes ----*/
                /* Configure QSPI_TCFG register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytessize | cmd->alternatebytemode | 
                                    cmd->addresssize | cmd->addressmode | cmd->instructionmode | 
                                    cmd->instruction | functionalmode;

                if(functionalmode != QSPI_MEMORY_MAPPED ){
                    /* Configure QSPI_ADDR register with address value */
                    QSPI_ADDR_FMC_S = cmd->address;
                }
            }
            else{
                /*---- Command with instruction and alternate bytes ----*/
                /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytessize | cmd->alternatebytemode | 
                                    cmd->addressmode | cmd->instructionmode | 
                                    cmd->instruction | functionalmode;
            }
        }
        else{
            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with instruction and address ----*/
                /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytemode | cmd->addresssize | cmd->addressmode | 
                                    cmd->instructionmode | cmd->instruction | functionalmode;

                if(functionalmode != QSPI_MEMORY_MAPPED){
                    /* Configure QSPI_ADDR_FMC_S register with address value */
                    QSPI_ADDR_FMC_S = cmd->address;
                }
            }
            else{
                /*---- Command with only instruction ----*/
                /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytemode | cmd->addressmode | cmd->instructionmode | 
                                    cmd->instruction | functionalmode;
            }
        }
    }
    else{
        if(cmd->alternatebytemode != QSPI_ALTERNATE_BYTES_NONE){
            /* Configure QSPI_ALTE_FMC_S register with alternate bytes value */
            QSPI_ALTE_FMC_S = cmd->alternatebytes;            

            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with address and alternate bytes ----*/
                /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytemode | cmd->alternatebytessize | cmd->addresssize | 
                                    cmd->addressmode | cmd->instructionmode | functionalmode;

                if(functionalmode != QSPI_MEMORY_MAPPED){
                    /* Configure QSPI_ADDR_FMC_S register with address value */
                    QSPI_ADDR_FMC_S = cmd->address;                
                }
            }
            else{
                /*---- Command with only alternate bytes ----*/
                /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytemode | cmd->alternatebytessize | 
                                    cmd->addressmode | cmd->instructionmode | functionalmode;
            }
        }
        else{
            if(cmd->addressmode != QSPI_ADDRESS_NONE){
                /*---- Command with only address ----*/
                /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                    cmd->alternatebytemode | cmd->addresssize | 
                                    cmd->addressmode | cmd->instructionmode | functionalmode;
                
                if(functionalmode != QSPI_MEMORY_MAPPED){
                    /* Configure QSPI_ADDR_FMC_S register with address value */
                    QSPI_ADDR_FMC_S = cmd->address;
                }
            }
            else{
                /*---- Command with only data phase ----*/
                if(cmd->datamode != QSPI_DATA_NONE){
                    /* Configure QSPI_TCFG_FMC_S register with all communications parameters */
                    QSPI_TCFG_FMC_S = cmd->datamode | (cmd->dummycycles << 18) | cmd->sioomode | 
                                        cmd->alternatebytemode | cmd->addressmode | 
                                        cmd->instructionmode | functionalmode;
                }
            }
        }
    }
}

/*!
    \brief      QSPI transmit data in FMC mode secure register
    \param[in]  tdata: 8-bit data
    \param[out] none
    \retval     none
*/
void qspi_transmit_fmc_s(uint8_t *tdata)
{
    uint32_t TxCounter;
    TxCounter = QSPI_DTLEN_FMC_S + 1U;

    /* Configure QSPI: CFG register with functional as indirect write */
    QSPI_TCFG_FMC_S = (QSPI_TCFG_FMC_S & ~QSPI_MEMORY_MAPPED) | QSPI_INDIRECT_WRITE;

    while(TxCounter > 0U){
        while((QSPI_STAT_FMC_S & QSPI_FLAG_FT) == 0U){
        }
        *(uint8_t *)(QSPI + 0x00000120U) = *tdata++;
        TxCounter--;
    }

    while((QSPI_STAT_FMC_S & QSPI_FLAG_TC) == 0U){
    }
    QSPI_STATC_FMC_S = QSPI_STATC_TCC;
}

/*!
    \brief      QSPI receive data in FMC mode secure register
    \param[in]  rdata: empty array
    \param[out] 8-bit data array
    \retval     none
*/
void qspi_receive_fmc_s(uint8_t *rdata)
{
    uint32_t RxCounter, addr;
    RxCounter = QSPI_DTLEN_FMC_S + 1U;
    addr = QSPI_ADDR_FMC_S;

    /* Configure QSPI: CFG register with functional as indirect read */
    QSPI_TCFG_FMC_S = (QSPI_TCFG_FMC_S & ~QSPI_MEMORY_MAPPED) | QSPI_INDIRECT_READ;
    /* Start the transfer by re-writing the address in AR register */
    QSPI_ADDR_FMC_S = addr;

    while(RxCounter > 0U){
        while((QSPI_STAT_FMC_S & (QSPI_FLAG_FT | QSPI_FLAG_TC)) == 0U){
        }
        *rdata++ = *(uint8_t *)(QSPI + 0x00000120U);
        RxCounter--;
    }
    while((QSPI_STAT_FMC_S & QSPI_FLAG_TC) == 0U){
    }
    QSPI_STATC_FMC_S = QSPI_STATC_TCC;
}

/*!
    \brief      configure QSPI autopolling mode in FMC mode secure register
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[in]  cfg: autopolling struct initialization stuct members of the structure 
                             and the member values are shown as below:
                  match: between 0 and 0xFFFFFFFF
                  mask: between 0 and 0xFFFFFFFF
                  interval: between 0 and 0xFFFF
                  statusbytessize: between 1 and 4
                  matchmode: QSPI_MATCH_MODE_AND, QSPI_MATCH_MODE_OR
                  automaticstop: QSPI_AUTOMATIC_STOP_DISABLE, QSPI_AUTOMATIC_STOP_ENABLE
    \param[out] none
    \retval     none
*/
void qspi_autopolling_fmc_s(qspi_command_struct *cmd, qspi_autopolling_struct *cfg)
{
    /* Wait till BUSY flag reset */
    while((QSPI_STAT_FMC_S & QSPI_FLAG_BUSY) != 0U){
    }

    QSPI_STATMATCH = cfg->match;
    QSPI_STATMK = cfg->mask;
    QSPI_INTERVAL = cfg->interval;
    QSPI_CTL = (QSPI_CTL & (~(QSPI_CTL_SPS | QSPI_CTL_SPMOD))) | (cfg->matchmode | cfg->automaticstop);
    cmd->nbdata = cfg->statusbytessize;
    qspi_config_fmc_s(cmd, QSPI_AUTO_POLLING);

    while((QSPI_STAT_FMC_S & QSPI_FLAG_SM) == 0U){
    }
    QSPI_STATC_FMC_S = QSPI_STATC_SMC;
}

/*!
    \brief      configure QSPI memorymapped mode in FMC mode secure register
    \param[in]  cmd: QSPI command parameter initialization stuct members of the structure 
                             and the member values are shown as below:
                  instruction: reference flash commands description
                  address: 0-flash size
                  alternatebytes: 0-0xFFFFFFFF
                  addresssize: QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS
                  alternatebytessize: QSPI_ALTERNATE_BYTES_8_BITS, QSPI_ALTERNATE_BYTES_16_BITS, 
                                      QSPI_ALTERNATE_BYTES_24_BITS, QSPI_ALTERNATE_BYTES_32_BITS
                  dummycycles: between 0 and 31
                  instructionmode: QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, 
                                   QSPI_INSTRUCTION_2_LINES, QSPI_INSTRUCTION_4_LINES
                  addressmode: QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINES, QSPI_ADDRESS_4_LINES
                  alternatebytemode: QSPI_ALTERNATE_BYTES_NONE, QSPI_ALTERNATE_BYTES_1_LINE, 
                                     QSPI_ALTERNATE_BYTES_2_LINES, QSPI_ALTERNATE_BYTES_4_LINES
                  datamode: QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINES, QSPI_DATA_4_LINES
                  nbdata: 0-0xFFFFFFFF
                  sioomode: QSPI_SIOO_INST_EVERY_CMD, QSPI_SIOO_INST_ONLY_FIRST_CMD
    \param[in]  timeout: 0-0xFFFF
    \param[in]  toen: QSPI_TMOUTEN_DISABLE, QSPI_TMOUTEN_ENABLE
    \param[out] none
    \retval     none
*/
void qspi_memorymapped_fmc_s(qspi_command_struct *cmd, uint16_t timeout, uint32_t toen)
{
    while((QSPI_STAT_FMC_S & QSPI_FLAG_BUSY) != 0U){
    }

    /* Configure QSPI: CTL register with timeout counter enable */
    QSPI_CTL = (QSPI_CTL & ~QSPI_CTL_TMOUTEN) | toen;

    if(toen == QSPI_CTL_TMOUTEN){
        /* Configure QSPI: TOR register with the low-power timeout value */
        QSPI_TMOUT = timeout;

        /* Clear interrupt */
        QSPI_STATC_FMC_S = QSPI_STATC_TMOUTC;

        /* Enable the QSPI TimeOut Interrupt */
        QSPI_CTL |= QSPI_FLAG_TMOUTIE;
    }
    qspi_config_fmc_s(cmd, QSPI_MEMORY_MAPPED);
}
