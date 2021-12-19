/*!
    \file    fatfs.c
    \brief   fatfs init and config file

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
#include "fatfs.h"
#include "stdio.h"
#include "ffconf.h"
#include "diskio.h"
#include "app_type.h"
#include "wrapper_os.h"
#include "gd32w51x.h"
#include "debug_print.h"
#include "wear_levelling_flash.h"
#include "nspe_region.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif


#if defined CONFIG_FATFS_SUPPORT
#define USE_WL
#define MAX_FILE_PATH 128
#define MAX_FILE_STATE 4     /* max file state level, eg:a/b/c/d.txt */
#define FATFS_OTHER_ERROR -1

static FATFS *fs = NULL;     /* Filesystem object */

/*!
    \brief      fatfs: create a fatfs volume and mount a logical drive
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fatfs_mk_mount(void)
{
    int res = FR_OK;
    BYTE *work = NULL;

#ifdef USE_WL
    sys_memset(&wl_config_global, 0, sizeof(wl_config_global));
    wl_config_global.cfg.start_addr = RE_IMG_1_NSPE_OFFSET;
    wl_config_global.cfg.full_mem_size = FATFS_FULL_MEM_SIZE;
    wl_config_global.cfg.page_size = FATFS_SECTOR_SIZE;
    wl_config_global.cfg.sector_size = FATFS_SECTOR_SIZE;
    wl_config_global.cfg.wr_size = 0x00000008; //bits
    wl_config_global.cfg.version = 0x00000001;
    wl_config_global.cfg.temp_buff_size = FATFS_SECTOR_SIZE;

    if (wl_flash_config(&wl_config_global))
        return;
#endif

    fs = (FATFS *)sys_malloc(sizeof(FATFS));
    if (fs == NULL) {
        return;
    }

    res = f_mount(fs, "0", 1);  /*check if has made an volum in flash before*/
    if (res == FR_OK) {
        DEBUGPRINT("Fatfs: mount succeed\r\n");
        return;
    }

    work = (BYTE *)sys_malloc(FF_MAX_SS);
    if (work == NULL) {
        sys_mfree(fs);
        return;
    }

    res = f_mkfs("0", 0, work, FF_MAX_SS);
    if (res != FR_OK) {
        DEBUGPRINT("fatfs: mkfs failed\r\n");
        fresult_analyse(res);
        sys_mfree(fs);
        sys_mfree(work);
        return;
    }
    DEBUGPRINT("fatfs: mkfs succeed\r\n");

    res = f_mount(fs, "0", 1);
    if (res == FR_OK) {
        sys_mfree(work);
        DEBUGPRINT("Fatfs: mount succeed\r\n");
        return;
    }
    sys_mfree(fs); /* if mount succeed, do not free fs */

}

/*!
    \brief      delete an dir and all files under it
    \param[in]  path:pointer to dir/file path
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
int delete_files(char* path, char* path_temp)
{
    int res = FR_OK;
    DIR dir;
    UINT i, j;
    static FILINFO fno;

    res = f_opendir(&dir, path);
    if (res != FR_OK) {
        return f_unlink(path);
    }

    i = strlen(path);
    for (;;) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) {
            break;
        }

        if (fno.fattrib & AM_DIR) {
            snprintf(&path[i], (strlen(fno.fname) + 2), "/%s", fno.fname);
            res = fatfs_delete(path);
            if (res != FR_OK)
                break;
            path[i] = 0;
        } else {
            snprintf(path_temp, (i + strlen(fno.fname) + 2), "%s/%s", path, fno.fname);
            res = f_unlink(path_temp);
        }
    }

    f_closedir(&dir);
    res = f_unlink(path);
    if (res != FR_OK) {
        DEBUGPRINT("delete path %s failed\r\n", path);
    }
    return res;
}

/*!
    \brief      scan an dir and print all files or dir path under it
    \param[in]  path:pointer to dir path
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
static int scan_dir(char* path)
{
    int res = FR_OK;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir(&dir, path);
    if (res != FR_OK)
        return res;

    for (;;) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
            break;

        if (fno.fattrib & AM_DIR) {
            DEBUGPRINT("DIR:  %s/%s\r\n", path, fno.fname);
            i = strlen(path);
            snprintf(&path[i], (strlen(fno.fname) + 2), "/%s", fno.fname);
            res = scan_dir(path);
            if (res != FR_OK)
                break;
            path[i] = 0;
        } else {
            DEBUGPRINT("FILE: %s/%s %dBytes\r\n", path, fno.fname, fno.fsize);
        }
    }
    res = f_closedir(&dir);
    return res;
}

/*!
    \brief      devided a file/dir path by '/' or '\'
    \param[in]  buf:pointer to dir/file path
    \param[out] argv:pointer to store each stage of path
    \retval     stage of path
*/
static int parse_path(char *buf, char **argv)
{
    int argc = 0;

    if (*buf == '/' || *buf == '\\') {
        buf++;        /* remove first '/' or '\\' */
    }

    while(*buf != '\0') {
        argv[argc] = buf;
        argc++;
        buf++;

        while(((*buf != '/') && (*buf != '\\')) && (*buf != '\0'))
            buf++;

        while(*buf == '/' || *buf == '\\') {
            *buf = '\0';
            buf++;
        }
    }
    return argc;
}

/*!
    \brief      analyse return value of fatfs operation any print it's reason
    \param[in]  res: FRESULT
    \param[out] none
    \retval     none
*/
static void fresult_analyse(int8_t res)
{
    switch (res) {
    case FR_OK:
        // DEBUGPRINT("Succeeded\r\n");
        break;
    case FR_DISK_ERR:
        DEBUGPRINT("FATFS_ERROR: A hard error occurred in the low level disk I/O layer\r\n");
        break;
    case FR_INT_ERR:
        DEBUGPRINT("FATFS_ERROR: Assertion failed\r\n");
        break;
    case FR_NOT_READY:
        DEBUGPRINT("FATFS_ERROR: The physical drive cannot work\r\n");
        break;
    case FR_NO_FILE:
    case FR_NO_PATH:
        DEBUGPRINT("FATFS_ERROR: Could not find the file or path\r\n");
        break;
    case FR_INVALID_NAME:
        DEBUGPRINT("FATFS_ERROR: The path name format is invalid\r\n");
        break;
    case FR_DENIED:
        DEBUGPRINT("FATFS_ERROR: Access denied due to prohibited access or directory full\r\n");
        break;
    case FR_EXIST:
        DEBUGPRINT("FATFS_ERROR: Access denied due to prohibited access\r\n");
        break;
    case FR_INVALID_OBJECT:
        DEBUGPRINT("FATFS_ERROR: The file/directory object is invalid\r\n");
        break;
    case FR_WRITE_PROTECTED:
        DEBUGPRINT("FATFS_ERROR: The physical drive is write protected\r\n");
        break;
    case FR_INVALID_DRIVE:
        DEBUGPRINT("FATFS_ERROR: The logical drive number is invalid\r\n");
        break;
    case FR_NOT_ENABLED:
        DEBUGPRINT("FATFS_ERROR: The volume has no work area \r\n");
        break;
    case FR_NO_FILESYSTEM:
        DEBUGPRINT("FATFS_ERROR: There is no valid FAT volume\r\n");
        break;
    case FR_MKFS_ABORTED:
        DEBUGPRINT("FATFS_ERROR: The f_mkfs() aborted due to any problem\r\n");
        break;
    case FR_TIMEOUT:
        DEBUGPRINT("FATFS_ERROR: Could not get a grant to access the volume within defined period\r\n");
        break;
    case FR_LOCKED:
        DEBUGPRINT("FATFS_ERROR: The operation is rejected according to the file sharing policy\r\n");
        break;
    case FR_NOT_ENOUGH_CORE:
        DEBUGPRINT("FATFS_ERROR: LFN working buffer could not be allocated\r\n");
        break;
    case FR_TOO_MANY_OPEN_FILES:
        DEBUGPRINT("FATFS_ERROR: Number of open files > FF_FS_LOCK\r\n");
        break;
    case FR_INVALID_PARAMETER:
        DEBUGPRINT("FATFS_ERROR: Given parameter is invalid\r\n");
        break;
    case FATFS_OTHER_ERROR:
        DEBUGPRINT("FATFS_ERROR: Another error occured\r\n");
        break;
    default:
        DEBUGPRINT("FATFS_ERROR: Unknown error occured!\r\n");
        break;
    }
}

/*!
    \brief      create a file by path
    \param[in]  path:pointer to file path
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
static int file_create(char *path)
{
    FIL *fil = NULL;
    int res = FR_OK;

    fil = (FIL *)sys_malloc(sizeof(FIL));
    if (NULL == fil) {
        DEBUGPRINT("ERROR_FATFS: file_create, malloc fil failed\r\n");
        return FATFS_OTHER_ERROR;
    }

    res = f_open(fil, path, FA_CREATE_NEW);
    if (res == FR_OK) {
        res = f_close(fil);
    }

    sys_mfree(fil);
    return res;
}

/*!
    \brief      create a dir/file by any stage path
    \param[in]  path:pointer to dir/file path, the maximum depth of path is 4
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
int fatfs_create(char *path)
{
    int res = FR_OK;
    char *argv_temp[MAX_FILE_STATE];
    int argc_temp = 0;
    int category = 0;
    int i = 0;
    int pos = 0;
    char path_temp[MAX_FILE_PATH + 2] = {0};  /* +2: 1 for "\" or "/" in the start of path; 1 for end "\0" */

    if (path[strlen(path) - 1] == '\\' || path[strlen(path) - 1] == '/') {
        category = 0;                /* it's a dir */
    } else {
        category = 1;                /* it's a file */
    }

    argc_temp = parse_path(path, argv_temp);
    if ( argc_temp > 4 || argc_temp == 0) {
        DEBUGPRINT("FATFS_ERROR: file path stage should <=4\r\n");
        return FATFS_OTHER_ERROR;
    }

    for (i = 0; i < (argc_temp - 1); i++ ) {     /* create path */
        if ((pos + strlen(argv_temp[i])) > MAX_FILE_PATH) {
            DEBUGPRINT("FATFS_ERROR: file path: %s/%s length should less than %d\r\n", path_temp, argv_temp[i], MAX_FILE_PATH);
            return FATFS_OTHER_ERROR;
        }

        if (i > 0) {
            path_temp[pos++] = '/';
        }
        strcpy(&path_temp[pos], argv_temp[i]);
        pos += strlen(argv_temp[i]);

        res = f_mkdir(path_temp);
        if (res != FR_EXIST && res != FR_OK) {
            return res;
        }
    }

    if ((pos + strlen(argv_temp[argc_temp - 1])) > MAX_FILE_PATH) {
        DEBUGPRINT("FATFS_ERROR: file path: %s/%s too long, should less than %d\r\n", path_temp, argv_temp[i], MAX_FILE_PATH);
        return FATFS_OTHER_ERROR;
    }

    if (argc_temp > 1) {
        path_temp[pos++] = '/';
    }
    strcpy(&path_temp[pos], argv_temp[argc_temp - 1]);

    if (category) {
        res = file_create(path_temp);
    } else {
        res = f_mkdir(path_temp);
    }

    return res;
}

/*!
    \brief      write data to a existed file by append to the end
    \param[in]  path:pointer to file path
    \param[in]  data:pointer to data need to be written
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
int fatfs_append(char *path, char *data)
{
    int res = FR_OK;
    FIL *fil;
    FILINFO fno;
    uint32_t byte_written = 0;

    res = f_stat(path, &fno);
    if (res != FR_OK)
        return res;

    fil = (FIL *)sys_malloc(sizeof(FIL));
    if (fil == NULL) {
        DEBUGPRINT("ERROR_FATFS: fatfs_file_append, malloc fil failed\r\n");
        return FATFS_OTHER_ERROR;
    }

    res = f_open(fil, path, FA_WRITE | FA_OPEN_APPEND);
    if (res == FR_OK) {
        res = f_write(fil, data, strlen(data), (UINT *)&byte_written);
        if (res != FR_OK) {
            fresult_analyse(res);
        }
        res = f_close(fil);
    }

    sys_mfree(fil);
    return res;
}

/*!
    \brief      read data from a existed file and print it
    \param[in]  path:pointer to file path
    \param[in]  len:length to read, 0 for read whole file
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
int fatfs_read(char *path, uint32_t len)
{
    int res = FR_OK;
    FILINFO fno;
    FIL *fil = NULL;
    TCHAR *buff = NULL;
    uint32_t read_len = 0, succeed = 0;

    res = f_stat(path, &fno);
    if (res != FR_OK)
        return res;

    fil = (FIL *)sys_malloc(sizeof(FIL));
    if (NULL == fil) {
        DEBUGPRINT("ERROR_FATFS: fatfs_file_read, malloc fil failed\r\n");
        return FATFS_OTHER_ERROR;
    }

    res = f_open(fil, path, FA_READ);
    if (res != FR_OK)
        goto Exit2;

    read_len = ((f_size(fil) >= len) && (len != 0)) ? len : f_size(fil);
    if (read_len == 0 ) {
        DEBUGPRINT("ERROR_FATFS: file is empty\r\n");
        res = f_close(fil);
        goto Exit2;
    }

    buff = (void *)sys_malloc(read_len);
    if (NULL == buff) {
        DEBUGPRINT("ERROR_FATFS: fatfs_file_read, malloc read buff failed\r\n");
        res = f_close(fil);
        goto Exit2;
    }

    res = f_read(fil, buff, read_len, (UINT *)&read_len);
    if (res == FR_OK) {
        DEBUGPRINT("Fatfs: read %dBytes, conntent:", read_len, (char *)buff);
        for (int i = 0; i < read_len; i++) {
            DEBUGPRINT("0x%02x ", buff[i]);
        }
        DEBUGPRINT("\r\n");
    } else {
        fresult_analyse(res);
    }
    res  = f_close(fil);

    sys_mfree(buff);
Exit2:
    sys_mfree(fil);
    return res;
}

/*!
    \brief      delete a whole dir or a file
    \param[in]  path:pointer to file path
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
int fatfs_delete(char *path)
{
    int res = FR_OK;
    FILINFO fno;
    char *path_buffer = NULL, *path_temp = NULL;

    res = f_stat(path, &fno);
    if (res == FR_OK) {
        path_buffer = (char *)sys_malloc((MAX_FILE_PATH + 2) * 2);
        if (path_buffer == NULL) {
            DEBUGPRINT("ERROR_FATFS: fatfs_delete, malloc path_buffer failed\r\n");
            return FATFS_OTHER_ERROR;
        }
        path_temp = path_buffer + MAX_FILE_PATH + 2;
        strncpy((char *)path_buffer, path, MAX_FILE_PATH + 2);
        res = delete_files(path_buffer, path_temp);
        sys_mfree(path_buffer);
    }
    return res;
}

/*!
    \brief      show all file/dir under path
    \param[in]  path:pointer to file path
    \param[in]  root_path:1 for show all files and dir, 0 for show files and dir under input path
    \param[out] none
    \retval     FRESULT: fatfs operation results
*/
int fatfs_show(char *path, uint8_t root_path)
{
    int res = FR_OK;
    char path_buffer[MAX_FILE_PATH + 2] = {0};

    if (root_path) {
        strncpy((char *)path_buffer, "/", MAX_FILE_PATH);
    } else {
        strncpy((char *)path_buffer, path, MAX_FILE_PATH);
    }
    res = scan_dir((char *)path_buffer);
    return res;
}

/*!
    \brief      handler to fatfs cmd input
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     0 for succeed, 1 for failed
*/
uint32_t cmd_fatfs_exec(int argc, char **argv)
{
    int res = FR_OK;
    uint32_t len = 0;

    if (strcmp(argv[1], "create") == 0) {
        if (argc == 3) {
            res = fatfs_create(argv[2]);
            fresult_analyse(res);
            return 0;
        }

    } else if (strcmp(argv[1], "append") == 0) {
        if (argc == 4) {
            res = fatfs_append(argv[2], argv[3]);
            fresult_analyse(res);
            return 0;
        }

    } else if (strcmp(argv[1], "read") == 0) {
        if (argc == 4) {
            len = (uint32_t)atoi(argv[3]);
            res = fatfs_read(argv[2], len);
            fresult_analyse(res);
            return 0;
        } else if (argc == 3) {
            res = fatfs_read(argv[2], 0);
            fresult_analyse(res);
            return 0;
        }


    } else if (strcmp(argv[1], "delete") == 0) {
        if (argc == 3) {
            res = fatfs_delete(argv[2]);
            fresult_analyse(res);
            return 0;
        }

    } else if (strcmp(argv[1], "show") == 0) {
        if (argc == 2) {
            res = fatfs_show(NULL, 1);
            fresult_analyse(res);
            return 0;

        } else if (argc == 3) {
            res = fatfs_show(argv[2], 0);
            fresult_analyse(res);
            return 0;
        }
    }
    return 1;
}
#endif

# if 0
/*!
    \brief      write data to flash after erasing the sector
    \param[in]  address: flash offset address
    \param[in]  pdata: pointer to data to be written
    \param[in]  size: length of data write to flash
    \param[out] none
    \retval     result: 0 for success, -1 for fail
*/
static int fs_normal_flash_write(uint32_t address, const uint8_t* pdata, uint16_t size)
{
    int result = 0;
    uint32_t sector_start_address = 0;
    uint16_t sector_offset = 0;
    uint16_t sector_remain = 0;
    uint32_t i;
    uint8_t *temp_buffer;
    uint8_t *temp_data;

    temp_buffer = (uint8_t *)sys_malloc(FLASH_SIP_PAGE_SIZE);
    if (temp_buffer == NULL) {
        DEBUGPRINT("fatfs:fs_normal_flash_write, malloc temp buffer failed\r\n");
        return -1;
    }

    temp_data = (uint8_t *)sys_malloc(size);
    if (temp_data == NULL) {
        DEBUGPRINT("fatfs:fs_normal_flash_write, malloc temp_data failed\r\n");
        return -1;
    }
    sys_memcpy(temp_data, pdata, size);

    sector_start_address = address & (~(FLASH_SIP_PAGE_SIZE - 1));     /* sector start address */
    sector_offset = address & (FLASH_SIP_PAGE_SIZE - 1);             /* sector inner offset */
    sector_remain = FLASH_SIP_PAGE_SIZE - sector_offset;             /* sector remain space*/

    if (size <= sector_remain) {
        sector_remain = size;
    }

    while(1) {
        /* read whole sector */
        result = mbl_flash_indirect_read(sector_start_address, temp_buffer, FLASH_SIP_PAGE_SIZE);
        if (result)
            break;

        for (i = 0; i < sector_remain; i++) {                     /* check if sector has been written before */
            if (temp_buffer[sector_offset + i] != 0xFF)
                break;
        }

        if (i < sector_remain) {
            /* erase sector */
            result = mbl_flash_erase(sector_start_address, FLASH_SIP_PAGE_SIZE);
            if (result)
                break;

            sys_memcpy((temp_buffer + sector_offset), temp_data, sector_remain);

            result = mbl_flash_write(sector_start_address, temp_buffer, FLASH_SIP_PAGE_SIZE);
            if (result)
                break;
        } else {
            result = mbl_flash_write(address, temp_data, sector_remain);
            if (result)
                break;
        }

        if (size == sector_remain)
            break;

        sector_start_address += FLASH_SIP_PAGE_SIZE;
        sector_offset = 0;
        temp_data += sector_remain;
        address += sector_remain;
        size -= sector_remain;

        if (size > FLASH_SIP_PAGE_SIZE) {
            sector_remain = FLASH_SIP_PAGE_SIZE;
        } else {
            sector_remain = size;
        }
    }

    sys_mfree(temp_buffer);
    sys_mfree(temp_data);
    result = mbl_flash_erase(address, FLASH_SIP_PAGE_SIZE);
    if (result)
        return result;

    result = mbl_flash_write(address, pdata, FLASH_SIP_PAGE_SIZE);
    return result;
}
#endif

/*!
    \brief      write data to flash
    \param[in]  sector:	Start sector in LBA
    \param[in]  buff: Data buffer to store write data
    \param[in]  count: Number of sectors to write
    \retval     result: 0 for success, -1 for fail
*/
int fs_flash_write(LBA_t sector,const BYTE *buff, UINT count)
{
    int32_t result;

#ifdef USE_WL
    result = wl_flash_erase_range(&wl_config_global, (sector * FATFS_SECTOR_SIZE), (count * FATFS_SECTOR_SIZE));
    if (result)
        return RES_ERROR;

    result = wl_flash_write(&wl_config_global, (sector * FATFS_SECTOR_SIZE), buff, (count * FATFS_SECTOR_SIZE));
    if (result)
        return RES_ERROR;
#else
    result = mbl_flash_erase((sector * FATFS_SECTOR_SIZE + RE_IMG_1_NSPE_OFFSET), (count * FATFS_SECTOR_SIZE));
    if (result)
        return RES_ERROR;

    result = mbl_flash_write((sector * FATFS_SECTOR_SIZE + RE_IMG_1_NSPE_OFFSET), (uint8_t *)buff, (count * FATFS_SECTOR_SIZE));
    if (result)
        return RES_ERROR;
#endif

    return RES_OK;
}

/*!
    \brief      read data from flash
    \param[in]  sector:	Start sector in LBA
    \param[out] buff: Data buffer to store read data
    \param[in]  count: Number of sectors to read
    \retval     result: 0 for success, -1 for fail
*/
int fs_flash_read(LBA_t sector, BYTE *buff, UINT count)
{
    DRESULT res = RES_OK;
    int32_t result;

#ifdef USE_WL
    result = wl_flash_read(&wl_config_global, (sector * FATFS_SECTOR_SIZE), buff, (count * FATFS_SECTOR_SIZE));
#else
    result = mbl_flash_indirect_read((sector * FATFS_SECTOR_SIZE + RE_IMG_1_NSPE_OFFSET), buff, (count * FATFS_SECTOR_SIZE));
#endif
    if (result != 0) {
        DEBUGPRINT("FATFS_ERROR: read from flash error!\r\n");
        res = RES_ERROR;
    }

    return res;
}
