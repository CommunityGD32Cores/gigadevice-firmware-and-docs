/*!
    \file    mbl_status.h
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

#ifndef __MBL_STATUS_H__
#define __MBL_STATUS_H__

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Major numbers (4 bit) to identify
 * the consumer of shared data in runtime SW
 */
#define TLV_MAJOR_CORE     0x0
#define TLV_MAJOR_IAS      0x1

/**
 * The shared data between boot loader and runtime SW is TLV encoded. The
 * shared data is stored in a well known location in secure memory and this is
 * a contract between boot loader and runtime SW.
 *
 * The structure of shared data must be the following:
 *  - At the beginning there must be a header: struct shared_data_tlv_header
 *    This contains a magic number and a size field which covers the entire
 *    size of the shared data area including this header.
 *  - After the header there come the entries which are composed from an entry
 *    header structure: struct shared_data_tlv_entry and the data. In the entry
 *    header is a type field (tly_type) which identify the consumer of the
 *    entry in the runtime SW and specify the subtype of that data item. There
 *    is a size field (tlv_len) which covers the size of the entry header and
 *    the data. After this structure comes the actual data.
 *  - Arbitrary number and size of data entry can be in the shared memory area.
 *
 * This table gives of overview about the tlv_type field in the entry header.
 * The tlv_type always composed from a major and minor number. Major number
 * identifies the addressee in runtime SW, who should process the data entry.
 * Minor number used to encode more info about the data entry. The actual
 * definition of minor number could change per major number. In case of boot
 * status data, which is going to be processed by initial attestation service
 * the minor number is split further to two part: sw_module and claim. The
 * sw_module identifies the SW component in the system which the data item
 * belongs to and the claim part identifies the exact type of the data.
 *
 * |---------------------------------------|
 * |            tlv_type (16)              |
 * |---------------------------------------|
 * | tlv_major(4)|      tlv_minor(12)      |
 * |---------------------------------------|
 * | MAJOR_IAS   | sw_module(6) | claim(6) |
 * |---------------------------------------|
 * | MAJOR_CORE  |          TBD            |
 * |---------------------------------------|
 */

/*====================================================*/
/* TLV_MAJOR_CORE */
/*====================================================*/
/*
 * Type
 */
#define SYS_STATUS                    0x00
#define SYS_SAU                       0x01

/*
 * Subtype
 */
#define STATUS_RUNNING_IMG            0x00
#define STATUS_ROM_VERSION            0x01

#define SAU_ENTRY_START               0x00
#define SAU_ENTRY_END                 0x01

#define GET_CORE_TYPE(tlv_type)       (GET_MINOR(tlv_type) >> 8)
#define GET_CORE_SUBTYPE(tlv_type)    (GET_MINOR(tlv_type) & 0xFF)
#define SET_CORE_MINOR(type, subtype) (((type) << 8) | (subtype))

/*====================================================*/
/* TLV_MAJOR_IAS */
/*====================================================*/
/* Initial attestation: SW components / SW modules
 * This list is intended to be adjusted per device. It contains more SW
 * components than currently available in TF-M project. It serves as an example,
 * what kind of SW components might be available.
 */
#define SW_GENERAL     0x00
#define SW_MBL         0x01
#define SW_PROT        0x02
#define SW_AROT        0x03
#define SW_SPE         0x04
#define SW_NSPE        0x05
#define SW_S_NS        0x06
#define SW_MAX         0x07
#define SW_BOOT_RECORD    0x3F

/* Initial attestation: Claim per SW components / SW modules */
/* Bits: 0-2 */
#define SW_VERSION       0x00
#define SW_SIGNER_ID     0x01
/* Reserved              0x02 */
#define SW_TYPE          0x03
/* Bits: 3-5 */
#define SW_MEASURE_VALUE 0x08
#define SW_MEASURE_TYPE  0x09

/* Initial attestation: General claim does not belong any particular SW
 * component. But they might be part of the boot status.
 */
#define BOOT_SEED          0x00
#define HW_VERSION         0x01
#define SECURITY_LIFECYCLE 0x02
/*Added by GD*/
#define IMPLEMENTATION_ID  0x03
#define INSTANCE_ID        0x04
/*Added by GD end*/

/* General macros to handle TLV type */
#define MAJOR_MASK 0xF     /* 4  bit */
#define MAJOR_POS  12      /* 12 bit */
#define MINOR_MASK 0xFFF   /* 12 bit */

#define SET_TLV_TYPE(major, minor) \
        ((((major) & MAJOR_MASK) << MAJOR_POS) | ((minor) & MINOR_MASK))
#define GET_MAJOR(tlv_type) ((tlv_type) >> MAJOR_POS)
#define GET_MINOR(tlv_type) ((tlv_type) &  MINOR_MASK)

/* Initial attestation specific macros */
#define MODULE_POS 6               /* 6 bit */
#define CLAIM_MASK 0x3F            /* 6 bit */
#define MEASUREMENT_CLAIM_POS 3    /* 3 bit */

#define GET_IAS_MODULE(tlv_type) (GET_MINOR(tlv_type) >> MODULE_POS)
#define GET_IAS_CLAIM(tlv_type)  (GET_MINOR(tlv_type)  & CLAIM_MASK)
#define SET_IAS_MINOR(sw_module, claim) (((sw_module) << 6) | (claim))

#define GET_IAS_MEASUREMENT_CLAIM(ias_claim) ((ias_claim) >> \
                                              MEASUREMENT_CLAIM_POS)
#define BOOT_SEED_SIZE (32u)
#define HW_VERSION_MAX_SIZE (18u)
#define IMPLEMENTATION_ID_MAX_SIZE (32u)
#define INSTANCE_ID_MAX_SIZE (33u)

/*====================================================*/
/* TLV structures */
/*====================================================*/

/* Magic value which marks the beginning of shared data area in memory */
#define SHARED_DATA_TLV_INFO_MAGIC    0x2019

/**
 * Shared data TLV header.  All fields in little endian.
 *
 *    -----------------------------------
 *    | tlv_magic(16) | tlv_tot_len(16) |
 *    -----------------------------------
 */
struct shared_data_tlv_header {
    uint16_t tlv_magic;
    uint16_t tlv_tot_len; /* size of whole TLV area (including this header) */
};

#define SHARED_DATA_HEADER_SIZE sizeof(struct shared_data_tlv_header)

/**
 * Shared data TLV entry header format. All fields in little endian.
 *
 *    -------------------------------
 *    | tlv_type(16) |  tlv_len(16) |
 *    -------------------------------
 *    |         Raw data            |
 *    -------------------------------
 */
struct shared_data_tlv_entry {
    uint16_t tlv_type;
    uint16_t tlv_len; /* size of single TLV entry (including this header). */
};

/**
 * \struct tfm_boot_data
 *
 * \brief Store the data for the runtime SW
 */
struct tfm_boot_data {
    struct shared_data_tlv_header header;
    uint8_t data[];
};

#define SHARED_DATA_ENTRY_HEADER_SIZE sizeof(struct shared_data_tlv_entry)
#define SHARED_DATA_ENTRY_SIZE(size) (size + SHARED_DATA_ENTRY_HEADER_SIZE)


/*!
 * \enum shared_data_err_t
 *
 * \brief Return values for adding data entry to shared memory area
 */
enum shared_memory_err_t {
    SHARED_MEMORY_OK = 0,
    SHARED_MEMORY_OVERFLOW = 1,
    SHARED_MEMORY_OVERWRITE = 2,
    SHARED_MEMORY_GEN_ERROR = 3,

    /* This is used to force the maximum size */
    TLV_TYPE_MAX = 0xFFFFFFFF,
};

/*!
 * \enum boot_status_err_t
 *
 * \brief Return values for saving boot status information to shared memory are
 */
enum boot_status_err_t {
    BOOT_STATUS_OK,
    BOOT_STATUS_ERROR,
};

void
mbl_status_init(void);

/*!
 * \brief Add a data item to the shared data area between bootloader and
 *        runtime SW
 *
 * \param[in] major_type  TLV major type, identify consumer
 * \param[in] minor_type  TLV minor type, identify TLV type
 * \param[in] size        length of added data
 * \param[in] data        pointer to data
 *
 * \return Returns error code as specified in \ref shared_memory_err_t
 */
enum shared_memory_err_t
mbl_status_tlv_add(uint8_t        major_type,
                             uint16_t       minor_type,
                             size_t         size,
                             const uint8_t *data);

int32_t
mbl_status_tlv_get(uint8_t        major_type,
                             uint16_t       minor_type,
                                        uint16_t  *tlv_len,
                                        uint8_t  **tlv_ptr);
/*!
 * \brief Add an image's all boot status information to the shared data area
 *        between bootloader and runtime SW
 *
 * \param[in]  sw_module  Identifier of the SW component
 * \param[in]  hdr        Pointer to the image header stored in RAM
 * \param[in]  fap        Pointer to the flash area where image is stored
 *
 * \return Returns error code as specified in \ref boot_status_err_t
 */
enum boot_status_err_t
mbl_status_sw_info_add(struct sw_info_t *sw_info);


enum boot_status_err_t
mbl_status_boot_seed_add(uint8_t *boot_seed);

enum boot_status_err_t
mbl_status_hw_ver_add(uint8_t *hw_version);

enum boot_status_err_t
mbl_status_life_cycle_add(uint32_t life_cycle);

enum boot_status_err_t
mbl_status_impl_id_add(uint8_t *impl_id);

enum boot_status_err_t
mbl_status_inst_id_add(uint8_t *instance_id);

enum boot_status_err_t
mbl_status_running_img_add(uint8_t *img_idx);

enum boot_status_err_t
mbl_status_sau_entry_add(uint8_t *start, uint8_t * end);

enum boot_status_err_t
mbl_status_rom_version_add(uint8_t *rom_ver);

enum boot_status_err_t
mbl_status_rom_version_get(uint8_t *rom_ver);

int32_t
mbl_status_show(void);

int32_t
mbl_status_dump(void);

#ifdef __cplusplus
}
#endif

#endif /* __MBL_STATUS_H__ */
