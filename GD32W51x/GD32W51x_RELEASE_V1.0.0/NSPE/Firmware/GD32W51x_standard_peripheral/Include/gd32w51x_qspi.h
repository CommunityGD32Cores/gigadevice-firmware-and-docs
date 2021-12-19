/*!
    \file    gd32w51x_qspi.h
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

#ifndef GD32W51X_QSPI_H
#define GD32W51X_QSPI_H

#include "gd32w51x.h"

/* QSPI definitions */
#define QSPI                              QSPI_REG_BASE

/* registers definitions */
#define QSPI_CTL                          REG32(QSPI + 0x00000000U)            /*!< QSPI control register */
#define QSPI_DCFG                         REG32(QSPI + 0x00000004U)            /*!< QSPI device configuration register */
#define QSPI_STAT                         REG32(QSPI + 0x00000008U)            /*!< QSPI status register */
#define QSPI_STATC                        REG32(QSPI + 0x0000000CU)            /*!< QSPI status clear register */
#define QSPI_DTLEN                        REG32(QSPI + 0x00000010U)            /*!< QSPI data length register */
#define QSPI_TCFG                         REG32(QSPI + 0x00000014U)            /*!< QSPI transfer configuration register */
#define QSPI_ADDR                         REG32(QSPI + 0x00000018U)            /*!< QSPI address register */
#define QSPI_ALTE                         REG32(QSPI + 0x0000001CU)            /*!< QSPI alternate bytes register */
#define QSPI_DATA                         REG32(QSPI + 0x00000020U)            /*!< QSPI data register */
#define QSPI_STATMK                       REG32(QSPI + 0x00000024U)            /*!< QSPI status mask register */
#define QSPI_STATMATCH                    REG32(QSPI + 0x00000028U)            /*!< QSPI status match register */
#define QSPI_INTERVAL                     REG32(QSPI + 0x0000002CU)            /*!< QSPI interval register */
#define QSPI_TMOUT                        REG32(QSPI + 0x00000030U)            /*!< QSPI timeout register */
#define QSPI_FLUSH                        REG32(QSPI + 0x00000034U)            /*!< QSPI fifo flush register */
#define QSPI_WTCNT                        REG32(QSPI + 0x00000038U)            /*!< QSPI wait cnt for indirect wire mode register */
#define QSPI_SPTMOUT                      REG32(QSPI + 0x0000003CU)            /*!< QSPI timeout for status polling mode register */
#define QSPI_FMC_SECCFG                   REG32(QSPI + 0x0000007CU)            /*!< QSPI FMC mode security configuration register */
#define QSPI_CTLF                         REG32(QSPI + 0x00000080U)            /*!< QSPI control register in FMC mode */
#define QSPI_TCFGF                        REG32(QSPI + 0x00000084U)            /*!< QSPI transfer configuration register in FMC mode*/
#define QSPI_ALTEF                        REG32(QSPI + 0x00000088U)            /*!< QSPI alternate bytes register in FMC mode */
#define QSPI_BYTE_CNT                     REG32(QSPI + 0x0000008CU)            /*!< QSPI complete bytes counter register */
#define QSPI_PRIVCFG                      REG32(QSPI + 0x00000090U)            /*!< QSPI privilege configuration register */

/* QSPI in FMC mode secure register */
#define QSPI_STAT_FMC_S                   REG32(QSPI + 0x00000108U)            /*!< QSPI status register */
#define QSPI_STATC_FMC_S                  REG32(QSPI + 0x0000010CU)            /*!< QSPI status clear register */
#define QSPI_DTLEN_FMC_S                  REG32(QSPI + 0x00000110U)            /*!< QSPI data length register */
#define QSPI_TCFG_FMC_S                   REG32(QSPI + 0x00000114U)            /*!< QSPI transfer configuration register */
#define QSPI_ADDR_FMC_S                   REG32(QSPI + 0x00000118U)            /*!< QSPI address register */
#define QSPI_ALTE_FMC_S                   REG32(QSPI + 0x0000011CU)            /*!< QSPI alternate bytes register */
#define QSPI_DATA_FMC_S                   REG32(QSPI + 0x00000120U)            /*!< QSPI data register */

/* bits definitions */
/* QSPI_CTL */
#define QSPI_CTL_QSPIEN                   BIT(0)                               /*!< enable the quadspi */
#define QSPI_CTL_ABORT                    BIT(1)                               /*!< abort request */
#define QSPI_CTL_TMOUTEN                  BIT(3)                               /*!< timeout counter enable */
#define QSPI_CTL_SSAMPLE                  BITS(4,5)                            /*!< sample shift */
#define QSPI_CTL_SCKDEN                   BIT(6)                               /*!< sck delay enable */
#define QSPI_CTL_FTL                      BITS(8,11)                           /*!< fifo threshold level */
#define QSPI_CTL_BUSY                     BIT(14)                              /*!< busy flag */
#define QSPI_CTL_FMCMOD                   BIT(15)                              /*!< busy in FMC mode */
#define QSPI_CTL_FL                       BITS(16,20)                          /*!< fifo level */
#define QSPI_CTL_SPS                      BIT(22)                              /*!< status polling mode stop */
#define QSPI_CTL_SPMOD                    BIT(23)                              /*!< status polling match mode */
#define QSPI_CTL_PSC                      BITS(24,31)                          /*!< prescaler set */

/* QSPI_DCFG */
#define QSPI_DCFG_CKMOD                   BIT(0)                               /*!< this bit indicates the sck level when qspi is free */
#define QSPI_DCFG_CSHC                    BITS(8,10)                           /*!< chip select high cycle */
#define QSPI_DCFG_FMSZ                    BITS(16,20)                          /*!< flash memory size */

/* QSPI_STAT */
#define QSPI_STAT_TERR                    BIT(0)                               /*!< transfer error flag */
#define QSPI_STAT_TC                      BIT(1)                               /*!< transfer complete flag */
#define QSPI_STAT_FT                      BIT(2)                               /*!< fifo threshold flag */
#define QSPI_STAT_SM                      BIT(3)                               /*!< status match flag */
#define QSPI_STAT_TMOUT                   BIT(4)                               /*!< timeout flag */
#define QSPI_STAT_WS                      BIT(5)                               /*!< wrong start sequence flag */
#define QSPI_STAT_TERRIE                  BIT(6)                               /*!< enable transfer error interrupt */
#define QSPI_STAT_TCIE                    BIT(7)                               /*!< enable transfer complete interrupt */
#define QSPI_STAT_FTIE                    BIT(8)                               /*!< enable fifo threshold */
#define QSPI_STAT_SMIE                    BIT(9)                               /*!< enable status match */
#define QSPI_STAT_TMOUTIE                 BIT(10)                              /*!< enable timeout */
#define QSPI_STAT_WSIE                    BIT(11)                              /*!< enable wrong start sequence */
#define QSPI_STAT_DMAEN                   BIT(16)                              /*!< enable dma */


/* QSPI_STATC */
#define QSPI_STATC_TERRC                  BIT(0)                               /*!< clear transfer error flag */
#define QSPI_STATC_TCC                    BIT(1)                               /*!< clear transfer complete flag */
#define QSPI_STATC_SMC                    BIT(3)                               /*!< clear status match flag */
#define QSPI_STATC_TMOUTC                 BIT(4)                               /*!< clear timeout flag */
#define QSPI_STATC_WSC                    BIT(5)                               /*!< clear wrong start sequence flag */

/* QSPI_DTLEN */
#define QSPI_DTLEN_DTLEN                  BITS(0,31)                           /*!< data length */

/* QSPI_TCFG */
#define QSPI_TCFG_INSTRUCTION             BITS(0,7)                            /*!< command information to be send to the flash memory */
#define QSPI_TCFG_IMOD                    BITS(8,9)                            /*!< instruction mode */
#define QSPI_TCFG_ADDRMOD                 BITS(10,11)                          /*!< address mode */
#define QSPI_TCFG_ADDRSZ                  BITS(12,13)                          /*!< address size */
#define QSPI_TCFG_ALTEMOD                 BITS(14,15)                          /*!< alternate bytes mode */
#define QSPI_TCFG_ALTESZ                  BITS(16,17)                          /*!< alternate bytes size */
#define QSPI_TCFG_DUMYC                   BITS(18,22)                          /*!< number of dummy cycles */
#define QSPI_TCFG_DATAMOD                 BITS(24,25)                          /*!< data mode */
#define QSPI_TCFG_FMOD                    BITS(26,27)                          /*!< functional mode */
#define QSPI_TCFG_SIOO                    BIT(28)                              /*!< send instruction only once */

/* QSPI_ADDR */
#define QSPI_ADDR_ADDR                    BITS(0,31)                           /*!< address to be send to the external flash memory */

/* QSPI_ALTE */
#define QSPI_ALTE_ALTE                    BITS(0,31)                           /*!< optional data to be send to the flash memory */

/* QSPI_DATA */
#define QSPI_DATA_DATA                    BITS(0,31)                           /*!< data to be transferred through the flash memory */

/* QSPI_STATMK */
#define QSPI_STATMK_MASK                  BITS(0,31)                           /*!< status mask */

/* QSPI_STATMA */
#define QSPI_STATMATCH_MATCH              BITS(0,31)                           /*!< status match */

/* QSPI_INTERVAL */
#define QSPI_INTERVAL_INTERVAL            BITS(0,15)                           /*!< interval cycle */

/* QSPI_TMOUT */
#define QSPI_TMOUT_TMOUT                  BITS(0,15)                           /*!< timeout cycle */

/* QSPI_FLUSH */
#define QSPI_FLUSH_FLUSH                  BIT(0)                               /*!< used to flush all qspi interval fifo */

/* QSPI_WTCNT */
#define QSPI_WTCNT_WTCNT                  BITS(0,31)                           /*!< wait cnt when an indirect write operation is completed or aborted */

/* QSPI_SPTMOUT */
#define QSPI_SPTMOUT_SPTMOUT              BITS(0,31)                           /*!< timeout cnt when a FMC transfer trys to abort a status polling mode operation */

/* QSPI_FMC_SECCFG */
#define QSPI_FMC_SECCFG_FMCSEC            BIT(0)                               /*!< use for indicate if register of FMC mode is secure or not */

/* QSPI_CTLF */
#define QSPI_CTLF_SSAMPLEF                BITS(4,5)                            /*!< sample shift in FMC mode */
#define QSPI_CTLF_SCKDENF                 BIT(6)                               /*!< sck delay enable when read data from flash in FMC mode */
#define QSPI_CTLF_SCKDVALUEF              BITS(12,15)                          /*!< sck delay value in FMC mode */
#define QSPI_CTLF_CSHCF                   BITS(16,18)                          /*!< the number of sck delay value */
#define QSPI_CTLF_CKMODF                  BIT(23)                              /*!< this bit indicates the sck level in FMC mode when qspi is free */
#define QSPI_CTLF_PSCF                    BITS(24,31)                          /*!< prescaler set */

/* QSPI_TCFGF */
#define QSPI_TCFGF_INSTRUCTIONF           BITS(0,7)                            /*!< instruction in FMC mode */
#define QSPI_TCFGF_IMODF                  BITS(8,9)                            /*!< instruction mode in FMC mode */
#define QSPI_TCFGF_ADDRMODF               BITS(10,11)                          /*!< address mode in FMC mode */
#define QSPI_TCFGF_ADDRSZF                BITS(12,13)                          /*!< address size in FMC mode */
#define QSPI_TCFGF_ALTEMODF               BITS(14,15)                          /*!< alternate bytes mode in FMC mode */
#define QSPI_TCFGF_ALTESZF                BITS(16,17)                          /*!< alternate bytes size in FMC mode */
#define QSPI_TCFGF_DUMYCF                 BITS(18,22)                          /*!< number of dummy cycles in FMC mode */
#define QSPI_TCFGF_DATAMODF               BITS(24,25)                          /*!< data mode in FMC mode */
#define QSPI_TCFGF_SIOOF                  BIT(28)                              /*!< send instruction only once mode in FMC mode */

/* QSPI_ALTEF */
#define QSPI_ALTEF_ALTEF                  BITS(0,31)                           /*!< alternate bytes in FMC mode */

/* QSPI_BYTE_CNT */
#define QSPI_BYTE_CNT_BYTECNT             BITS(0,31)                           /*!< remained bytes which has been aborted by FMC mode */

/* QSPI_PRIVCFG */
#define QSPI_PRIVCFG_PRIV                 BIT(0)                               /*!< use for indicate if register is privileged */

/* constants definitions */
/* qspi init struct definitions */
typedef struct
{
    uint32_t prescaler;          /* Specifies the prescaler factor for generating clock based on the AHB clock.
                                  This parameter can be a number between 0 and 255 */ 
    uint32_t fifothreshold;      /* Specifies the threshold number of bytes in the FIFO (used only in indirect mode)
                                  This parameter can be a value between 1 and 16 */
    uint32_t sampleshift;        /* Specifies the Sample Shift. The data is sampled 1/2 clock cycle delay later to 
                                  take in account external signal delays. (It should be QSPI_SAMPLE_SHIFTING_NONE in DDR mode) */
    uint32_t flashsize;          /* Specifies the Flash Size. FlashSize+1 is effectively the number of address bits 
                                  required to address the flash memory. The flash capacity can be up to 4GB 
                                  (addressed using 32 bits) in indirect mode, but the addressable space in 
                                  memory-mapped mode is limited to 256MB
                                  This parameter can be a number between 0 and 31 */
    uint32_t chipselecthightime; /* Specifies the Chip Select High Time. ChipSelectHighTime+1 defines the minimum number 
                                  of clock cycles which the chip select must remain high between commands. */   
    uint32_t clockmode;          /* Specifies the Clock Mode. It indicates the level that clock takes between commands. */
}qspi_init_struct;

/* qspi command struct definitions */
typedef struct
{
    uint32_t instruction;        /* Specifies the Instruction to be sent
                                  This parameter can be a value (8-bit) between 0x00 and 0xFF */
    uint32_t address;            /* Specifies the Address to be sent (Size from 1 to 4 bytes according AddressSize)
                                  This parameter can be a value (32-bits) between 0x0 and 0xFFFFFFFF */
    uint32_t alternatebytes;     /* Specifies the Alternate Bytes to be sent (Size from 1 to 4 bytes according AlternateBytesSize)
                                  This parameter can be a value (32-bits) between 0x0 and 0xFFFFFFFF */
    uint32_t addresssize;        /* Specifies the Address Size */
    uint32_t alternatebytessize; /* Specifies the Alternate Bytes Size */
    uint32_t dummycycles;        /* Specifies the Number of Dummy Cycles.
                                  This parameter can be a number between 0 and 31 */
    uint32_t instructionmode;    /* Specifies the Instruction Mode */
    uint32_t addressmode;        /* Specifies the Address Mode */
    uint32_t alternatebytemode;  /* Specifies the Alternate Bytes Mode */
    uint32_t datamode;           /* Specifies the Data Mode (used for dummy cycles and data phases) */
    uint32_t nbdata;             /* Specifies the number of data to transfer. (This is the number of bytes)
                                  This parameter can be any value between 0 and 0xFFFFFFFF (0 means undefined length 
                                  until end of memory)*/
    uint32_t sioomode;           /* Specifies the send instruction only once mode */
}qspi_command_struct;

/* qspi autopolling struct definitions */
typedef struct
{
    uint32_t match;              /* Specifies the value to be compared with the masked status register to get a match.
                                  This parameter can be any value between 0 and 0xFFFFFFFF */
    uint32_t mask;               /* Specifies the mask to be applied to the status bytes received. 
                                  This parameter can be any value between 0 and 0xFFFFFFFF */
    uint32_t interval;           /* Specifies the number of clock cycles between two read during automatic polling phases.
                                  This parameter can be any value between 0 and 0xFFFF */
    uint32_t statusbytessize;    /* Specifies the size of the status bytes received.
                                  This parameter can be any value between 1 and 4 */
    uint32_t matchmode;          /* Specifies the method used for determining a match. */
    uint32_t automaticstop;      /* Specifies if automatic polling is stopped after a match. */
}qspi_autopolling_struct;

/* QSPI flag definitions */
#define QSPI_FLAG_BUSY                    QSPI_CTL_BUSY                        /*!< busy flag */
#define QSPI_FLAG_TERR                    QSPI_STAT_TERR                       /*!< transfer error flag */
#define QSPI_FLAG_TC                      QSPI_STAT_TC                         /*!< transfer complete flag */
#define QSPI_FLAG_FT                      QSPI_STAT_FT                         /*!< fifo threshold flag */
#define QSPI_FLAG_SM                      QSPI_STAT_SM                         /*!< status match flag */
#define QSPI_FLAG_TMOUT                   QSPI_STAT_TMOUT                      /*!< timeout flag */
#define QSPI_FLAG_WS                      QSPI_STAT_WS                         /*!< wrong start sequence flag */
#define QSPI_FLAG_TERRIE                  QSPI_STAT_TERRIE                     /*!< enable transfer error interrupt */
#define QSPI_FLAG_TCIE                    QSPI_STAT_TCIE                       /*!< enable transfer complete interrupt */
#define QSPI_FLAG_FTIE                    QSPI_STAT_FTIE                       /*!< enable fifo threshold */
#define QSPI_FLAG_SMIE                    QSPI_STAT_SMIE                       /*!< enable status match */
#define QSPI_FLAG_TMOUTIE                 QSPI_STAT_TMOUTIE                    /*!< enable timeout */
#define QSPI_FLAG_WSIE                    QSPI_STAT_WSIE                       /*!< enable wrong start sequence */

/* enable QSPI dma */
#define QSPI_DMAEN                        QSPI_STAT_DMAEN                      /*!< enable dma */

/* QSPI clock mode */
#define QSPI_CLOCK_MODE_0                 0x00000000                           /*!< qspi clock mode 0 */
#define QSPI_CLOCK_MODE_3                 QSPI_DCFG_CKMOD                      /*!< qspi clock mode 3 */

/* QSPI sample shift */
#define CTL_SSAMPLE(regval)               (BITS(4,5) & ((uint32_t)(regval) << 4))
#define QSPI_SAMPLE_SHIFTING_NONE         CTL_SSAMPLE(0)                             /*!< no shift when sample data */
#define QSPI_SAMPLE_SHIFTING_HALFCYCLE    CTL_SSAMPLE(1)                             /*!< 1/2 sck cycle shift when sample data */
#define QSPI_SAMPLE_SHIFTING_ONECYCLE     CTL_SSAMPLE(2)                             /*!< one sck cycle shift when sample data */

/* QSPI chip select high cycle */
#define DCFG_CSHC(regval)                 (BITS(8,10) & ((uint32_t)(regval) << 8))
#define QSPI_CS_HIGH_TIME_1_CYCLE         DCFG_CSHC(0)                               /*!< QSPI CS stays high for at least 1 cycle */
#define QSPI_CS_HIGH_TIME_2_CYCLE         DCFG_CSHC(1)                               /*!< QSPI CS stays high for at least 2 cycle */
#define QSPI_CS_HIGH_TIME_3_CYCLE         DCFG_CSHC(2)                               /*!< QSPI CS stays high for at least 3 cycle */
#define QSPI_CS_HIGH_TIME_4_CYCLE         DCFG_CSHC(3)                               /*!< QSPI CS stays high for at least 4 cycle */
#define QSPI_CS_HIGH_TIME_5_CYCLE         DCFG_CSHC(4)                               /*!< QSPI CS stays high for at least 5 cycle */
#define QSPI_CS_HIGH_TIME_6_CYCLE         DCFG_CSHC(5)                               /*!< QSPI CS stays high for at least 6 cycle */
#define QSPI_CS_HIGH_TIME_7_CYCLE         DCFG_CSHC(6)                               /*!< QSPI CS stays high for at least 7 cycle */
#define QSPI_CS_HIGH_TIME_8_CYCLE         DCFG_CSHC(7)                               /*!< QSPI CS stays high for at least 8 cycle */

/* QSPI instruction mode */
#define TCFG_IMODE(regval)                (BITS(8,9) & ((uint32_t)(regval) << 8))
#define QSPI_INSTRUCTION_NONE             TCFG_IMODE(0)                              /*!< no instruction */
#define QSPI_INSTRUCTION_1_LINE           TCFG_IMODE(1)                              /*!< instruction on a single line */
#define QSPI_INSTRUCTION_2_LINES          TCFG_IMODE(2)                              /*!< instruction on two lines */
#define QSPI_INSTRUCTION_4_LINES          TCFG_IMODE(3)                              /*!< instruction on four lines */

/* QSPI address mode */
#define TCFG_ADDRMOD(regval)              (BITS(10,11) & ((uint32_t)(regval) << 10))
#define QSPI_ADDRESS_NONE                 TCFG_ADDRMOD(0)                            /*!< no address */
#define QSPI_ADDRESS_1_LINE               TCFG_ADDRMOD(1)                            /*!< address on a single line */
#define QSPI_ADDRESS_2_LINES              TCFG_ADDRMOD(2)                            /*!< address on two lines */
#define QSPI_ADDRESS_4_LINES              TCFG_ADDRMOD(3)                            /*!< address on four lines */

/* QSPI address size */
#define TCFG_ADDRSZ(regval)               (BITS(12,13) & ((uint32_t)(regval) << 12))
#define QSPI_ADDRESS_8_BITS               TCFG_ADDRSZ(0)                             /*!< 8 bits address */
#define QSPI_ADDRESS_16_BITS              TCFG_ADDRSZ(1)                             /*!< 16 bits address */
#define QSPI_ADDRESS_24_BITS              TCFG_ADDRSZ(2)                             /*!< 24 bits address */
#define QSPI_ADDRESS_32_BITS              TCFG_ADDRSZ(3)                             /*!< 32 bits address */

/* QSPI alternate bytes mode */
#define TCFG_ALTEMOD(regval)              (BITS(14,15) & ((uint32_t)(regval) << 14))
#define QSPI_ALTERNATE_BYTES_NONE         TCFG_ALTEMOD(0)                            /*!< no alternate bytes */
#define QSPI_ALTERNATE_BYTES_1_LINE       TCFG_ALTEMOD(1)                            /*!< alternate bytes on a single line */
#define QSPI_ALTERNATE_BYTES_2_LINES      TCFG_ALTEMOD(2)                            /*!< alternate bytes on two lines */
#define QSPI_ALTERNATE_BYTES_4_LINES      TCFG_ALTEMOD(3)                            /*!< alternate bytes on four lines */

/* QSPI alternate bytes size */
#define TCFG_ALTESZ(regval)               (BITS(16,17) & ((uint32_t)(regval) << 16))
#define QSPI_ALTERNATE_BYTES_8_BITS       TCFG_ALTESZ(0)                             /*!< 8 bits alternate byte */
#define QSPI_ALTERNATE_BYTES_16_BITS      TCFG_ALTESZ(1)                             /*!< 16 bits alternate bytes */
#define QSPI_ALTERNATE_BYTES_24_BITS      TCFG_ALTESZ(2)                             /*!< 24 bits alternate bytes */
#define QSPI_ALTERNATE_BYTES_32_BITS      TCFG_ALTESZ(3)                             /*!< 32 bits alternate bytes */

/* QSPI data mode */
#define TCFG_DATAMOD(regval)              (BITS(24,25) & ((uint32_t)(regval) << 24))
#define QSPI_DATA_NONE                    TCFG_DATAMOD(0)                            /*!< no data */
#define QSPI_DATA_1_LINE                  TCFG_DATAMOD(1)                            /*!< data on a single line */
#define QSPI_DATA_2_LINES                 TCFG_DATAMOD(2)                            /*!< data on two lines */
#define QSPI_DATA_4_LINES                 TCFG_DATAMOD(3)                            /*!< data on four lines */

/* QSPI functional mode */
#define TCFG_FMOD(regval)                 (BITS(26,27) & ((uint32_t)(regval) << 26))
#define QSPI_INDIRECT_WRITE               TCFG_FMOD(0)                               /*! <indirect write mode */
#define QSPI_INDIRECT_READ                TCFG_FMOD(1)                               /*! <indirect read mode */
#define QSPI_AUTO_POLLING                 TCFG_FMOD(2)                               /*! <status polling mode */
#define QSPI_MEMORY_MAPPED                TCFG_FMOD(3)                               /*! <memory mapped mode */

/* QSPI sioo mode */
#define QSPI_SIOO_INST_EVERY_CMD          0x00000000                                 /*!< send instruction on every command sequence */
#define QSPI_SIOO_INST_ONLY_FIRST_CMD     QSPI_TCFG_SIOO                             /*!< send instruction only for the first command sequence */

/* QSPI timeout count */
#define QSPI_TMOUTEN_DISABLE              0x00000000                                 /*!< disable timeout counter */
#define QSPI_TMOUTEN_ENABLE               QSPI_CTL_TMOUTEN                           /*!< enable timeout counter */

/* QSPI match mode */
#define QSPI_MATCH_MODE_AND               0x00000000                                 /*!< and match mode */
#define QSPI_MATCH_MODE_OR                QSPI_CTL_SPMOD                             /*!< or match mode */

/* QSPI automatic stop */
#define QSPI_AUTOMATIC_STOP_DISABLE       0x00000000                                 /*!< disable automatic stop */
#define QSPI_AUTOMATIC_STOP_ENABLE        QSPI_CTL_SPS                               /*!< enable automatic stop */

/* QSPI in FMC mode definitions */
/* QSPI chip select high cycle in FMC mode */
#define CTLF_CSHCF(regval)                (BITS(16,18) & ((uint32_t)(regval) << 16))
#define QSPI_CS_HIGH_TIME_1_CYCLE_FMC     CTLF_CSHCF(0)                              /*!< QSPI CS stays high for at least 1 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_2_CYCLE_FMC     CTLF_CSHCF(1)                              /*!< QSPI CS stays high for at least 2 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_3_CYCLE_FMC     CTLF_CSHCF(2)                              /*!< QSPI CS stays high for at least 3 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_4_CYCLE_FMC     CTLF_CSHCF(3)                              /*!< QSPI CS stays high for at least 4 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_5_CYCLE_FMC     CTLF_CSHCF(4)                              /*!< QSPI CS stays high for at least 5 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_6_CYCLE_FMC     CTLF_CSHCF(5)                              /*!< QSPI CS stays high for at least 6 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_7_CYCLE_FMC     CTLF_CSHCF(6)                              /*!< QSPI CS stays high for at least 7 cycle in fmc mode */
#define QSPI_CS_HIGH_TIME_8_CYCLE_FMC     CTLF_CSHCF(7)                              /*!< QSPI CS stays high for at least 8 cycle in fmc mode */

/* function declarations */
/* QSPI deinitialization and initialization functions */
/* initialize QSPI parameter */
void qspi_init(qspi_init_struct* qspi_struct);
/* QSPI command parameter configure*/
void qspi_command(qspi_command_struct* cmd);
/* QSPI transmit data */
void qspi_transmit(uint8_t *tdata);
/* QSPI receive data */
void qspi_receive(uint8_t *rdata);
/* configure QSPI autopolling mode */
void qspi_autopolling(qspi_command_struct *cmd, qspi_autopolling_struct *cfg);
/* configure QSPI memorymapped mode */
void qspi_memorymapped(qspi_command_struct *cmd, uint16_t timeout, uint32_t toen);
/* abort the current transmission */
void qspi_abort(void);

/* QSPI command parameter configure*/
void qspi_command_fmc_s(qspi_command_struct* cmd);
/* QSPI transmit data */
void qspi_transmit_fmc_s(uint8_t *tdata);
/* QSPI receive data */
void qspi_receive_fmc_s(uint8_t *rdata);
/* configure QSPI autopolling mode */
void qspi_autopolling_fmc_s(qspi_command_struct *cmd, qspi_autopolling_struct *cfg);
/* configure QSPI memorymapped mode */
void qspi_memorymapped_fmc_s(qspi_command_struct *cmd, uint16_t timeout, uint32_t toen);

#endif /* GD32W51X_QSPI_H */
