/*!
    \file  usbd_dfu_mal.c
    \brief USB DFU device media access layer functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "dfu_mal.h"
#include "flash_if.h"
#include "usb_defines.h"

/* the reference tables of global memories callback and string descriptors.
   to add a new memory, you can do as follows: 
   1. modify the value of MAX_USED_MEDIA in usbd_dfu_mal.h
   2. add the pointer to the callback structure in this table
   3. add the pointer to the memory string descriptor in usbd_dfu_StringDesc table
   no other operation is required.
 */
DFU_MAL_Property_TypeDef* tMALTab[MAX_USED_MEMORY_MEDIA] = {
    &DFU_Flash_cb
};

/* the list of memory interface string descriptor pointers. this list
   can be updated whenever a memory has to be added or removed */
const uint8_t* USBD_DFU_StringDesc[MAX_USED_MEMORY_MEDIA] = 
{
    (const uint8_t *)FLASH_IF_STRING
};

/* memory buffer for downloaded data */
uint8_t  MAL_Buffer[TRANSFER_SIZE];

static uint8_t  DFU_MAL_CheckAddr (uint32_t Addr);

/*!
    \brief      initialize the memory media on the GD32
    \param[in]  none
    \param[out] none
    \retval     MAL_OK
*/
uint8_t  DFU_MAL_Init (void)
{
    uint32_t memIdx = 0;

    /* initialize all supported memory medias */
    for (memIdx = 0; memIdx < MAX_USED_MEMORY_MEDIA; memIdx++) {
        /* check if the memory media exists */
        if (tMALTab[memIdx]->pMAL_Init != NULL) {
            tMALTab[memIdx]->pMAL_Init();
        }
    }

    return MAL_OK;
}

/*!
    \brief      deinitialize the memory media on the GD32
    \param[in]  none
    \param[out] none
    \retval     MAL_OK
*/
uint8_t  DFU_MAL_DeInit (void)
{
    uint32_t memIdx = 0;

    /* deinitializes all supported memory medias */
    for (memIdx = 0; memIdx < MAX_USED_MEMORY_MEDIA; memIdx++) {
        /* check if the memory media exists */
        if (tMALTab[memIdx]->pMAL_DeInit != NULL) {
            tMALTab[memIdx]->pMAL_DeInit();
        }
    }

    return MAL_OK;
}

/*!
    \brief      Erase a memory sector
    \param[in]  Addr: memory sector address/code
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t  DFU_MAL_Erase (uint32_t Addr)
{
    uint32_t memIdx = DFU_MAL_CheckAddr(Addr);

    /* check if the address is in protected area */
    if (IS_PROTECTED_AREA(Addr)) {
        return MAL_FAIL;
    }

    if (memIdx < MAX_USED_MEMORY_MEDIA) {
        /* check if the operation is supported */
        if (tMALTab[memIdx]->pMAL_Erase != NULL) {
            return tMALTab[memIdx]->pMAL_Erase(Addr);
        } else {
            return MAL_FAIL;
        }
    } else {
        return MAL_FAIL;
    }
}

/*!
    \brief      write data to sectors of memory
    \param[in]  Addr: sector address/code
    \param[in]  Len: length of data to be written (in bytes)
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t  DFU_MAL_Write (uint32_t Addr, uint32_t Len)
{
    uint32_t memIdx = DFU_MAL_CheckAddr(Addr);

    /* check if the address is in protected area */
    if (IS_PROTECTED_AREA(Addr)) {
        return MAL_FAIL;
    }

    if (memIdx < MAX_USED_MEMORY_MEDIA) {
        /* check if the operation is supported */
        if (tMALTab[memIdx]->pMAL_Write != NULL) {
            return tMALTab[memIdx]->pMAL_Write(Addr, Len);
        } else {
            return MAL_FAIL;
        }
    } else {
        return MAL_FAIL;
    }
}

/*!
    \brief      Read data from sectors of memory
    \param[in]  Addr: sector address/code
    \param[in]  Len: length of data to be written (in bytes)
    \param[out] none
    \retval     Pointer to buffer
*/
uint8_t*  DFU_MAL_Read (uint32_t Addr, uint32_t Len)
{
    uint32_t memIdx = 0;

    if (Addr != OB_RDPT) {
        memIdx = DFU_MAL_CheckAddr(Addr);
    }

    if (memIdx < MAX_USED_MEMORY_MEDIA) {
        /* check if the operation is supported */
        if (tMALTab[memIdx]->pMAL_Read != NULL) {
            return tMALTab[memIdx]->pMAL_Read(Addr, Len);
        } else {
            return MAL_Buffer;
        }
    } else {
        return MAL_Buffer;
    }
}

/*!
    \brief      get the status of a given memory and store in buffer
    \param[in]  Addr: sector address/code (allow to determine which memory will be addressed)
    \param[in]  Cmd: 0 for erase and 1 for write
    \param[in]  buffer: pointer to the buffer where the status data will be stored
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t  DFU_MAL_GetStatus (uint32_t Addr, uint8_t Cmd, uint8_t *buffer)
{
    uint32_t memIdx = DFU_MAL_CheckAddr(Addr);

    if (memIdx < MAX_USED_MEMORY_MEDIA) {
        if (Cmd & 0x01) {
            SET_POLLING_TIMEOUT(tMALTab[memIdx]->WriteTimeout);
        } else {
            SET_POLLING_TIMEOUT(tMALTab[memIdx]->EraseTimeout);
        }

        return MAL_OK;
    } else {
        return MAL_FAIL;
    }
}

/*!
    \brief      Check the address is supported
    \param[in]  Addr: Sector address/code (allow to determine which memory will be addressed)
    \param[out] none
    \retval     Index of the addressed memory
*/
static uint8_t  DFU_MAL_CheckAddr (uint32_t Addr)
{
    uint32_t memIdx = 0;

    /* check with all supported memories */
    for (memIdx = 0; memIdx < MAX_USED_MEMORY_MEDIA; memIdx++) {
        /* if the check address is supported, return the memory index */
        if (tMALTab[memIdx]->pMAL_CheckAdd(Addr) == MAL_OK) {
            return memIdx;
        }
    }

    /* if there is no memory found, return MAX_USED_MEDIA */
    return (MAX_USED_MEMORY_MEDIA);
}
