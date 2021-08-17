/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "gd32f1x0.h"
#include "spi_sd.h"
#include "stdio.h"
#define BLOCK_SIZE            512 /* Block Size in Bytes */




/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
  BYTE drv        /* Physical drive nmuber (0..) */
)
{
  sd_error_enum  status;
  /* Supports only single drive */
  if (drv)
  {
    return STA_NOINIT;
  }
/*-------------------------- SD Init ----------------------------- */
  status = sd_card_init();
  if (status!=SD_RESPONSE_NO_ERROR )
  {
    return STA_NOINIT;
  }
  else
  {
    return RES_OK;
  }

}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
  BYTE drv    /* Physical drive nmuber (0..) */
)
{
  return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
  BYTE drv,   /* Physical drive nmuber (0..) */
  BYTE *buff,   /* Data buffer to store read data */
  DWORD sector, /* Sector address (LBA) */
  BYTE count    /* Number of sectors to read (1..255) */
)
{

  if (count > 1)
  {  
    sd_multiblocks_read(buff, sector, BLOCK_SIZE, count);
    spi_write(0xff);

  }
  else
  {
    sd_block_read(buff, sector, BLOCK_SIZE);
    spi_write(0xff);
  }
  return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
  BYTE drv,     /* Physical drive nmuber (0..) */
  const BYTE *buff, /* Data to be written */
  DWORD sector,   /* Sector address (LBA) */
  BYTE count      /* Number of sectors to write (1..255) */
)
{

  if (count > 1)
  {
    sd_multiblocks_write((uint8_t *)buff, sector, BLOCK_SIZE, count);
    spi_write(0xff);

  }
  else
  {
    sd_block_write((uint8_t *)buff,sector, BLOCK_SIZE);
    spi_write(0xff);
  }
  return RES_OK;
}
#endif /* _READONLY */




/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
  BYTE drv,   /* Physical drive nmuber (0..) */
  BYTE ctrl,    /* Control code */
  void *buff    /* Buffer to send/receive control data */
)
{

  return RES_OK;
}
 
/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/ 
DWORD get_fattime(void)
{

  return 0;

} 
