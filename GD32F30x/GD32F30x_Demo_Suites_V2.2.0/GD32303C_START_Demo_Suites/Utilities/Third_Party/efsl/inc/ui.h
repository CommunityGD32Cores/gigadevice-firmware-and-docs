

#ifndef __UI_H__
#define __UI_H__

/*****************************************************************************/
#include "stdint.h"
#include "gd32f30x.h"
#include "fs.h"
#include "types.h"
#include "fat.h"
#include "dir.h"
#include "config.h"
/*****************************************************************************/

short listFiles(FileSystem *fs, char *dir);
esint16 rmfile(FileSystem *fs,euint8* filename);
esint8 mkdir(FileSystem *fs,eint8* dirname);

#endif
