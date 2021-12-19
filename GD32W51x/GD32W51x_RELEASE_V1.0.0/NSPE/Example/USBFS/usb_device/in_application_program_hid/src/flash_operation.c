/*!
    \file    flash_operation.c
    \brief   flash operation driver

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
#include "flash_operation.h"
#include "usbd_conf.h"

/*!
    \brief      erase data of flash
    \param[in]  address: sector address/code
    \param[in]  file_length: length of the file 
    \param[in]  report_buffer: report buffer
    \param[out] none
    \retval     none
*/
void flash_erase(uint32_t address, uint32_t file_length, uint8_t* report_buffer)
{
    uint16_t page_count = 0U, i = 0U;

    page_count = report_buffer[6];

    for (i = 0U; i < page_count; i++) {
        /* call the standard flash erase-page function */
        fmc_page_erase(address);

        address += PAGE_SIZE;
    }
}

/*!
    \brief      write data to sectors of flash
    \param[in]  data: data to be written
    \param[in]  addr: sector address/code
    \param[in]  len: length of data to be written (in bytes)
    \param[out] none
    \retval     none
*/
void iap_data_write (uint8_t *data, uint32_t addr, uint32_t len)
{
    uint32_t idx = 0U;

    /* check if the address is in protected area */
    if (IS_PROTECTED_AREA(addr)) {
        return;
    }
    
    if (len & 0x03U) {/* not an aligned data */
        for (idx = len; idx < ((len & 0xFFFCU) + 4U); idx++) {
            data[idx] = 0xFFU;
        }
    }

   /* unlock the flash program erase controller */
    fmc_unlock();

    /* data received are word multiple */
    for (idx = 0U; idx < len; idx += 4U) {
        if (FMC_READY == fmc_word_program(addr, *(uint32_t *)(data + idx))) {
            addr += 4U;
        } else {
            while(1){
            }
        }
    }

    fmc_lock();
}
