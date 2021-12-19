/*!
    \file    mbl_nsc_api.h
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

#ifndef __MBL_NSC_API_H__
#define __MBL_NSC_API_H__

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

#undef cmse_nsfptr_create
#define cmse_nsfptr_create(p) ((intptr_t) (p) & ~1)

typedef struct param_vec {
    void *base;                 /*!< the start address of the memory buffer */
    size_t len;                 /*!< the size in bytes                      */
} param_vec;

void mbl_mutex_set_func(int tolock, void *func);
int mbl_flash_indirect_read(uint32_t offset, uint8_t *buf, uint32_t sz);
int mbl_flash_write(uint32_t offset, uint8_t *buf, uint32_t sz);
int mbl_flash_erase(uint32_t offset, uint32_t sz);
int mbl_sys_status_set(uint8_t type, uint8_t len, uint8_t *pval);
int mbl_sys_status_get(uint8_t type, uint8_t len, uint8_t* pval);
int mbl_sys_set_err_process(uint8_t method);
int mbl_sys_trace_level_set(uint8_t trace_level);
int mbl_sys_img_flag_set(uint8_t idx, uint8_t mask, uint8_t flag);
int mbl_sys_img_flag_reset(uint8_t idx);
int mbl_sys_running_img_set(uint8_t idx);
int mbl_sys_fw_version_set(uint32_t type, uint32_t version);
int mbl_sys_trng_seed_set(uint8_t val);
#if 0
uint8_t efuse_ctl_get_nsc(void);
int efuse_ctl_set_nsc(uint8_t ctl);
uint8_t efuse_tzctl_get_nsc(void);
int efuse_tzctl_set_nsc(uint8_t tzctl);
uint8_t efuse_fp_get_nsc(void);
int efuse_fp_set_nsc(uint8_t fp);
uint8_t efuse_usctl_get_nsc(void);
int efuse_usctl_set_nsc(uint8_t usctl);
int efuse_mcui_get_nsc(uint8_t *mcui);
int efuse_mcui_set_nsc(uint8_t *mcui);
int efuse_aeskey_get_nsc(uint8_t *aeskey);
int efuse_aeskey_set_nsc(uint8_t *aeskey);
int efuse_rotpk_get_nsc(uint8_t *rotpk);
int efuse_set_rotpk_nsc(uint8_t *rotpk);
int efuse_dbg_pwd_get_nsc(uint8_t *pwd);
int efuse_dbg_pwd_set_nsc(uint8_t *pwd);
int efuse_rss_get_nsc(uint8_t *rss);
int efuse_rss_set_nsc(uint8_t *rss);
int efuse_uid_get_nsc(uint8_t *uid);
int efuse_rf_get_nsc(uint8_t *rf, uint32_t offset, uint32_t sz);;
int efuse_rf_set_nsc(uint8_t *rf, uint32_t offset, uint32_t sz);
int efuse_usdata_get_nsc(uint8_t *usdata, uint32_t offset, uint32_t sz);
int efuse_usdata_get_nsc(uint8_t *usdata, uint32_t offset, uint32_t sz);
#endif
int iak_pub_get_nsc(uint8_t *output, uint32_t *len);
int iak_sign_nsc(uint8_t *hash, uint32_t hash_len, uint8_t *sig, uint32_t *sig_len);
/* #if (ROM_VERSION >= V_1_1) */
int key_derive_nsc(uint8_t *label, size_t label_len, uint8_t *key, size_t key_len);
/* #endif (ROM_VERSION >= V_1_1) */

int hmac_sha1_nsc(param_vec *in_vec, uint32_t in_len, uint8_t *out);
int cryp_aes_ecb_nsc(param_vec *in_vec, uint32_t in_len, uint8_t *out);

#endif  //__MBL_NSC_API_H__
