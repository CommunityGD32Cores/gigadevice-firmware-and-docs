/*!
    \file  usbh_data.h
    \brief header file for USB data file

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#ifndef __USBH_DATA_H
#define __USBH_DATA_H

#include "usbh_core.h"
#include "usb_conf.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"

#define FS_ROOT     file_text1

struct fsdata_file {
  const struct fsdata_file *next;
  const uint8_t *name;
  const uint8_t *data;
  const uint16_t len;
};

struct fsdata_file_noconst {
  struct fsdata_file *next;
  uint8_t *name;
  uint8_t *data;
  unsigned long len;
};

extern const uint8_t file1[];
extern const uint8_t file2[];
extern const uint8_t file3[];
extern const uint8_t Stored_File1[368];
extern const uint8_t Stored_File2[5603];
extern const uint8_t Stored_File3[800];

static const struct fsdata_file file_text3[] = {NULL, file3, Stored_File3, sizeof(Stored_File3)};
static const struct fsdata_file file_text2[] = {file_text3, file2, Stored_File2, sizeof(Stored_File2)};
static const struct fsdata_file file_text1[] = {file_text2, file1, Stored_File1, sizeof(Stored_File1)};

#endif /* __USBH_DATA_H */
