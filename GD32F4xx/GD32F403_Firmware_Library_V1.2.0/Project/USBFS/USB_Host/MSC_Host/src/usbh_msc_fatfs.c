/*!
    \file  usbh_msc_fatfs.c 
    \brief this file includes the mass storage related functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"

static volatile DSTATUS stat = STA_NOINIT; /* disk status */

extern usb_core_handle_struct          usb_core_dev;
extern usbh_host_struct                usb_host;
extern usbh_state_handle_struct        usbh_state_core;

/*!
    \brief      initialize disk drive
    \param[in]  drv: physical drive number (0)
    \param[out] none
    \retval     status
*/
DSTATUS disk_initialize (BYTE drv)
{
    if (hcd_is_device_connected(&usb_core_dev)) {
        stat &= ~STA_NOINIT;
    }

    return stat;
}

/*!
    \brief      get disk status
    \param[in]  drv: physical drive number (0)
    \param[out] none
    \retval     status
*/
DSTATUS disk_status (BYTE drv)
{
    /* supports only single drive */
    if (drv) return STA_NOINIT; 

    return stat;
}

/*!
    \brief      read sector(s)
    \param[in]  drv: physical drive number (0)
    \param[in]  buff: pointer to the data buffer to store read data
    \param[in]  sector: start sector number (LBA)
    \param[in]  count: sector count (1..255)
    \param[out] none
    \retval     status
*/
DRESULT disk_read (BYTE drv, 
                   BYTE *buff, 
                   DWORD sector, 
                   BYTE count)
{
    BYTE status = USBH_MSC_OK;

    if (drv || !count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    if (hcd_is_device_connected(&usb_core_dev)) {
        do {
            status = usbh_msc_read10(&usb_core_dev, buff,sector, 512 * count);
            usbh_msc_handle_botxfer(&usb_core_dev, &usb_host,  &usbh_state_core);

            if (!hcd_is_device_connected(&usb_core_dev)) {
                return RES_ERROR;
            }
        }
        while(USBH_MSC_BUSY == status);
    }

    if (USBH_MSC_OK == status) return RES_OK;

    return RES_ERROR;
}

#if _READONLY == 0

/*!
    \brief      write sector(s)
    \param[in]  drv: physical drive number (0)
    \param[in]  buff: pointer to the data buffer to be written
    \param[in]  sector: start sector number (LBA)
    \param[in]  count: sector count (1..255)
    \param[out] none
    \retval     status
*/
DRESULT disk_write (BYTE drv, 
                    const BYTE *buff, 
                    DWORD sector, 
                    BYTE count)
{
    BYTE status = USBH_MSC_OK;
    if (drv || !count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;
    if (stat & STA_PROTECT) return RES_WRPRT;

    if (hcd_is_device_connected(&usb_core_dev)) {
        do {
            status = usbh_msc_write10(&usb_core_dev,(BYTE*)buff,sector,512 * count);
            usbh_msc_handle_botxfer(&usb_core_dev, &usb_host,  &usbh_state_core);

            if (!hcd_is_device_connected(&usb_core_dev)) {
                return RES_ERROR;
            }
        } while(USBH_MSC_BUSY == status);
    }

    if (USBH_MSC_OK == status) return RES_OK;

    return RES_ERROR;
}

#endif /* _READONLY == 0 */

/*-----------------------------------------------------------------------*/
/* miscellaneous functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0

/*!
    \brief      I/O control function
    \param[in]  drv: physical drive number (0)
    \param[in]  ctrl: control code
    \param[in]  buff: buffer to send/receive control data
    \param[out] none
    \retval     status
*/
DRESULT disk_ioctl (BYTE drv, 
                    BYTE ctrl, 
                    void *buff)
{
    DRESULT res = RES_OK;

    if (drv) return RES_PARERR;

    res = RES_ERROR;

    if (stat & STA_NOINIT) return RES_NOTRDY;

    switch (ctrl) {
        case CTRL_SYNC:         /* make sure that no pending write process */
            res = RES_OK;
            break;

        case GET_SECTOR_COUNT:  /* get number of sectors on the disk (DWORD) */
            *(DWORD*)buff = (DWORD) usbh_msc_param.msc_capacity;
            res = RES_OK;
            break;

        case GET_SECTOR_SIZE:  /* get R/W sector size (WORD) */
            *(WORD*)buff = 512U;
            res = RES_OK;
            break;

        case GET_BLOCK_SIZE:   /* get erase block size in unit of sector (DWORD) */
            *(DWORD*)buff = 512U;
            break;

        default:
            res = RES_PARERR;
            break;
    }

    return res;
}
#endif /* _USE_IOCTL != 0 */

