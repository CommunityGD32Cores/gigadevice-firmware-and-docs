/*!
    \file    mbl_image_validate.c
    \brief   MBL image validate file for GD32W51x WiFi SDK

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

#include "mbl_includes.h"

/*!
    \brief      find boot image
    \param[in]  none
    \param[out] idx: pointer index of boot image
    \param[out] image_offset: pointer to offset of boot image
    \retval     result of find boot image
      \arg        0: find image success
      \arg        -1: offset of image is null
      \arg        -2: find image0 fail
      \arg        -3: find image1 fail
      \arg        -4: image1 has downloaded but image0 has not downloaded
      \arg        -5: other fail
      \arg        -6: read image address from system setting fail
*/
int boot_image_find(OUT uint32_t *idx, OUT uint32_t *image_offset)
{
    uint8_t img0_stat = 0, img1_stat = 0;
    struct sys_setting_t setting;
    uint8_t img0_found = 0, img1_found = 0;
    int boot_idx = -1;
    int ret, result = 0;

    if (NULL == image_offset) {
        result = -1;
        goto Failed;
    }

    ret = rom_sys_status_get(SYS_IMAGE0_STATUS, LEN_SYS_IMAGE_STATUS, &img0_stat);
    if (ret <= SYS_STATUS_FOUND_ERR) {
        result = -2;
        goto Failed;
    } else if (ret == SYS_STATUS_FOUND_OK) {
        img0_found = 1;
    } else {  // SYS_STATUS_NOT_FOUND
        img0_found = 0;
    }
    ret = rom_sys_status_get(SYS_IMAGE1_STATUS, LEN_SYS_IMAGE_STATUS, &img1_stat);
    if (ret <= SYS_STATUS_FOUND_ERR) {
        result = -3;
        goto Failed;
    } else if (ret == SYS_STATUS_FOUND_OK) {
        img1_found = 1;
    } else {  // SYS_STATUS_NOT_FOUND
        img1_found = 0;
    }

    /* Image0 never downloaded, Image1 should not be downloaded too. */
    if (!img0_found && img1_found) {
        result = -4;
        goto Failed;
    }

    if (!img0_found && !img1_found) {
        /* ISP newly downloaded */
        rom_sys_set_img_flag(IMAGE_0, IMG_FLAG_NEWER_MASK, IMG_FLAG_NEWER);
        boot_idx = 0;
        goto ImgSelected;
    }

    if (img0_found && ((img0_stat & IMG_FLAG_NEWER_MASK) == IMG_FLAG_NEWER)) {
        if (((img0_stat & IMG_FLAG_VERIFY_MASK) != IMG_FLAG_VERIFY_FAIL)
            && ((img0_stat & IMG_FLAG_IA_MASK) != IMG_FLAG_IA_FAIL)) {
            boot_idx = 0;
            goto ImgSelected;
        }
    }

    if (img1_found && ((img1_stat & IMG_FLAG_NEWER_MASK) == IMG_FLAG_NEWER)) {
        if (((img1_stat & IMG_FLAG_VERIFY_MASK) != IMG_FLAG_VERIFY_FAIL)
            && ((img1_stat & IMG_FLAG_IA_MASK) != IMG_FLAG_IA_FAIL)) {
            boot_idx = 1;
            goto ImgSelected;
        }
    }

    if (img0_found && ((img0_stat & IMG_FLAG_VERIFY_MASK) == IMG_FLAG_VERIFY_OK)
        && ((img0_stat & IMG_FLAG_IA_MASK) == IMG_FLAG_IA_OK)) {
        boot_idx = 0;
        goto ImgSelected;
    }

    if (img1_found && ((img1_stat & IMG_FLAG_VERIFY_MASK) == IMG_FLAG_VERIFY_OK)
        && ((img1_stat & IMG_FLAG_IA_MASK) == IMG_FLAG_IA_OK)) {
        boot_idx = 1;
        goto ImgSelected;
    }

    if (boot_idx == -1) {
        result = -5;
        goto Failed;
    }

ImgSelected:
    /* Read Image Address from system setting */
    ret = rom_sys_setting_get(&setting);
    if (ret != 0) {
        result = -6;
        goto Failed;
    }
    *idx = boot_idx;
    *image_offset = setting.img0_offset;

    return 0;

Failed:
    return result;
}

/*!
    \brief      find mbl image
    \param[in]  start_offset: flash start offset
    \param[in]  img_type: image type
    \param[out] img_offset: pointer to offset of image
    \retval     result of find mbl image
      \arg         0: find image success
      \arg         -1: invalid flash start offset
      \arg         -2: find image header fail
      \arg         -3: invalid flash offset
*/
int mbl_img_find(IN uint32_t start_offset,
                IN uint8_t img_type,
                OUT uint32_t *img_offset)
{
    struct image_header hdr;
    uint32_t flash_offset = start_offset;
    uint32_t hdr_offset = 0;
    uint32_t chksum;
    int ret;
    uint32_t flash_sz, erase_size;

    if (!rom_is_valid_flash_offset(flash_offset)) {
        return -3;
    }

    flash_sz = rom_flash_total_size();
    erase_size = rom_flash_erase_size();
    while (flash_offset < flash_sz) {
        hdr_offset = flash_offset + RE_VTOR_ALIGNMENT - sizeof(hdr);
        ret = rom_flash_read(hdr_offset, (void*)&hdr, sizeof(hdr));
        if (ret != 0) {
            return -2;
        }
        if ((hdr.magic_h == IMG_MAGIC_H) && (hdr.img_type == img_type)) {
            chksum = rom_cal_checksum((uint8_t *)(&hdr), sizeof(hdr));
            if (chksum == 0)
                break;
        }

        flash_offset += erase_size;
    }

    if (!rom_is_valid_flash_offset(flash_offset)) {
        return -1;
    } else {
        if (img_offset)
            *img_offset = hdr_offset;
        return 0;
    }
}

/*!
    \brief      find PSA root, application root and non-secure processing environment
    \param[in]  start_offset: flash start offset
    \param[out] prot_offset: pointer to offset of PSA root
    \param[out] arot_offset: pointer to offset of application root
    \param[out] nspe_offset: pointer to offfset of non-secure processing environment
    \retval     result of find mbl image
      \arg        1: find success
      \arg        -1: read flash fail while get image header
      \arg        -2: find arot fail
      \arg        -3: read flash fail while get arot size
      \arg        -4: find nspe fail
*/
static int prot_arot_nspe_find(IN uint32_t start_offset,
                               OUT uint32_t *prot_offset,
                               OUT uint32_t *arot_offset,
                               OUT uint32_t *nspe_offset)
{
    struct image_header hdr;
    uint32_t prot_sz, arot_sz, offset;
    int ret;

    /* Get image header */
    ret = rom_flash_read(start_offset, (void*)&hdr, sizeof(hdr));
    if (ret != 0) {
        rom_trace_ex(ROM_ERR, "find_arot_nspe: read flash error (%d)\r\n", ret);
        return -1;
    }
    if (hdr.img_type == IMG_TYPE_NSPE) {
        *prot_offset = 0;
        *arot_offset = 0;
        *nspe_offset = start_offset;
        return 1;
    } else if (hdr.img_type == IMG_TYPE_AROT) {
        *prot_offset = 0;
        *arot_offset = start_offset;
    } else if (hdr.img_type == IMG_TYPE_PROT) {
        *prot_offset = start_offset;
        prot_sz = hdr.tot_sz;

        /* Get AROT address */
        offset = *prot_offset + prot_sz;
        offset = (((offset >> 12) + 1) << 12);
        ret = mbl_img_find(offset, IMG_TYPE_AROT, arot_offset);
        if (ret == -1) {
            /* Not found, it may be happened. */
            *arot_offset = 0;
        } else if (ret < -1) {
            rom_trace_ex(ROM_ERR, "find_arot_nspe: found AROT error(%d, 0x%x). \r\n", ret, arot_offset);
            return -2;
        }
    }

    /* Get AROT size if existed */
    if (*arot_offset) {
        ret = rom_flash_read(*arot_offset, (void*)&hdr, sizeof(hdr));
        if (ret != 0) {
            rom_trace_ex(ROM_ERR, "find_arot_nspe: read flash error (%d)\r\n", ret);
            return -3;
        }
        arot_sz = hdr.tot_sz;
        offset = *arot_offset + arot_sz;
    } else {
        offset = (*prot_offset + prot_sz);
        arot_sz = 0;
    }

    /* Get NSPE address */
    offset = (((offset >> 12) + 1) << 12);
    ret = mbl_img_find(offset, IMG_TYPE_NSPE, nspe_offset);
    if (ret < 0) {
        rom_trace_ex(ROM_ERR, "find_arot_nspe: not found NSPE(%d, 0x%x). \r\n", ret, *nspe_offset);
        *nspe_offset = 0;
        return -4;
    }

    return 1;
}

/*!
    \brief      check non-secure processing environment header
    \param[in]  nspe_offset: processing environment header offset
    \param[out] none
    \retval     result of check nspe header
      \arg        0: check success
      \arg        -1: read flash fail while get image header
      \arg        -2: nspe header check fail
*/
static int nspe_header_check(IN uint32_t nspe_offset)
{
    struct image_header hdr;
    int ret;

    ret = rom_flash_read(nspe_offset, (void*)&hdr, sizeof(hdr));
    if (ret != 0) {
        rom_trace_ex(ROM_ERR, "nspe_header_check: read flash error (%d)\r\n", ret);
        return -1;
    }
    ret = rom_img_verify_hdr(&hdr, IMG_TYPE_IMG);
    if (ret != 0) {
        rom_trace_ex(ROM_ERR, "nspe_header_check: nspe header check failed (%d).\r\n", ret);
        return -2;
    }
    return 0;
}
/*!
    \brief      validate PSA root
    \param[in]  prot_offset: PSA root offset
    \param[in]  prot_pk: pointer to PSA root pass key
    \param[out] prot_info: pointer to PSA root information
    \retval     result of validate arot cert, Return non-zero if image could not be validated/does not validate
*/
static int prot_validate(IN uint32_t prot_offset,
                    IN uint8_t *prot_pk,
                    OUT struct sw_info_t *prot_info)
{

    return rom_img_validate(prot_offset, IMG_TYPE_PROT, prot_pk, prot_info);
}

/*!
    \brief      validate application root certification
    \param[in]  arot_offset: application root offset
    \param[in]  prot_pk: pointer to PSA root pass key
    \param[out] arot_pk: pointer to application root pass key
    \retval     result of validate arot cert, Return non-zero if image could not be validated/does not validate
      \arg        CERT_ERR_READ_FLASH: read flash fail while get image header
      \arg        CERT_ERR_BAD_SZ: cert size error
*/
static int arot_cert_validate(IN uint32_t arot_offset,
                    IN uint8_t *prot_pk,
                    OUT uint8_t *arot_pk)
{
    struct image_header hdr;
    uint32_t crt_offset;
    int crt_sz, ret;

    ret = rom_flash_read(arot_offset, (void*)&hdr, sizeof(hdr));
    if (ret != 0) {
        rom_trace_ex(ROM_ERR, "arot_cert_validate: read flash error (%d)\r\n", ret);
        return CERT_ERR_READ_FLASH;
    }

    crt_sz = hdr.tot_sz - hdr.hdr_sz - hdr.img_sz - hdr.ptlv_sz;
    if (crt_sz <= 0) {
        rom_trace_ex(ROM_ERR, "arot_cert_validate: the cert size is zero.\r\n", crt_sz);
        return CERT_ERR_BAD_SZ;
    }

    crt_offset = arot_offset + (hdr.hdr_sz + hdr.img_sz + hdr.ptlv_sz);

    return rom_cert_validate(crt_offset, crt_sz, prot_pk, arot_pk);
}

/*!
    \brief      validate application root
    \param[in]  arot_offset: application root offset
    \param[in]  arot_pk: pointer to application root pass key
    \param[out] arot_info: pointer to application root information
    \retval     result of validate arot, Return non-zero if image could not be validated/does not validate
*/
static int arot_validate(IN uint32_t arot_offset,
                    IN uint8_t *arot_pk,
                    OUT struct sw_info_t *arot_info)
{
    return rom_img_validate(arot_offset, IMG_TYPE_AROT, arot_pk, arot_info);
}

/*!
    \brief      validate non-secure processing environment certification
    \param[in]  nspe_offset: non-secure processing environment offset
    \param[in]  prot_pk: pointer to PSA root pass key
    \param[out] nspe_pk: pointer to nspe information
    \retval     result of validate nspe cert
      \arg        CERT_ERR_READ_FLASH: read flash fail while get image header
      \arg        CERT_ERR_BAD_SZ: cert size error
*/
static int nspe_cert_validate(IN uint32_t nspe_offset,
                    IN uint8_t *prot_pk,
                    OUT uint8_t *nspe_pk)
{
    struct image_header hdr;
    uint32_t crt_offset;
    int crt_sz, ret;

    ret = rom_flash_read(nspe_offset, (void*)&hdr, sizeof(hdr));
    if (ret != 0) {
        rom_trace_ex(ROM_ERR, "nspe_cert_validate: read flash error (%d)\r\n", ret);
        return CERT_ERR_READ_FLASH;
    }

    crt_sz = hdr.tot_sz - hdr.hdr_sz - hdr.img_sz - hdr.ptlv_sz;
    if (crt_sz <= 0) {
        rom_trace_ex(ROM_ERR, "nspe_cert_validate: the cert size is zero.\r\n", crt_sz);
        return CERT_ERR_BAD_SZ;
    }

    crt_offset = nspe_offset + (hdr.hdr_sz + hdr.img_sz + hdr.ptlv_sz);

    return rom_cert_validate(crt_offset, crt_sz, prot_pk, nspe_pk);
}

/*!
    \brief      validate non-secure processing environment
    \param[in]  nspe_offset: non-secure processing environment offset
    \param[in]  nspe_pk: pointer to nspe information
    \param[out] nspe_info: pointer to nspe information
    \retval     result of validate nspe, Return non-zero if image could not be validated/does not validate
*/
static int nspe_validate(IN uint32_t nspe_offset,
                    IN uint8_t *nspe_pk,
                    OUT struct sw_info_t *nspe_info)
{
    return rom_img_validate(nspe_offset, IMG_TYPE_NSPE, nspe_pk, nspe_info);
}

/*!
    \brief      validate image x
    \param[in]  img_offset: image offset
    \param[in]  pk: pointer to public key
    \param[in]  boot_opt: boot option
    \param[out] only_nspe: pointer to only npse, there is only npse while *only_npse = 1.
    \retval     result of validate nspe
      \arg        0: validate image success
      \arg        -1: find prot, arot and nspe address fail
      \arg        -2: check nspe header fail
      \arg        -3: verify prot fail
      \arg        -4: save prot information into mbl status fail
      \arg        -5: verify arot cert fail
      \arg        -6: verify arot fail
      \arg        -7: save arot information into mbl status fail
      \arg        -8: verify nspe cert fail
      \arg        -9: verify nspe fail
      \arg        -10: save nspe into mbl status fail
      \arg        -11: update main image version counter fail
*/
int image_x_validate(IN uint32_t img_offset,
                     IN uint8_t *pk,
                     IN uint8_t boot_opt,
                     OUT uint32_t *only_nspe)
{
    struct sw_info_t sw_info;
    uint32_t prot_offset = 0, arot_offset = 0, nspe_offset = 0;
    uint8_t arot_pk[IMG_PK_LEN];
    int ret, result = 0;

    /* Find PROT, AROT and NSPE address */
    ret = prot_arot_nspe_find(img_offset, &prot_offset, &arot_offset, &nspe_offset);
    if (ret != 1) {
        rom_trace_ex(ROM_ERR, "image_x_validate: image found error (ret = %d). \r\n", ret);
        result = -1;
        goto Failed;
    }
    if (prot_offset == 0 && arot_offset == 0 && only_nspe) {
        *only_nspe = 1;
    }
    rom_trace_ex(ROM_DBG, "image_x_validate: NSPE offset is 0x%x.\r\n", nspe_offset);

    if (boot_opt == IBL_VERIFY_NONE)
        return 0;

    /* Check if the Main Image Version (equal to NSPE Version) is higher than local NV counter */
    ret = nspe_header_check(nspe_offset);
    if (ret < 0) {
        rom_trace_ex(ROM_ERR, "image_x_validate: the nspe header check failed (ret = %d). \r\n", ret);
        result = -2;
        goto Failed;
    }

    /* ============ PROT ============ */
    if (prot_offset) {
        /* Validate PROT */
        memset(&sw_info, 0, sizeof(struct sw_info_t));
        ret = prot_validate(prot_offset, pk, &sw_info);
        if (ret != 0) {
            rom_trace_ex(ROM_ERR, "image_x_validate: PROT verified failed. \r\n");
            result = -3;
            goto Failed;
        } else {
            rom_trace_ex(ROM_ALWAYS, "MBL: PROT image verify OK.\r\n");
        }
        /* Save PROT sw info to MBL status */
        ret = mbl_status_sw_info_add(&sw_info);
        if (ret != BOOT_STATUS_OK) {
            rom_trace_ex(ROM_ERR, "image_x_validate: Failed to save PROT info to MBL status. \r\n");
            result = -4;
            goto Failed;
        }
    }
    /* ============ AROT ============ */
    if (arot_offset) {
        /* Validate AROT cert */
        if (boot_opt == IBL_VERIFY_CERT_IMG) {
            ret = arot_cert_validate(arot_offset, pk, arot_pk);
            if (ret != CERT_OK) {
                rom_trace_ex(ROM_ERR, "image_x_validate: AROT cert verified failed. \r\n");
                result = -5;
                goto Failed;
            } else {
                rom_trace_ex(ROM_ALWAYS, "MBL: AROT cert verify OK.\r\n");
            }
        } else {
            memcpy(arot_pk, pk, sizeof(arot_pk));
        }
        /* Validate AROT */
        memset(&sw_info, 0, sizeof(struct sw_info_t));
        ret = arot_validate(arot_offset, arot_pk, &sw_info);
        if (ret != 0) {
            rom_trace_ex(ROM_ERR, "image_x_validate: AROT verified failed. \r\n");
            result = -6;
            goto Failed;
        } else {
            rom_trace_ex(ROM_ALWAYS, "MBL: AROT image verify OK.\r\n");
        }
        /* Save AROT sw info to MBL status */
        ret = mbl_status_sw_info_add(&sw_info);
        if (ret != BOOT_STATUS_OK) {
            rom_trace_ex(ROM_ERR, "image_x_validate: Failed to save AROT info to MBL status. \r\n");
            result = -7;
            goto Failed;
        }
    }

    /* ============ NSPE ============ */
    if (arot_offset == 0) {
        /* Validate NSPE cert */
        if (boot_opt == IBL_VERIFY_CERT_IMG) {
            ret = nspe_cert_validate(nspe_offset, pk, arot_pk);
            if (ret != CERT_OK) {
                rom_trace_ex(ROM_ERR, "image_x_validate: NSPE cert verified failed. \r\n");
                result = -8;
                goto Failed;
            } else {
                rom_trace_ex(ROM_ALWAYS, "MBL: NSPE cert verify OK.\r\n");
            }
        } else {
            memcpy(arot_pk, pk, sizeof(arot_pk));
        }
    }
    /* Validate NSPE */
    memset(&sw_info, 0, sizeof(struct sw_info_t));
    ret = nspe_validate(nspe_offset, arot_pk, &sw_info);
    if (ret != 0) {
        rom_trace_ex(ROM_ERR, "image_x_validate: NSPE verified failed. \r\n");
        result = -9;
        goto Failed;
    } else {
        rom_trace_ex(ROM_ALWAYS, "MBL: NSPE image verify OK.\r\n");
    }

    /* Save NSPE sw info to MBL status */
    ret = mbl_status_sw_info_add(&sw_info);
    if (ret != BOOT_STATUS_OK) {
        rom_trace_ex(ROM_ERR, "image_x_validate: Failed to save NSPE info to MBL status. \r\n");
        result = -10;
        goto Failed;
    }

    /* Update Main Image Version Counter */
    ret = rom_sys_set_fw_ver(IMG_TYPE_IMG, sw_info.version);
    if (ret < 0) {
        rom_trace_ex(ROM_ERR, "Update Main Image version counter failed(%d).\r\n", ret);
        result = -11;
        goto Failed;
    } else if (ret == 1){
        /* The new image version is equal to the local counter. Do nothing. */
    } else if (ret == 0){
        rom_trace_ex(ROM_ALWAYS, "MBL: Current image version is %d.%d.%d\r\n",
                    (sw_info.version >> 24),
                    ((sw_info.version << 8) >> 24),
                    (sw_info.version & 0xFFFF));
    }

    return 0;

Failed:
    return result;
}
