/*!
    \file    mbl_nsc_api.c
    \brief   MBL non-secure callable API for GD32W51x WiFi SDK

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

#include "stdint.h"
#include "stdio.h"
#include "gd32w51x.h"
#include "rom_export.h"
#include "rom_flash.h"
#include "config_gdm32.h"
#include "arm_cmse.h"
#include "mbl_nsc_api.h"
#include "mbl_status.h"

typedef void (*nsfptr_mutex_t) (int) __attribute__((cmse_nonsecure_call));
static nsfptr_mutex_t get_mutex_nsf;
static nsfptr_mutex_t put_mutex_nsf;

/*!
    \brief      erase flash
    \param[in]  offset: flash erase offset
    \param[in]  len: flash erase length
    \param[out] none
    \retval     result of erase flash (0: erase success, -1: invalid offset or length)
*/
static int flash_patch_erase(uint32_t offset, int len)
{
    if (OBSTAT_NQSPI() == SET) {  /* SIP Flash */
        return rom_flash_erase(offset, len);

    } else {  /* QSPI Flash */
        uint32_t erase_sz = rom_flash_erase_size();
        uint32_t sector_start;

        if (!rom_is_valid_flash_offset(offset)
            || len <= 0 || !rom_is_valid_flash_offset(offset + len - 1)) {
            return -1;
        }
        sector_start = FLASH_START_QSPI + (offset & (~(erase_sz - 1)));
        while (len > 0) {
            __disable_irq();
            rom_qspi_flash_erase_sector(sector_start);
            __enable_irq();
            sector_start += erase_sz;
            len -= erase_sz;
        }
    }
    return 0;
}

/*!
    \brief      write flash
    \param[in]  offset: flash write offset
    \param[in]  data: pointer to buffer write to flash
    \param[in]  len: flash write length
    \param[out] none
    \retval     result of write to flash (0: write success, -1: invalid offset or length)
*/
static int flash_patch_write(uint32_t offset, const void *data, int len)
{
    if (OBSTAT_NQSPI() == SET) {  /* SIP Flash */
        return rom_flash_write_fast(offset, data, len);

    } else {  /* QSPI Flash */
        uint8_t *data_u8 = (uint8_t *)data;
        uint32_t base_addr;
        uint32_t page_offset;
        uint32_t size_to_program;

        if (!rom_is_valid_flash_offset(offset) || data == NULL
            || len <= 0 || !rom_is_valid_flash_offset(offset + len - 1)) {
            return -1;
        }

        base_addr = FLASH_START_QSPI;
        page_offset = (offset & (FLASH_QSPI_PAGE_SIZE - 1));
        if (page_offset != 0) {
            size_to_program = (len > FLASH_QSPI_PAGE_SIZE - page_offset) ? (FLASH_QSPI_PAGE_SIZE - page_offset) : len;
            __disable_irq();
            rom_qspi_flash_program_page((base_addr + offset), data_u8, size_to_program);
            __enable_irq();
            offset += size_to_program;
            data_u8 += size_to_program;
            len -= size_to_program;
        }

        while (len > 0) {
            size_to_program = (len > FLASH_QSPI_PAGE_SIZE) ? FLASH_QSPI_PAGE_SIZE : len;
            __disable_irq();
            rom_qspi_flash_program_page((base_addr + offset), data_u8, size_to_program);
            __enable_irq();
            offset += size_to_program;
            data_u8 += size_to_program;
            len -= size_to_program;
        }
        return 0;
    }
}

/*!
    \brief      read flash indirect
    \param[in]  offset: flash read offset
    \param[out] data: pointer to buffer store data read from flash
    \param[in]  len: flash read length
    \retval     result of read from flash (0 if read success, otherwise non-zero)
*/
static int flash_indirect_patch_read(uint32_t offset, void *data, int len)
{
    int cache_enabled = 0;
    int ret;

    if (ICACHE_CTL & ICACHE_CTL_EN) {
        icache_disable();
        cache_enabled = 1;
    }

    if (OBSTAT_NQSPI() == SET) {  /* SIP Flash */
        /* !!!Please set this area as no decryption by calling fmc_set_nodec */
        ret = rom_flash_read(offset, data, len);

    } else {  /* QSPI Flash */
        if (!rom_is_valid_flash_offset(offset) || data == NULL
            || len <= 0 || !rom_is_valid_flash_offset(offset + len - 1)) {
            ret = -1;
            goto Exit;
        }

        ret = rom_qspi_flash_read(FLASH_START_QSPI + offset, data, len);
    }
Exit:
    if (cache_enabled)
        icache_enable();
    return ret;
}

/*!
    \brief      derive symmetric key
    \param[in]  label: pinter to lable that derive key
    \param[in]  label_sz: size of lable
    \param[out] key: pointer to key
    \param[in]  key_len: length of key
    \retval     result of derive symmetric key
*/
static int symm_key_patch_derive(uint8_t *label, size_t label_sz,
                        uint8_t *key, size_t key_len)
{// TODO
    int i;

    for (i = 0; i < key_len; i++) {
        key[i] = i + label[i % label_sz];
    }
    return 0;
}

/* NS callable */
/*!
    \brief      get mutex function
    \param[in]  type: mutex type
    \param[out] none
    \retval     none
*/
static void sys_mutex_get_func_s(int type)
{
    uint32_t prim = __get_PRIMASK();
    if (prim != 1)
        get_mutex_nsf(type);
}

/*!
    \brief      put mutex function
    \param[in]  type: mutex type
    \param[out] none
    \retval     none
*/
static void sys_mutex_put_func_s(int type)
{
    uint32_t prim = __get_PRIMASK();
    if (prim != 1)
        put_mutex_nsf(type);
}

/* NS callable */
/*!
    \brief      set mutex function
    \param[in]  tolock: lock or unlock flag
    \param[in]  func: pointer to nonsecure function
    \param[out] none
    \retval     none
*/
__attribute__((cmse_nonsecure_entry))
void mbl_mutex_set_func(int tolock, void *func)
{
    if (tolock) {
        get_mutex_nsf = (nsfptr_mutex_t)cmse_nsfptr_create(func);
        rom_set_mutex_func(1, (void *)sys_mutex_get_func_s);
    } else {
        put_mutex_nsf = (nsfptr_mutex_t)cmse_nsfptr_create(func);
        rom_set_mutex_func(0, (void *)sys_mutex_put_func_s);
    }
}

/* NS callable */
/*!
    \brief      mbl read flash indirect
    \param[in]  offset: flash offset
    \param[out] buf: pointer to buffer store data read from flash
    \param[in]  sz: size of data read from flash
    \retval     result of read from flash
*/
__attribute__((cmse_nonsecure_entry))
int mbl_flash_indirect_read(uint32_t offset, uint8_t *buf, uint32_t sz)
{
    return flash_indirect_patch_read(offset, buf, sz);
}

/*!
    \brief      mbl write flash
    \param[in]  offset: flash offset
    \param[in]  buf: pointer to buffer store data  write to flash
    \param[in]  sz: size of data write flash
    \param[out] none
    \retval     result of write to flash
*/
__attribute__((cmse_nonsecure_entry))
int mbl_flash_write(uint32_t offset, uint8_t *buf, uint32_t sz)
{
    return flash_patch_write(offset, buf, sz);
}

/*!
    \brief      mbl erase flash
    \param[in]  offset: flash offset
    \param[in]  sz: size of erase flash
    \param[out] none
    \retval     result of erase flash
*/
__attribute__((cmse_nonsecure_entry))
int mbl_flash_erase(uint32_t offset, uint32_t sz)
{
    return flash_patch_erase(offset, sz);
}

/*!
    \brief      mbl set system status
    \param[in]  type: type of system status to set
    \param[in]  len: length of system status to set
    \param[in]  pval: pointer to the value of system status to set
    \param[out] none
    \retval     result of set system status
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_status_set(uint8_t type, uint8_t len, uint8_t *pval)
{
    return rom_sys_status_set(type, len, pval);
}

/*!
    \brief      mbl get system status
    \param[in]  type: type of get
    \param[in]  len: length of get
    \param[out] pval: pointer to the value of system status to get
    \retval     result of get system status
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_status_get(uint8_t type, uint8_t len, uint8_t* pval)
{
    return rom_sys_status_get(type, len, pval);
}

/*!
    \brief      mbl set error status
    \param[in]  method: type of method
    \param[out] none
    \retval     result of set error status
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_set_err_process(uint8_t method)
{
    return rom_sys_set_err_process(method);
}

/*!
    \brief      mbl set system trace level
    \param[in]  trace_level: trace level of system to set
    \param[out] none
    \retval     status of set system trace level
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_trace_level_set(uint8_t trace_level)
{
    return rom_sys_set_trace_level(trace_level);
}

/*!
    \brief      mbl set system image flag
    \param[in]  inx: image index
    \param[in]  mask: image status mask value
    \param[in]  flag: image flag
    \param[out] none
    \retval     status of set image flag
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_img_flag_set(uint8_t idx, uint8_t mask, uint8_t flag)
{
    rom_sys_set_img_flag(idx, mask, flag);
    return 0;
}

/*!
    \brief      mbl set system running image
    \param[in]  idx: image index
    \param[out] none
    \retval     status of set running image
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_running_img_set(uint8_t idx)
{
    return rom_sys_set_running_img(idx);
}

/*!
    \brief      mbl set system firmware version
    \param[in]  type: image type
    \param[in]  version: image version
    \param[out] none
    \retval     status of set firmware version
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_fw_version_set(uint32_t type, uint32_t version)
{
    rom_sys_set_fw_ver(type, version);
    return 0;
}

/*!
    \brief      mbl set system trng seed
    \param[in]  val: trng seed value
    \param[out] none
    \retval     status of set system trng
*/
__attribute__((cmse_nonsecure_entry))
int mbl_sys_trng_seed_set(uint8_t val)
{
    return rom_sys_set_trng_seed(val);
}
#if 0
/*!
    \brief      get efuse control non-secure callable function
    \param[in]  none
    \param[out] none
    \retval     result of get efuse control
*/
__attribute__((cmse_nonsecure_entry))
uint8_t efuse_ctl_get_nsc(void)
{
    return rom_efuse_get_ctl();
}

/*!
    \brief      set efuse control non-secure callable function
    \param[in]  ctl: control
    \param[out] none
    \retval     status of efuse control
*/
__attribute__((cmse_nonsecure_entry))
int efuse_ctl_set_nsc(uint8_t ctl)
{
    return rom_efuse_set_ctl(ctl);
}

/*!
    \brief      get efuse trust zone control non-secure callable function
    \param[in]  none
    \param[out] none
    \retval     result of get efuse trust zone control
*/
__attribute__((cmse_nonsecure_entry))
uint8_t efuse_tzctl_get_nsc(void)
{
    return rom_efuse_get_tzctl();
}

/*!
    \brief      set efuse trust zone control non-secure callable function
    \param[in]  tzctl: trust zone control
    \param[out] none
    \retval     status of set efuse trust zone control
*/
__attribute__((cmse_nonsecure_entry))
int efuse_tzctl_set_nsc(uint8_t tzctl)
{
    return rom_efuse_set_tzctl(tzctl);
}

/*!
    \brief      get efuse flash protection non-secure callable function
    \param[in]  none
    \param[out] none
    \retval     result of get efuse flash protection non-secure callable function
*/
__attribute__((cmse_nonsecure_entry))
uint8_t efuse_fp_get_nsc(void)
{
    return rom_efuse_get_fp();
}

/*!
    \brief      set efuse flash protection non-secure callable function
    \param[in]  fp: flash protection
    \param[out] none
    \retval     status of set efuse flash protection
*/
__attribute__((cmse_nonsecure_entry))
int efuse_fp_set_nsc(uint8_t fp)
{
    return rom_efuse_set_fp(fp);
}

/*!
    \brief      get efuse user control non-secure callable function
    \param[in]  none
    \param[out] none
    \retval     result of get efuse user control
*/
__attribute__((cmse_nonsecure_entry))
uint8_t efuse_usctl_get_nsc(void)
{
    return rom_efuse_get_usctl();
}

/*!
    \brief      set efuse user control non-secure callable function
    \param[in]  usctl: user control
    \param[out] none
    \retval     result of set efuse user control
*/
__attribute__((cmse_nonsecure_entry))
int efuse_usctl_set_nsc(uint8_t usctl)
{
    return rom_efuse_set_usctl(usctl);
}

/*!
    \brief      get efuse mcu id non-secure callable function
    \param[in]  none
    \param[out] mcui: pointer to mcu id
    \retval     status of get efuse mcu id
*/
__attribute__((cmse_nonsecure_entry))
int efuse_mcui_get_nsc(uint8_t *mcui)
{
    return rom_efuse_get_mcui(mcui);
}

/*!
    \brief      set efuse mcu id non-secure callable function
    \param[in]  mcui: pointer to mcu id
    \param[out] none
    \retval     status of set efuse mcu id
*/
__attribute__((cmse_nonsecure_entry))
int efuse_mcui_set_nsc(uint8_t *mcui)
{
    return rom_efuse_set_mcui(mcui);
}

/*!
    \brief      get efuse aes key non-secure callable function
    \param[in]  none
    \param[out] aeskey: pointer to aes key
    \retval     status of get efuse aes key
*/
__attribute__((cmse_nonsecure_entry))
int efuse_aeskey_get_nsc(uint8_t *aeskey)
{
    return rom_efuse_get_aeskey(aeskey);
}

/*!
    \brief      set efuse aes key non-secure callable function
    \param[in]  aeskey: pointer to aes key
    \param[out] none
    \retval     status of set efuse aes key non-secure callable function
*/
__attribute__((cmse_nonsecure_entry))
int efuse_aeskey_set_nsc(uint8_t *aeskey)
{
    return rom_efuse_set_aeskey(aeskey);
}

/*!
    \brief      get efuse root of trust public key non-secure callable function
    \param[in]  none
    \param[out] rotpk: pointer to root of trust public key
    \retval     status of get efuse root of trust public key
*/
__attribute__((cmse_nonsecure_entry))
int efuse_rotpk_get_nsc(uint8_t *rotpk)
{
    return rom_efuse_get_rotpk(rotpk);
}

/*!
    \brief      set efuse root of trust public key non-secure callable function
    \param[in]  *rotpk: pointer to root of trust public key
    \param[out] none
    \retval     status of set efuse root of trust public key
*/
__attribute__((cmse_nonsecure_entry))
int efuse_set_rotpk_nsc(uint8_t *rotpk)
{
    return rom_efuse_set_rotpk(rotpk);
}

/*!
    \brief      get efuse debug pass word non-secure callable function
    \param[in]  none
    \param[out] pwd: pointer to pass word
    \retval     status of get efuse pass word
*/
__attribute__((cmse_nonsecure_entry))
int efuse_dbg_pwd_get_nsc(uint8_t *pwd)
{
    return rom_efuse_get_dbg_pwd(pwd);
}

/*!
    \brief      set efuse debug pass word non-secure callable function
    \param[in]  pwd: pointer to pass word to set
    \param[out] none
    \retval     status of set efuse pass word
*/
__attribute__((cmse_nonsecure_entry))
int efuse_dbg_pwd_set_nsc(uint8_t *pwd)
{
    return rom_efuse_set_dbg_pwd(pwd);
}

/*!
    \brief      get efuse rss non-secure callable function
    \param[in]  none
    \param[out] rss: pointer to rss to get
    \retval     status of get efuse rss
*/
__attribute__((cmse_nonsecure_entry))
int efuse_rss_get_nsc(uint8_t *rss)
{
    return rom_efuse_get_rss(rss);
}

/*!
    \brief      set efuse rss non-secure callable function
    \param[in]  rss: pointer to rss
    \param[out] none
    \retval     status of set efuse rss
*/
__attribute__((cmse_nonsecure_entry))
int efuse_rss_set_nsc(uint8_t *rss)
{
    return rom_efuse_set_rss(rss);
}

/*!
    \brief      get efuse uid non-secure callable function
    \param[in]  none
    \param[out] uid: pointer to uid
    \retval     status of get efuse uid
*/
__attribute__((cmse_nonsecure_entry))
int efuse_uid_get_nsc(uint8_t *uid)
{
    return rom_efuse_get_uid(uid);
}

/*!
    \brief      get efuse rf parameters non-secure callable function
    \param[out] rf: pointer to rf
    \param[in]  offset: offset of rf
    \param[in]  sz: size of rf
    \retval     status of get efuse rf
*/
__attribute__((cmse_nonsecure_entry))
int efuse_rf_get_nsc(uint8_t *rf, uint32_t offset, uint32_t sz)
{
    return rom_efuse_get_rf(rf, offset, sz);
}

/*!
    \brief      set efuse rf parameters non-secure callable function
    \param[in]  rf: pointer to rf
    \param[in]  offset: offset of rf
    \param[in]  sz: size of rf
    \param[out] none
    \retval     status of set efuse rf
*/
__attribute__((cmse_nonsecure_entry))
int efuse_rf_set_nsc(uint8_t *rf, uint32_t offset, uint32_t sz)
{
    return rom_efuse_set_rf(rf, offset, sz);
}

/*!
    \brief      get efuse user data non-secure callable function
    \param[out] usdata: pointer to user data
    \param[in]  offset: offset of user data
    \param[in]  sz: size of user data
    \retval     status of get efuse user data
*/
__attribute__((cmse_nonsecure_entry))
int efuse_usdata_get_nsc(uint8_t *usdata, uint32_t offset, uint32_t sz)
{
    return rom_efuse_get_usdata(usdata, offset, sz);
}

/*!
    \brief      set efuse user data non-secure callable function
    \param[in]  usdata: pointer to user data
    \param[in]  offset: offset of user data
    \param[in]  sz: size of user data
    \param[out] none
    \retval     status of set efuse user data
*/
__attribute__((cmse_nonsecure_entry))
int efuse_usdata_get_nsc(uint8_t *usdata, uint32_t offset, uint32_t sz)
{
    return rom_efuse_set_usdata(usdata, offset, sz);
}
#endif
/*!
    \brief      get iak public key callable function
    \param[in]  none
    \param[out] output: pointer to output value
    \param[out] len: pointer to length
    \retval     result of get iak public key
*/
__attribute__((cmse_nonsecure_entry))
int iak_pub_get_nsc(uint8_t *output, uint32_t *len)
{
    return rom_do_iak_getpub(2, 0, 1, 0, output, len);
}

/*!
    \brief      sign iak callable function
    \param[in]  hash: pointer to hash value
    \param[in]  hash_len: hash length
    \param[out] sig: pointer to sign
    \param[out] sig_len: pointer to sign length
    \retval     status of sign iak
*/
__attribute__((cmse_nonsecure_entry))
int iak_sign_nsc(uint8_t *hash, uint32_t hash_len, uint8_t *sig, uint32_t *sig_len)
{
    return rom_do_iak_sign(2, 0, 1, hash, hash_len, sig, sig_len);
}

/*!
    \brief      derive key non-secure callable function
    \param[in]  label: pointer to lable
    \param[in]  label_len: lable length
    \param[out] key: pointer to key
    \param[in]  key_len: key length
    \retval     status of derive key
*/
__attribute__((cmse_nonsecure_entry))
int key_derive_nsc(uint8_t *label, size_t label_len, uint8_t *key, size_t key_len)
{
    uint32_t rom_version;

    mbl_status_rom_version_get((uint8_t *)&rom_version);
    if (rom_version == V_1_0) {
        rom_trace_ex(ROM_INFO, "Rom Version: 1.0\r\n");
        return symm_key_patch_derive(label, label_len, key, key_len);
    } else {
        rom_trace_ex(ROM_INFO, "Rom Version: %d.%d\r\n", (rom_version >> 8), (rom_version & 0xFF));
        return rom_do_symm_key_derive(label, label_len, key, key_len);
    }
}

/*!
    \brief      hmac sha1 non-secure callable function
    \param[in]  in_vec: pointer to in vector
    \param[in]  in_len: in length
    \param[out] out: pointer to out
    \retval     status of hmac sha1
*/
__attribute__((cmse_nonsecure_entry))
int hmac_sha1_nsc(param_vec *in_vec, uint32_t in_len, uint8_t *out)
{
    int ROM_HMAC_SHA1(uint8_t *, uint32_t, uint8_t *, uint32_t, uint8_t *);
    uint8_t *Key = in_vec[0].base;
    uint32_t Keylen = in_vec[0].len;
    uint8_t *Input = in_vec[1].base;
    uint32_t Ilen = in_vec[1].len;
    uint8_t* Output = out;
    ROM_HMAC_SHA1(Key, Keylen, Input, Ilen, Output);
    return 0;
}

/*!
    \brief      cryp aes ecb non-secure callable function
    \param[in]  in_vec: pointer to in vector
    \param[in]  in_len: in length
    \param[out]  out: pointer to out
    \retval     status of cryp aes ecb
*/
__attribute__((cmse_nonsecure_entry))
int cryp_aes_ecb_nsc(param_vec *in_vec, uint32_t in_len, uint8_t *out)
{
    int ROM_CRYP_AES_ECB(uint8_t, uint8_t *, uint16_t, uint8_t *, uint32_t, uint8_t *);
    uint8_t Mode = *(uint8_t *)in_vec[0].base;
    uint8_t* Key = in_vec[1].base;
    uint16_t Keysize = in_vec[1].len;
    uint8_t* Input = in_vec[2].base;
    uint32_t Ilength = in_vec[2].len;
    uint8_t* Output = out;
    ROM_CRYP_AES_ECB(Mode, Key, Keysize, Input, Ilength, Output);
    return 0;
}


#if 0
/* NS callable */
#define MAX_LINE_LENGTH_BYTES (64)
#define DEFAULT_LINE_LENGTH_BYTES (16)
/*!
    \brief      print buffer
    \param[in]  addr: address of buffer to print
    \param[in]  width: print width
    \param[in]  count: print count
    \param[in]  linelen: print line length
    \param[out] none
    \retval     result of print buffer
*/
__attribute__((cmse_nonsecure_entry))
int buffer_print(uint32_t addr, uint32_t width, uint32_t count, uint32_t linelen)
{
    uint8_t linebuf[MAX_LINE_LENGTH_BYTES];
    uint32_t *uip = (void *)linebuf;
    uint16_t *usp = (void *)linebuf;
    uint8_t *ucp = (void *)linebuf;
    uint8_t *pdata = (uint8_t *)addr;
    int i;

    if (linelen*width > MAX_LINE_LENGTH_BYTES)
        linelen = MAX_LINE_LENGTH_BYTES / width;
    if (linelen < 1)
        linelen = DEFAULT_LINE_LENGTH_BYTES / width;

    while (count) {
        printf("%08x:", addr);

        /* check for overflow condition */
        if (count < linelen)
            linelen = count;

        /* Copy from memory into linebuf and print hex values */
        for (i = 0; i < linelen; i++) {
            if (width == 4) {
                uip[i] = *(volatile unsigned long *)pdata;
                printf(" %08x", uip[i]);
            } else if (width == 2) {
                usp[i] = *(volatile unsigned short *)pdata;
                printf(" %04x", usp[i]);
            } else {
                ucp[i] = *(volatile unsigned char *)pdata;
                printf(" %02x", ucp[i]);
            }
            pdata += width;
        }

        /* update references */
        addr += linelen * width;
        count -= linelen;
    }

    return 0;
}
#endif
