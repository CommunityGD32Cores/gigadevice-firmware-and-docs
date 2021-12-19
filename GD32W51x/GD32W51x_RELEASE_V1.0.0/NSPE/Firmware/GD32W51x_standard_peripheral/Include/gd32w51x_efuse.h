/*!
    \file  gd32w51x_efuse.h
    \brief definitions for the EFUSE

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

#ifndef GD32W51X_EFUSE_H
#define GD32W51X_EFUSE_H

#include "gd32w51x.h"

/* RCU definitions */
#define EFUSE                           EFUSE_BASE

/* registers definitions */
#define EFUSE_CS                        REG32(EFUSE + 0x00U)        /*!< control and status register */
#define EFUSE_ADDR                      REG32(EFUSE + 0x04U)        /*!< address register */
#define EFUSE_CTL                       REG32(EFUSE + 0x08U)        /*!< control register */
#define EFUSE_TZCTL                     REG32(EFUSE + 0x0CU)        /*!< trustzone control register */
#define EFUSE_FPCTL                     REG32(EFUSE + 0x10U)        /*!< flash protection control register register */
#define EFUSE_USCTL                     REG32(EFUSE + 0x14U)        /*!< user byte control register */
#define EFUSE_MCUI                      REG32(EFUSE + 0x18U)        /*!< mcu initialization data register  */
#define EFUSE_AES                       REG32(EFUSE + 0x24U)        /*!< firmware AES key register  */
#define EFUSE_ROTPK                     REG32(EFUSE + 0x34U)        /*!< root public key register */
#define EFUSE_DP                        REG32(EFUSE + 0x54U)        /*!< Debug password register */
#define EFUSE_RSS                       REG32(EFUSE + 0x5CU)        /*!< IAK or RSS register */
#define EFUSE_PUID                      REG32(EFUSE + 0x9CU)        /*!< product UID register */
#define EFUSE_HUK                       REG32(EFUSE + 0xACU)        /*!< hardware unique key register */
#define EFUSE_RF                        REG32(EFUSE + 0xBCU)        /*!< rf data register */
#define EFUSE_USER                      REG32(EFUSE + 0xECU)        /*!< user data register */
#define EFUSE_STZEN                     REG32(EFUSE + 0x118U)       /*!< trustzone enable register */

/* bits definitions */
/* EFUSE_CS */
#define EFUSE_CS_EFSTR                  BIT(0)                      /*!< Start efuse operation */
#define EFUSE_CS_EFRW                   BIT(1)                      /*!< The selection of efuse operation */
#define EFUSE_CS_PGIF                   BIT(16)                     /*!< Program operation completed flag */
#define EFUSE_CS_RDIF                   BIT(17)                     /*!< Read operation completed flag */
#define EFUSE_CS_OBERIF                 BIT(18)                     /*!< Overstep boundary error flag */
#define EFUSE_CS_PGIE                   BIT(20)                     /*!< Enable bit for program operation completed interrup */
#define EFUSE_CS_RDIE                   BIT(21)                     /*!< Enable bit for read operation completed interrupt */
#define EFUSE_CS_OBERIE                 BIT(22)                     /*!< Enable bit for overstep boundary error interrupt */
#define EFUSE_CS_PGIC                   BIT(24)                     /*!< Clear bit for program operation completed interrupt flag */
#define EFUSE_CS_RDIC                   BIT(25)                     /*!< Clear bit for read operation completed interrupt flag */
#define EFUSE_CS_OBERIC                 BIT(26)                     /*!< Clear bit for overstep boundary error interrupt flag */

/* EFUSE_ADDR */
#define EFUSE_ADDR_EFADDR               BITS(0,7)                   /*!< Read or write efuse data start address */
#define EFUSE_ADDR_EFSIZE               BITS(8,14)                  /*!< Read or write efuse data size */

/* EFUSE_CTL */
#define EFUSE_CTL_EFSB                  BIT(0)                      /*!< Boot from Secure boot */
#define EFUSE_CTL_EFBOOTLK              BIT(1)                      /*!< EFUSE_CTL register bit[5:2] lock bit */
#define EFUSE_CTL_EFBOOT1               BIT(2)                      /*!< Efuse boot1 */
#define EFUSE_CTL_SWBOOT1               BIT(3)                      /*!< Efuse boot 1 bit enable */
#define EFUSE_CTL_EFBOOT0               BIT(4)                      /*!< Efuse boot0 */
#define EFUSE_CTL_SWBOOT0               BIT(5)                      /*!< Efuse boot 0 bit enable */

/* EFUSE_TZCTL */
#define EFUSE_TZCTL_TZEN                BIT(0)                      /*!< Trust zone enable bit */
#define EFUSE_TZCTL_NDBG                BIT(1)                      /*!< Debugging permission setting */
#define EFUSE_TZCTL_ROTLK               BIT(2)                      /*!< EFUSE_ROTPK_KEY register lock bit */
#define EFUSE_TZCTL_RFLK                BIT(3)                      /*!< EFUSE_RF_DATA register lock bit */
#define EFUSE_TZCTL_RSSLK               BIT(4)                      /*!< EFUSE_IAK_RSS register lock bit */
#define EFUSE_TZCTL_DPLK                BIT(5)                      /*!< EFUSE_DP register lock bit */
#define EFUSE_TZCTL_VFIMG               BIT(6)                      /*!< Verify firmware image */
#define EFUSE_TZCTL_VFCERT              BIT(7)                      /*!< Verify firmware certificate */

/* EFUSE_FPCTL */
#define EFUSE_FPCTL_FP                  BITS(0,7)                   /*!< Efuse flash protection value */

/* EFUSE_USCTL */
#define EFUSE_USCTL_HWDG                BIT(0)                      /*!< Free watchdog timer selection */
#define EFUSE_USCTL_NRSTSTDB            BIT(1)                      /*!< Reset option of entry standby mode */
#define EFUSE_USCTL_NRSTDPSLP           BIT(2)                      /*!< Reset option of entry deep sleep mode */
#define EFUSE_USCTL_EFOPLK              BIT(3)                      /*!< EFUSE_FP_CTL and EFUSE_USER_CTL register lock bit */
#define EFUSE_USCTL_MCUINITLK           BIT(4)                      /*!< EFUSE_MCU_INIT_DATA register lock bit */
#define EFUSE_USCTL_AESEN               BIT(5)                      /*!< Lock EFUSE_AES_KEY register and enable AES decrypt function */
#define EFUSE_USCTL_UDLK                BIT(6)                      /*!< EFUSE_USER_DATA register lock bit */

/* EFUSE_MCUI */
/* EFUSE_AES */
/* EFUSE_ROTPK */
/* EFUSE_DP */
/* EFUSE_PUID */
/* EFUSE_HUK */
/* EFUSE_RF */
/* EFUSE_USER */

/* EFUSE_STZEN */
#define EFUSE_STZEN_STZEN               BIT(0)                      /*!< Enable Trustzone function by software */

/* EFUSE_RSS */

/* EFUSE Macro Type */
#define EFUSE_TYPE_CTL                  0x00
#define EFUSE_TYPE_TZ_CTL               0x01
#define EFUSE_TYPE_FP                   0x02
#define EFUSE_TYPE_US_CTL               0x03
#define EFUSE_TYPE_MCU_INIT             0x04
#define EFUSE_TYPE_AES_KEY              0x05
#define EFUSE_TYPE_ROTPK                0x06
#define EFUSE_TYPE_DBG_PWD              0x07
#define EFUSE_TYPE_RSS                  0x08
#define EFUSE_TYPE_MCU_UID              0x09
#define EFUSE_TYPE_HUK                  0x0a
#define EFUSE_TYPE_RF                   0x0b
#define EFUSE_TYPE_US_DATA              0x0c

/* EFUSE Macro Width */
#define EFUSE_WD_CTL                    0x01
#define EFUSE_WD_TZ_CTL                 0x01
#define EFUSE_WD_FP                     0x01
#define EFUSE_WD_US_CTL                 0x01
#define EFUSE_WD_MCU_INIT               0x0c
#define EFUSE_WD_AES_KEY                0x10
#define EFUSE_WD_ROTPK                  0x20
#define EFUSE_WD_DBG_PWD                0x08
#define EFUSE_WD_RSS                    0x40
#define EFUSE_WD_PUID                   0x10
#define EFUSE_WD_HUK                    0x10
#define EFUSE_WD_RF                     0x30
#define EFUSE_WD_US_DATA                0x20
#define EFUSE_WD_MAX                    EFUSE_WD_RSS

/* EFUSE Macro Address */
#define EFUSE_ADDR_CTL                  0x00
#define EFUSE_ADDR_TZ_CTL               0x01
#define EFUSE_ADDR_FP                   0x02
#define EFUSE_ADDR_US_CTL               0x03
#define EFUSE_ADDR_MCU_INIT             0x04
#define EFUSE_ADDR_AES_KEY              0x10
#define EFUSE_ADDR_ROTPK                0x20
#define EFUSE_ADDR_DBG_PWD              0x40
#define EFUSE_ADDR_RSS                  0x48
#define EFUSE_ADDR_PUID                 0x88
#define EFUSE_ADDR_HUK                  0x98
#define EFUSE_ADDR_RF                   0xa8
#define EFUSE_ADDR_US_DATA              0xd8
#define EFUSE_ADDR_MAX                  0xff

/* EFUSE Macro Register Address */
#define EFUSE_REG_CTL                  (EFUSE + 0x08U)
#define EFUSE_REG_TZCTL                (EFUSE + 0x0CU)
#define EFUSE_REG_FPCTL                (EFUSE + 0x10U)
#define EFUSE_REG_US_CTL               (EFUSE + 0x14U)
#define EFUSE_REG_MCU_INIT             (EFUSE + 0x18U)
#define EFUSE_REG_AES_KEY              (EFUSE + 0x24U)
#define EFUSE_REG_ROTPK                (EFUSE + 0x34U)
#define EFUSE_REG_DBG_PWD              (EFUSE + 0x54U)
#define EFUSE_REG_RSS                  (EFUSE + 0x5CU)
#define EFUSE_REG_PUID                 (EFUSE + 0x9CU)
#define EFUSE_REG_HUK                  (EFUSE + 0xACU)
#define EFUSE_REG_RF                   (EFUSE + 0xBCU)
#define EFUSE_REG_US_DATA              (EFUSE + 0xECU)
#define EFUSE_REG_STZEN                (EFUSE + 0x118U)

/* RFCTL */
#define EFUSE_RFCTL_OFFSET             (EFUSE_WD_RF - 1)
#define IS_CLOCK_FPGA(RFCTL)           (RFCTL & BIT(0))
#define IS_CLOCK_SRC_HXTAL40M(RFCTL)   (RFCTL & BIT(1))
#define NOT_DETECT_ISP(RFCTL)          (RFCTL & BIT(2))

#define EFUSE_ATTR_UNLOCK       0
#define EFUSE_ATTR_LOCKED       1

struct efuse_attr_t {
    uint8_t write_mode;
    uint8_t read_mode;
    uint8_t width;
    uint8_t efuse_addr;
    uint32_t reg_addr;
    uint32_t locked;
};

ErrStatus efuse_get_attr(uint8_t type, struct efuse_attr_t *attr);
ErrStatus efuse_write(uint8_t type, uint8_t offset, uint8_t size, uint8_t *buf);
ErrStatus efuse_read(uint8_t type, uint8_t offset, uint8_t size, uint8_t *buf);
#endif /* GD32W51X_EFUSE_H */
