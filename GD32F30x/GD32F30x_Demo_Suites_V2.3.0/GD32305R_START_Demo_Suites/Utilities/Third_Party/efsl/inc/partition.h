
/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename : partition.h                                                      *
* Description : Headerfile for partition.c                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
\*****************************************************************************/

#ifndef __PARTITION_H__
#define __PARTITION_H__

/*****************************************************************************/
#include "stdint.h"
#include "config.h"
#include "error.h"
#include "interface.h"
#include "disc.h"
#include "types.h"
/*****************************************************************************/

#define PT_FAT12  0x01
#define PT_FAT16A 0x04
#define PT_FAT16  0x06
#define PT_FAT32  0x0B
#define PT_FAT32A 0x5C
#define PT_FAT16B 0x5E

/*************************************************************************************\
              Partition
               -------
* Disc*		disc				Pointer to disc containing this partition.
* eint8		activePartition	 	Array subscript for disc->partitions[activePartition]
\*************************************************************************************/
struct Partition{
	Disc *disc;
 	esint8 activePartition;
};
typedef struct Partition Partition;

void part_initPartition(Partition *part,Disc* refDisc);
eint16 part_isFatPart(euint8 type);
esint8 part_readBuf(Partition *part, euint32 address, euint8* buf);
esint8 part_readPartBuf(Partition *part, euint32 address, euint8* buf, euint32 offset, euint16 len);
eint16 part_writeBuf(Partition *part,euint32 address,euint8* buf);
euint8* part_getSect(Partition *part, euint32 address,euint8 mode);
esint8 part_relSect(Partition *part, euint8* buf);
esint8 part_flushPart(Partition *part,euint32 addr_l, euint32 addr_h);
esint8 part_directSectorRead(Partition *part, euint32 address, euint8* buf);
esint8 part_directSectorWrite(Partition *part, euint32 address, euint8* buf);
euint32 part_getRealLBA(Partition *part,euint32 address);

#include "extract.h"

#endif
