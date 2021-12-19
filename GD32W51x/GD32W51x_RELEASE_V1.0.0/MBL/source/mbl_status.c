/*!
    \file    mbl_status.c
    \brief   MBL status operation for GD32W51x WiFi SDK

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
    \brief      print data
    \param[in]  data: pointer to data
    \param[in]  count: print count
    \param[out] none
    \retval     result of print data
*/
static int print_data(uint8_t *data, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++) {
        if (i % 16 == 0)
            rom_printf("\r\n\t");
        rom_printf("%02x ", *(data + i));
    }
    rom_printf("\r\n");
    return 0;
}

/*!
    \brief      print sw information
    \param[in]  sw_info: pointer to sw information
    \param[out] none
    \retval     result of print sw information
*/
static void sw_info_print(struct sw_info_t *sw_info)
{
    rom_printf("Add sw info:\r\n");
    rom_printf("  Type: %d\r\n", sw_info->type);
    rom_printf("  Version: 0x%x\r\n", sw_info->version);
#ifdef BOOT_RECORD
    rom_printf("  Boot Record: ");
    print_data(sw_info->record, sw_info->record_len);
#else
    rom_printf("  Digest: ");
    print_data(sw_info->digest, IMG_HASH_LEN);
    rom_printf("  Signer ID: ");
    print_data(sw_info->signer_id, IMG_PK_LEN);
#endif
}

/*!
    \brief      mbl status initialize
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbl_status_init(void)
{
    struct tfm_boot_data *boot_data;

    boot_data = (struct tfm_boot_data *)MBL_SHARED_DATA_START;

    memset((void *)MBL_SHARED_DATA_START, 0, MBL_SHARED_DATA_SIZE);
    boot_data->header.tlv_magic   = SHARED_DATA_TLV_INFO_MAGIC;
    boot_data->header.tlv_tot_len = SHARED_DATA_HEADER_SIZE;
}

/* See in boot_record.h */
/*!
    \brief      mbl add status type, length and value
    \param[in]  major_type: major type, identify consumer
    \param[in]  minor_type: minor type, identify tlv type
    \param[in]  size: size of data
    \param[in]  data: pointer to data
    \param[out] none
    \retval     result of mbl status add tlv
      \arg        SHARED_MEMORY_OK: mbl status add tlv success
      \arg        SHARED_MEMORY_OVERWRITE: shared memory overwrite
      \arg        SHARED_MEMORY_OVERFLOW: shared memory overflow
*/
enum shared_memory_err_t
mbl_status_tlv_add(uint8_t        major_type,
                   uint16_t       minor_type,
                   size_t         size,
                   const uint8_t *data)
{
    struct shared_data_tlv_entry tlv_entry = {0};
    struct tfm_boot_data *boot_data;
    uint8_t *next_tlv;
    uint32_t tlv_end, offset;

    boot_data = (struct tfm_boot_data *)MBL_SHARED_DATA_START;

    /* Check whether TLV entry is already added.
    * Get the boundaries of TLV section
    */
    tlv_end = MBL_SHARED_DATA_START + boot_data->header.tlv_tot_len;
    offset  = MBL_SHARED_DATA_START + SHARED_DATA_HEADER_SIZE;

    /* Iterates over the TLV section looks for the same entry if found then
    * returns with error: SHARED_MEMORY_OVERWRITE
    */
    for (; offset < tlv_end; offset += tlv_entry.tlv_len) {
        /* Create local copy to avoid unaligned access */
        memcpy(&tlv_entry, (const void *)offset, SHARED_DATA_ENTRY_HEADER_SIZE);
        if (GET_MAJOR(tlv_entry.tlv_type) == major_type &&
                GET_MINOR(tlv_entry.tlv_type) == minor_type) {
            return SHARED_MEMORY_OVERWRITE;
        }
    }

    /* Add TLV entry */
    tlv_entry.tlv_type = SET_TLV_TYPE(major_type, minor_type);
    tlv_entry.tlv_len  = SHARED_DATA_ENTRY_SIZE(size);

    /* Verify overflow of shared area */
    if ((boot_data->header.tlv_tot_len + tlv_entry.tlv_len) > IBL_SHARED_DATA_SIZE){
        return SHARED_MEMORY_OVERFLOW;
    }

    next_tlv = (uint8_t *)boot_data + boot_data->header.tlv_tot_len;
    memcpy(next_tlv, &tlv_entry, SHARED_DATA_ENTRY_HEADER_SIZE);

    next_tlv += SHARED_DATA_ENTRY_HEADER_SIZE;
    memcpy(next_tlv, data, size);

    boot_data->header.tlv_tot_len += tlv_entry.tlv_len;

    return SHARED_MEMORY_OK;
}

/*!
    \brief      mbl get status type, length and value
    \param[in]  major_type: major type, identify consumer
    \param[in]  minor_type: minor type, identify tlv type
    \param[out] tlv_len: pointer to tlv length
    \param[out] tlv_ptr: pointer to tlv address
    \retval     result of mbl get status tlv
*/
int32_t mbl_status_tlv_get(uint8_t       major_type,
                           uint16_t      minor_type,
                           uint16_t      *tlv_len,
                           uint8_t       **tlv_ptr)
{
    struct shared_data_tlv_entry tlv_entry;
    struct tfm_boot_data *boot_data;
    uint8_t *tlv_end;
    uint8_t *tlv_curr;

    boot_data = (struct tfm_boot_data *)MBL_SHARED_DATA_START;
    if (boot_data->header.tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return -1;
    }

    /* Get the boundaries of TLV section where to lookup*/
    tlv_end = (uint8_t *)&boot_data + boot_data->header.tlv_tot_len;
    if (*tlv_ptr == NULL) {
        /* At first call set to the beginning of the TLV section */
        tlv_curr = boot_data->data;
    } else {
        /* Any subsequent call set to the next TLV entry */
        memcpy(&tlv_entry, *tlv_ptr, SHARED_DATA_ENTRY_HEADER_SIZE);
        tlv_curr  = (*tlv_ptr) + tlv_entry.tlv_len;
    }

    /* Iterates over the TLV section and returns the address and size of TLVs
    * with requested module identifier
    */
    for (; tlv_curr < tlv_end; tlv_curr += tlv_entry.tlv_len) {
        /* Create local copy to avoid unaligned access */
        memcpy(&tlv_entry, tlv_curr, SHARED_DATA_ENTRY_HEADER_SIZE);
        if (GET_MAJOR(tlv_entry.tlv_type) == major_type &&
                GET_MINOR(tlv_entry.tlv_type) == minor_type) {
            *tlv_ptr = tlv_curr + SHARED_DATA_ENTRY_HEADER_SIZE;
            if (tlv_len)
                *tlv_len = tlv_entry.tlv_len - SHARED_DATA_ENTRY_HEADER_SIZE;
            return 1;
        }
    }

    return 0;
}


#ifndef BOOT_RECORD
/*!
    \brief      Add a type identifier(short test name) of SW component to the shared
                memory area
    \param[in]  sw_module: identifier of the SW component
    \param[out] none
    \retval     result of add sw type
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
static enum boot_status_err_t
mbl_status_sw_type_add(uint32_t sw_module)
{
    /*FixMe: Use a hard coded value for now. Later on when multiple image will
    *       be handled by MCUBoot then this must be revisited.
    */
    uint16_t ias_minor;
    uint32_t cnt;
    enum shared_memory_err_t res;
    char *sw_type;

    switch (sw_module) {
        case SW_MBL:
            sw_type = "MBL";
            cnt = 3;
            break;
        case SW_PROT:
            sw_type = "PROT";
            cnt = 4;
            break;
        case SW_AROT:
            sw_type = "AROT";
            cnt = 4;
            break;
        case SW_NSPE:
            sw_type = "NSPE";
            cnt = 4;
            break;
        default:
            sw_type = "UNKNOWN";
            cnt = 7;
    }

    /* Add the type identifier of the SW component to the shared data area */
    ias_minor = SET_IAS_MINOR(sw_module, SW_TYPE);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                cnt,
                                (const uint8_t *)sw_type);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}


/*!
    \brief      add status sw version
    \param[in]  sw_module: identifier of the sw component
    \param[in]  version: sw version
    \param[out] none
    \retval     result of add sw version
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
 */
static enum boot_status_err_t
mbl_status_sw_version_add(uint32_t sw_module, uint32_t version)
{
    int32_t cnt;
    enum shared_memory_err_t res;
    char sw_version[14]; /* 8bit.8bit.16bit: 3 + 1 + 3 + 1 + 5 + 1 */
    uint16_t ias_minor;

    /* FixMe: snprintf can be replaced with a custom implementation */
    cnt = snprintf(sw_version, sizeof(sw_version), "%u.%u.%u",
                    (version >> 24),
                    ((version & 0xFF0000) >> 16),
                    (version & 0xFFFF));
    if (cnt < 0 || cnt >= sizeof(sw_version)) {
        return BOOT_STATUS_ERROR;
    }

    /* Add the version of the SW component to the shared data area */
    ias_minor = SET_IAS_MINOR(sw_module, SW_VERSION);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                cnt,
                                (const uint8_t *)sw_version);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status sw digest
    \param[in]  sw_module: identifier of the sw component
    \param[in]  digest: pointer to diget add to tlv
    \param[out] none
    \retval     result of add sw digest
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
static enum boot_status_err_t
mbl_status_sw_digest_add(uint32_t sw_module, uint8_t *digest)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;
    char measure_type[] = "SHA256";

    /* Add the image's hash value to the shared data area */
    ias_minor = SET_IAS_MINOR(sw_module, SW_MEASURE_VALUE);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                IMG_HASH_LEN,
                                digest);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    /* Add the measurement type to the shared data area */
    ias_minor = SET_IAS_MINOR(sw_module, SW_MEASURE_TYPE);
                                res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                sizeof(measure_type) - 1,
                                (const uint8_t *)measure_type);
    if (res) {
        return BOOT_STATUS_ERROR;
    }
    return BOOT_STATUS_OK;
}

/*!
    \brief      add status sw signer
    \param[in]  sw_module: identifier of the sw component
    \param[in]  signer_id: pointer to signer id
    \param[out] none
    \retval     result of add sw signer
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
static enum boot_status_err_t
mbl_status_sw_signer_add(uint32_t sw_module, uint8_t *signer_id)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;

    /* Add the hash of the public key to the shared data area */
    ias_minor = SET_IAS_MINOR(sw_module, SW_SIGNER_ID);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                IMG_PKHASH_LEN,
                                signer_id);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}
#endif

/*!
    \brief      add status sw information
    \param[in]  sw_info: pointer to sw information
    \param[out] none
    \retval     result of add sw information
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_sw_info_add(struct sw_info_t *sw_info)
{
    uint32_t sw_module;
#ifdef BOOT_RECORD
    uint16_t ias_minor;
    enum shared_memory_err_t res;
#else
    enum boot_status_err_t res;
#endif

    switch(sw_info->type) {
        case IMG_TYPE_MBL:
            sw_module = SW_MBL;
            break;
        case IMG_TYPE_PROT:
            sw_module = SW_PROT;
            break;
        case IMG_TYPE_AROT:
            sw_module = SW_AROT;
            break;
        case IMG_TYPE_NSPE:
            sw_module = SW_NSPE;
            break;
        default:
            sw_module = SW_MAX;
            break;
    }
#ifdef BOOT_RECORD
    /* Add the CBOR encoded boot record to the shared data area. */
    ias_minor = SET_IAS_MINOR(sw_module, SW_BOOT_RECORD);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                sw_info->record_len,
                                sw_info->record);
    if (res) {
        return BOOT_STATUS_ERROR;
    }
#else
    res = mbl_status_sw_type_add(sw_module);
    if (res) {
        return res;
    }

    res = mbl_status_sw_version_add(sw_module, sw_info->version);
    if (res) {
        return res;
    }

    res = mbl_status_sw_digest_add(sw_module, sw_info->digest);
    if (res) {
        return res;
    }

    res = mbl_status_sw_signer_add(sw_module, sw_info->signer_id);
    if (res) {
        return res;
    }
#endif /* BOOT RECORD */

    // print_sw_info(sw_info);
    return BOOT_STATUS_OK;
}

/*!
    \brief      add status boot seed
    \param[in]  boot_seed: pointer to boot seed
    \param[out] none
    \retval     result of add boot seed
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_boot_seed_add(uint8_t *boot_seed)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;

    ias_minor = SET_IAS_MINOR(SW_GENERAL, BOOT_SEED);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                BOOT_SEED_SIZE,
                                boot_seed);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status hardware version
    \param[in]  hw_version: pointer to boot hardware version
    \param[out] none
    \retval     result of add hardware version
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_hw_ver_add(uint8_t *hw_version)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;

    ias_minor = SET_IAS_MINOR(SW_GENERAL, HW_VERSION);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                HW_VERSION_MAX_SIZE,
                                hw_version);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status life cycle
    \param[in]  life_cycle: life cycle
    \param[out] none
    \retval     result of add life cycle
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_life_cycle_add(uint32_t life_cycle)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;

    ias_minor = SET_IAS_MINOR(SW_GENERAL, SECURITY_LIFECYCLE);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                sizeof(uint32_t),
                                (uint8_t *)&life_cycle);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status implement id
    \param[in]  impl_id: pointer to implement id
    \param[out] none
    \retval     result of add implement id
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_impl_id_add(uint8_t *impl_id)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;

    ias_minor = SET_IAS_MINOR(SW_GENERAL, IMPLEMENTATION_ID);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                IMPLEMENTATION_ID_MAX_SIZE,
                                impl_id);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status instance id
    \param[in]  instance_id: pointer to instance id
    \param[out] none
    \retval     result of add instance id
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_inst_id_add(uint8_t *instance_id)
{
    uint16_t ias_minor;
    enum shared_memory_err_t res;

    ias_minor = SET_IAS_MINOR(SW_GENERAL, INSTANCE_ID);
    res = mbl_status_tlv_add(TLV_MAJOR_IAS,
                                ias_minor,
                                INSTANCE_ID_MAX_SIZE,
                                instance_id);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status running image
    \param[in]  img_idx: pointer to image index
    \param[out] none
    \retval     result of add image index
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_running_img_add(uint8_t *img_idx)
{
    uint16_t core_minor;
    enum shared_memory_err_t res;

    core_minor = SET_CORE_MINOR(SYS_STATUS, STATUS_RUNNING_IMG);
    res = mbl_status_tlv_add(TLV_MAJOR_CORE,
                                core_minor,
                                4,
                                img_idx);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status sau entry
    \param[in]  start: pointer to start entry
    \param[in]  end: pointer to end entry
    \param[out] none
    \retval     result of add sau entry
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_sau_entry_add(uint8_t *start, uint8_t *end)
{
    uint16_t core_minor;
    enum shared_memory_err_t res;

    core_minor = SET_CORE_MINOR(SYS_SAU, SAU_ENTRY_START);
    res = mbl_status_tlv_add(TLV_MAJOR_CORE,
                                core_minor,
                                4,
                                start);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    core_minor = SET_CORE_MINOR(SYS_SAU, SAU_ENTRY_END);
    res = mbl_status_tlv_add(TLV_MAJOR_CORE,
                                core_minor,
                                4,
                                end);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      add status rom version
    \param[in]  rom_ver: pointer to rom version
    \param[out] none
    \retval     result of add rom version
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_rom_version_add(uint8_t *rom_ver)
{
    uint16_t core_minor;
    enum shared_memory_err_t res;

    core_minor = SET_CORE_MINOR(SYS_STATUS, STATUS_ROM_VERSION);
    res = mbl_status_tlv_add(TLV_MAJOR_CORE,
                                core_minor,
                                4,
                                rom_ver);
    if (res) {
        return BOOT_STATUS_ERROR;
    }

    return BOOT_STATUS_OK;
}

/*!
    \brief      get status rom version
    \param[in]  none
    \param[out] rom_ver: pointer to rom version
    \retval     result of add rom version
      \arg        BOOT_STATUS_OK: boot status ok
      \arg        BOOT_STATUS_ERROR: boot status error
*/
enum boot_status_err_t
mbl_status_rom_version_get(uint8_t *rom_ver)
{
    uint16_t core_minor;
    enum shared_memory_err_t res;
    uint8_t *tlv_val = NULL;

    core_minor = SET_CORE_MINOR(SYS_STATUS, STATUS_ROM_VERSION);
    res = (enum shared_memory_err_t) mbl_status_tlv_get(TLV_MAJOR_CORE,
                                core_minor,
                                NULL,
                                &tlv_val);
    if (res == 1) {
        memcpy(rom_ver, tlv_val, 4);
        return BOOT_STATUS_OK;
    } else {
        memset(rom_ver, 0, 4);
        return BOOT_STATUS_ERROR;
    }
}

/*!
    \brief      show status
    \param[in]  none
    \param[out] none
    \retval     result of show status
*/
int32_t mbl_status_show(void)
{
    struct shared_data_tlv_entry tlv_entry;
    struct tfm_boot_data *boot_data;
    uint8_t *tlv_end;
    uint8_t *tlv_curr;
    uint16_t i;

    boot_data = (struct tfm_boot_data *)MBL_SHARED_DATA_START;
    if (boot_data->header.tlv_magic != SHARED_DATA_TLV_INFO_MAGIC) {
        return -1;
    }

    /* Get the boundaries of TLV section where to lookup*/
    tlv_end = (uint8_t *)boot_data + boot_data->header.tlv_tot_len;

    /* Iterates over the TLV section */
    rom_printf("MBL Status Show:\r\n================================\r\n");
    rom_printf("Magic: %04x\r\n", boot_data->header.tlv_magic);
    rom_printf("Total length: 0x%04x\r\n", boot_data->header.tlv_tot_len);

    tlv_curr = boot_data->data;
    rom_printf("T\tL\tV\r\n");
    for (; tlv_curr < tlv_end; tlv_curr += tlv_entry.tlv_len) {
        /* Create local copy to avoid unaligned access */
        memcpy(&tlv_entry, tlv_curr, SHARED_DATA_ENTRY_HEADER_SIZE);
        rom_printf("%04x\t%04x\t", tlv_entry.tlv_type, tlv_entry.tlv_len);
        i = SHARED_DATA_ENTRY_HEADER_SIZE;
        for (; i < tlv_entry.tlv_len; i++) {
            if (i == tlv_entry.tlv_len -1)
                rom_printf("%02x", tlv_curr[i]);
            else
                rom_printf("%02x-", tlv_curr[i]);
        }
        rom_printf("\r\n");
    }
    return 0;
}

/*!
    \brief      dump status
    \param[in]  none
    \param[out] none
    \retval     result of dump status
*/
int32_t mbl_status_dump(void)
{
    struct tfm_boot_data *boot_data;
    uint8_t *p;

    boot_data = (struct tfm_boot_data *)MBL_SHARED_DATA_START;
    p = (uint8_t *)boot_data;
    rom_printf("MBL Status Dump:\r\n================================\r\n");
    print_data(p, boot_data->header.tlv_tot_len);

    return 0;
}
