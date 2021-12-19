/*!
    \file    gd32w51x_efuse.c
    \brief   EFUSE driver

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

#include "gd32w51x_efuse.h"

/*write mode*/
#define EFUSE_NON_WRITEABLE             0
#define EFUSE_BYTE_WRITEABLE            1
#define EFUSE_WIDTH_WRITEABLE           2


/* read mode*/
#define EFUSE_BUS_READABLE              0
#define EFUSE_BYTE_READABLE             1
#define EFUSE_WIDTH_READABLE            2

#define ALIGN_BYTES         sizeof(uint32_t)

/*!
    \brief      get the efuse attribute by type
    \param[in]  type
      \arg        EFUSE_TYPE_CTL     : Efuse control
      \arg        EFUSE_TYPE_TZ_CTL  : Efuse trustzone control
      \arg        EFUSE_TYPE_FP      : Efuse flash protection
      \arg        EFUSE_TYPE_US_CTL  : Efuse user control
      \arg        EFUSE_TYPE_MCU_INIT: Efuse MCU initialization parameters
      \arg        EFUSE_TYPE_AES_KEY : Efuse AES key
      \arg        EFUSE_TYPE_ROTPK   : Efuse ROTPK
      \arg        EFUSE_TYPE_DBG_PWD : Efuse debug password
      \arg        EFUSE_TYPE_RSS     : Efuse RSS signature
      \arg        EFUSE_TYPE_MCU_UID : Efuse MCU UID
      \arg        EFUSE_TYPE_HUK     : Efuse HUK
      \arg        EFUSE_TYPE_RF      : Efuse RF
      \arg        EFUSE_TYPE_US_DATA : Efuse user data
    \param[out]  attr : the pointer of efuse attribute
    \retval     status
*/
ErrStatus efuse_get_attr(uint8_t type, struct efuse_attr_t *attr)
{
    ErrStatus status = SUCCESS;

    switch (type) {
        case EFUSE_TYPE_CTL:
            attr->reg_addr = EFUSE_REG_CTL;
            attr->efuse_addr = EFUSE_ADDR_CTL;
            attr->width = EFUSE_WD_CTL;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_WIDTH_READABLE;
            attr->locked = (EFUSE_CTL & EFUSE_CTL_EFBOOTLK) ? EFUSE_ATTR_LOCKED : EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_TZ_CTL:
            attr->reg_addr = EFUSE_REG_TZCTL;
            attr->efuse_addr = EFUSE_ADDR_TZ_CTL;
            attr->width = EFUSE_WD_CTL;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->locked = EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_FP:
            attr->reg_addr = EFUSE_REG_FPCTL;
            attr->efuse_addr = EFUSE_ADDR_FP;
            attr->width = EFUSE_WD_CTL;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->locked = (EFUSE_USCTL & EFUSE_USCTL_EFOPLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_US_CTL:
            attr->reg_addr = EFUSE_REG_US_CTL;
            attr->efuse_addr = EFUSE_ADDR_US_CTL;
            attr->width = EFUSE_WD_US_CTL;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->locked = EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_MCU_INIT:
            attr->reg_addr = EFUSE_REG_MCU_INIT;
            attr->efuse_addr = EFUSE_ADDR_MCU_INIT;
            attr->width = EFUSE_WD_MCU_INIT;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->locked = (EFUSE_USCTL & EFUSE_USCTL_MCUINITLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_AES_KEY:
            attr->reg_addr = EFUSE_REG_AES_KEY;
            attr->efuse_addr = EFUSE_ADDR_AES_KEY;
            attr->width = EFUSE_WD_AES_KEY;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->locked = (EFUSE_USCTL & EFUSE_USCTL_AESEN) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_ROTPK:
            attr->reg_addr = EFUSE_REG_ROTPK;
            attr->efuse_addr = EFUSE_ADDR_ROTPK;
            attr->width = EFUSE_WD_ROTPK;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_WIDTH_READABLE;
            attr->locked = (EFUSE_TZCTL & EFUSE_TZCTL_ROTLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_DBG_PWD:
            attr->reg_addr = EFUSE_REG_DBG_PWD;
            attr->efuse_addr = EFUSE_ADDR_DBG_PWD;
            attr->width = EFUSE_WD_DBG_PWD;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_WIDTH_READABLE;
            attr->locked = (EFUSE_TZCTL & EFUSE_TZCTL_DPLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_RSS:
            attr->reg_addr = EFUSE_REG_RSS;
            attr->efuse_addr = EFUSE_ADDR_RSS;
            attr->width = EFUSE_WD_RSS;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_WIDTH_READABLE;
            attr->locked = (EFUSE_TZCTL & EFUSE_TZCTL_RSSLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_MCU_UID:
            attr->reg_addr = EFUSE_REG_PUID;
            attr->efuse_addr = EFUSE_ADDR_PUID;
            attr->width = EFUSE_WD_PUID;
            attr->write_mode = EFUSE_NON_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->locked = EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_HUK:
            attr->reg_addr = EFUSE_REG_HUK;
            attr->efuse_addr = EFUSE_ADDR_HUK;
            attr->write_mode = EFUSE_NON_WRITEABLE;
            attr->read_mode = EFUSE_BUS_READABLE;
            attr->width = EFUSE_WD_HUK;
            attr->locked = EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_RF:
            attr->reg_addr = EFUSE_REG_RF;
            attr->efuse_addr = EFUSE_ADDR_RF;
            attr->width = EFUSE_WD_RF;
            attr->write_mode = EFUSE_BYTE_WRITEABLE;
            attr->read_mode = EFUSE_BYTE_READABLE;
            attr->locked = (EFUSE_TZCTL & EFUSE_TZCTL_RFLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        case EFUSE_TYPE_US_DATA :
            attr->reg_addr = EFUSE_REG_US_DATA;
            attr->efuse_addr = EFUSE_ADDR_US_DATA;
            attr->width = EFUSE_WD_US_DATA;
            attr->write_mode = EFUSE_WIDTH_WRITEABLE;
            attr->read_mode = EFUSE_WIDTH_READABLE;
            attr->locked = (EFUSE_USCTL & EFUSE_USCTL_UDLK) ? EFUSE_ATTR_LOCKED: EFUSE_ATTR_UNLOCK;
            break;
        default:
            status = ERROR;
            break;
    }

    return status;
}

/*!
    \brief      write data to bus by register address
    \param[in]  reg_addr : the register address
    \param[in]  offset : the offset bytes
    \param[in]  size : the size of bytes to write
    \param[in]  buf : the data to write
    \param[out] none
    \retval     status
*/

void efuse_bus_write(uint32_t reg_addr, uint8_t offset, uint8_t size, uint8_t *buf)
{
    uint32_t reg_data;
    uint8_t i;
    uint8_t j;
    uint8_t *pdata;
    uint8_t *pbuf;

    pbuf = buf;
    pdata = (uint8_t *) &reg_data;

    if (offset > 0) {
        reg_addr = reg_addr + ((offset >> 2) << 2);
        offset = offset & (ALIGN_BYTES - 1);

        j = ALIGN_BYTES - offset;
        j = (size > j) ? j : size;

        reg_data = REG32(reg_addr);

        for (i = 0; i < j; i++) {
            pdata[i + offset] = *pbuf++;
        }

        REG32(reg_addr) = reg_data;

        reg_addr += ALIGN_BYTES;
        size -= j;
    }

    j = size >> 2;

    if (j > 0) {
        for (i = 0; i < j; i++) {
            reg_data = *((uint32_t *) pbuf);
            REG32(reg_addr) = reg_data;

            reg_addr += ALIGN_BYTES;
            pbuf += ALIGN_BYTES;
            size -= ALIGN_BYTES;
        }
    }

    if (size > 0) {
        reg_data = REG32(reg_addr);

        for (i = 0; i < size; i++) {
            pdata[i] = *pbuf++;
        }

        REG32(reg_addr) = reg_data;
    }
}

/*!
    \brief      write bytes data to efuse
    \param[in]  attr : the pointer of efuse attribute
    \param[in]  offset : the offset bytes from efuse macro address
    \param[in]  size : the size of bytes to write
    \param[in]  buf : the data to write
    \param[out] none
    \retval     status
*/
void efuse_bytes_write(struct efuse_attr_t *attr, uint8_t offset, uint8_t size, uint8_t *buf)
{
    uint8_t i;


    for (i = 0; i < size; i++) {
        EFUSE_CS = EFUSE_CS & (~EFUSE_CS_PGIF);
        EFUSE_CS = EFUSE_CS | EFUSE_CS_EFRW;
        EFUSE_ADDR = ((attr->efuse_addr + offset + i) & EFUSE_ADDR_EFADDR) | ((1 << 8) & EFUSE_ADDR_EFSIZE);

        efuse_bus_write(attr->reg_addr, offset + i, 1, buf + i);

        EFUSE_CS = EFUSE_CS | EFUSE_CS_EFSTR;

        while(!(EFUSE_CS & EFUSE_CS_PGIF));

        EFUSE_CS = EFUSE_CS | EFUSE_CS_PGIC;
    }
}


/*!
    \brief      write width data to efuse
    \param[in]  attr : the pointer of efuse attribute
    \param[in]  offset : the offset bytes from efuse macro address
    \param[in]  size : the size of bytes to write
    \param[in]  buf : the data to write
    \param[out] none
    \retval     status
*/
void efuse_width_write(struct efuse_attr_t *attr, uint8_t offset, uint8_t size, uint8_t *buf)
{
    EFUSE_CS = EFUSE_CS & (~EFUSE_CS_PGIF);
    EFUSE_CS = EFUSE_CS | EFUSE_CS_EFRW;
    EFUSE_ADDR = ((attr->efuse_addr + offset) & EFUSE_ADDR_EFADDR) | ((size << 8) & EFUSE_ADDR_EFSIZE);

    efuse_bus_write(attr->reg_addr, offset, size, buf);

    EFUSE_CS = EFUSE_CS | EFUSE_CS_EFSTR;

    while(!(EFUSE_CS & EFUSE_CS_PGIF));

    EFUSE_CS = EFUSE_CS | EFUSE_CS_PGIC;

}

/*!
    \brief      read bus data to buffer by register address
    \param[in]  reg_addr : the register address
    \param[in]  offset : the offset bytes
    \param[in]  size : the size of bytes to write
    \param[out] buf : the data to write
    \retval     status
*/

void efuse_bus_read(uint32_t reg_addr, uint8_t offset, uint8_t size, uint8_t *buf)
{
    uint32_t reg_data;
    uint8_t i;
    uint8_t j;
    uint8_t *pdata;
    uint8_t *pbuf;

    pbuf = buf;
    pdata = (uint8_t *) &reg_data;

    if (offset > 0) {
        reg_addr = reg_addr+ ((offset >> 2) << 2);
        offset = offset & (ALIGN_BYTES - 1);

        j = ALIGN_BYTES - offset;
        j = (size > j) ? j : size;
        reg_data = REG32(reg_addr);

        for (i = 0; i < j; i++) {
            *pbuf++ = pdata[i + offset];
        }

        reg_addr += ALIGN_BYTES;
        size -= j;
    }

    j = size >> 2;

    if (j > 0) {
        for (i = 0; i < j; i++) {
            reg_data = REG32(reg_addr);
            *((uint32_t *) pbuf) = reg_data;

            reg_addr += ALIGN_BYTES;
            pbuf += ALIGN_BYTES;
            size -= ALIGN_BYTES;
        }
    }

    if (size > 0) {
        reg_data = REG32(reg_addr);

        for (i = 0; i < size; i++) {
            *pbuf++ = pdata[i];
        }
    }
}


/*!
    \brief      macro bytes read efuse data to buffer
    \param[in]  attr : the pointer of efuse attribute
    \param[in]  offset : the offset bytes from efuse macro address
    \param[in]  size : the size of bytes to write
    \param[out] buf : the data to write
    \retval     status
*/
void efuse_macro_bytes_read(struct efuse_attr_t *attr, uint8_t offset, uint8_t size, uint8_t *buf)
{
    uint8_t i;

    for (i = 0; i < size; i++) {
        EFUSE_CS = EFUSE_CS & (~EFUSE_CS_RDIF);
        EFUSE_CS = EFUSE_CS & (~EFUSE_CS_EFRW);
        EFUSE_ADDR = ((attr->efuse_addr + offset + i) & EFUSE_ADDR_EFADDR) | ((1 << 8) & EFUSE_ADDR_EFSIZE);
        EFUSE_CS = EFUSE_CS | EFUSE_CS_EFSTR;

        while(!(EFUSE_CS & EFUSE_CS_RDIF));

        EFUSE_CS = EFUSE_CS | EFUSE_CS_RDIC;

        efuse_bus_read(attr->reg_addr, offset + i, 1, buf + i);
    }
}

/*!
    \brief      macro width read efuse data to buffer
    \param[in]  attr : the pointer of efuse attribute
    \param[in]  offset : the offset bytes from efuse macro address
    \param[in]  size : the size of bytes to write
    \param[out] buf : the data to write
    \retval     status
*/
void efuse_macro_width_read(struct efuse_attr_t *attr, uint8_t offset, uint8_t size, uint8_t *buf)
{
    EFUSE_CS = EFUSE_CS & (~EFUSE_CS_RDIF);
    EFUSE_CS = EFUSE_CS & (~EFUSE_CS_EFRW);
    EFUSE_ADDR = ((attr->efuse_addr + offset) & EFUSE_ADDR_EFADDR) | ((size << 8) & EFUSE_ADDR_EFSIZE);
    EFUSE_CS = EFUSE_CS | EFUSE_CS_EFSTR;

    while(!(EFUSE_CS & EFUSE_CS_RDIF));

    EFUSE_CS = EFUSE_CS | EFUSE_CS_RDIC;

    efuse_bus_read(attr->reg_addr, offset, size, buf);
}

/*!
    \brief      write data to efuse by efuse type
    \param[in]  macro
      \arg        EFUSE_TYPE_CTL     : Efuse control
      \arg        EFUSE_TYPE_TZ_CTL  : Efuse trustzone control
      \arg        EFUSE_TYPE_FP      : Efuse flash protection
      \arg        EFUSE_TYPE_US_CTL  : Efuse user control
      \arg        EFUSE_TYPE_MCU_INIT: Efuse MCU initialization parameters
      \arg        EFUSE_TYPE_AES_KEY : Efuse AES key
      \arg        EFUSE_TYPE_ROTPK   : Efuse ROTPK
      \arg        EFUSE_TYPE_DBG_PWD : Efuse debug password
      \arg        EFUSE_TYPE_RSS     : Efuse RSS signature
      \arg        EFUSE_TYPE_MCU_UID : Efuse MCU UID
      \arg        EFUSE_TYPE_HUK     : Efuse HUK
      \arg        EFUSE_TYPE_RF      : Efuse RF
      \arg        EFUSE_TYPE_US_DATA : Efuse user data
    \param[in]  offset : the offset bytes from efuse macro address
    \param[in]  size : the size of bytes to writes
    \param[in]  buf : the data to write
    \param[out] none
    \retval     status
*/
ErrStatus efuse_write(uint8_t type, uint8_t offset, uint8_t size, uint8_t *buf)
{
    struct efuse_attr_t attr;

    if (efuse_get_attr(type, &attr) != SUCCESS)
        return ERROR;

    if (attr.locked == EFUSE_ATTR_LOCKED)
        return ERROR;

    if ((offset + size) > attr.width)
        return ERROR;

    if (attr.write_mode == EFUSE_BYTE_WRITEABLE) {

        efuse_bytes_write(&attr, offset, size, buf);

    } else if (attr.write_mode == EFUSE_WIDTH_WRITEABLE) {

        efuse_width_write(&attr, offset, size, buf);

    } else {
        return ERROR;
    }

    return SUCCESS;
}

/*!
    \brief      read efuse data to buffer by efuse type
    \param[in]  type
      \arg        EFUSE_TYPE_CTL     : Efuse control
      \arg        EFUSE_TYPE_TZ_CTL  : Efuse trustzone control
      \arg        EFUSE_TYPE_FP      : Efuse flash protection
      \arg        EFUSE_TYPE_US_CTL  : Efuse user control
      \arg        EFUSE_TYPE_MCU_INIT: Efuse MCU initialization parameters
      \arg        EFUSE_TYPE_AES_KEY : Efuse AES key
      \arg        EFUSE_TYPE_ROTPK   : Efuse ROTPK
      \arg        EFUSE_TYPE_DBG_PWD : Efuse debug password
      \arg        EFUSE_TYPE_RSS     : Efuse RSS signature
      \arg        EFUSE_TYPE_MCU_UID : Efuse MCU UID
      \arg        EFUSE_TYPE_HUK     : Efuse HUK
      \arg        EFUSE_TYPE_RF      : Efuse RF
      \arg        EFUSE_TYPE_US_DATA : Efuse user data
    \param[in]  offset : the offset bytes from efuse macro address
    \param[in]  size : the size of bytes to read
    \param[out]  buf : read out data to the buffer
    \retval     status
*/

ErrStatus efuse_read(uint8_t type, uint8_t offset, uint8_t size, uint8_t *buf)
{
    struct efuse_attr_t attr;

    if (efuse_get_attr(type, &attr) != SUCCESS)
        return ERROR;

    if ((offset + size) > attr.width)
        return ERROR;

    if (attr.locked == EFUSE_ATTR_UNLOCK) {
        if (attr.read_mode == EFUSE_BYTE_READABLE) {
            efuse_macro_bytes_read(&attr, offset, size, buf);
        } else if (attr.read_mode == EFUSE_WIDTH_READABLE) {
            efuse_macro_width_read(&attr, offset, size, buf);
        } else {
            efuse_bus_read(attr.reg_addr, offset, size, buf);
        }
    } else {
        efuse_bus_read(attr.reg_addr, offset, size, buf);
    }


    return SUCCESS;
}
