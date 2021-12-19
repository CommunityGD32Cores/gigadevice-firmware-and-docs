/*!
    \file    usbh_usr.c
    \brief   user application layer for USBFS host-mode MSC class operation

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

#include <string.h>
#include "usbh_usr.h"
#include "drv_usb_hw.h"
#include "ff.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bbb.h"

extern usb_core_driver usbh_core;
extern usbh_host usb_host;

FATFS fatfs;
FIL file;

uint8_t line_idx;
uint8_t usbh_usr_application_state = USBH_USR_FS_INIT;

/*  points to the DEVICE_PROP structure of current device */
usbh_user_cb usr_cb =
{
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    usbh_usr_msc_application,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};

const uint8_t MSG_HOST_HEADER[] = "USBFS MSC Host";
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v2.0.0";

/* local function prototypes ('static') */
static uint8_t explore_disk (char* path, uint8_t recu_level);
static void toggle_leds (void);

/*!
    \brief      user operation for host-mode initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0U;

    if (0U == startup) {
        startup = 1U;

        /* configure the LEDs and KEYs*/
        gd_eval_led_init(LED2);
        gd_eval_led_init(LED3);
        gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_GPIO);

        gd_eval_com_init(EVAL_COM0);

        LCD_UsrLog("\r\n++++USB host library started++++\r\n");
    }
}

/*!
    \brief      deinitialize user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
    usbh_usr_application_state = USBH_USR_FS_INIT;
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
    LCD_UsrLog("> Device Attached.\r\n");
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error(void)
{
    LCD_ErrLog("> UNRECOVERED ERROR STATE.\r\n");
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected(void)
{
    LCD_UsrLog("> Device Disconnected.\r\n");
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    LCD_UsrLog("> Reset the USB device.\r\n");
}

/*!
    \brief      user operation for detecting device speed
    \param[in]  device_speed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if (PORT_SPEED_HIGH == device_speed) {
        LCD_UsrLog("> High speed device detected.\r\n");
    } else if(PORT_SPEED_FULL == device_speed) {
        LCD_UsrLog("> Full speed device detected.\r\n");
    } else if(PORT_SPEED_LOW == device_speed) {
        LCD_UsrLog("> Low speed device detected.\r\n");
    } else {
        LCD_ErrLog("> Device Fault.\r\n");
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  device_desc: device descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
    usb_desc_dev *pDevStr = device_desc;

    LCD_DevInformation("VID: %04Xh\r\n", (uint32_t)pDevStr->idVendor);
    LCD_DevInformation("PID: %04Xh\r\n", (uint32_t)pDevStr->idProduct);
}

/*!
    \brief      usb device is successfully assigned the Address 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_address_assigned(void)
{
}

/*!
    \brief      user operation when configuration descriptor is available
    \param[in]  cfg_desc: pointer to configuration descriptor
    \param[in]  itf_desc: pointer to interface descriptor
    \param[in]  ep_desc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc,
                                           usb_desc_itf *itf_desc,
                                           usb_desc_ep *ep_desc)
{
    usb_desc_itf *id = itf_desc;

    if (0x08U  == (*id).bInterfaceClass) {
        LCD_UsrLog("> Mass storage device connected.\r\n");
    } else if (0x03U  == (*id).bInterfaceClass) {
        LCD_UsrLog("> HID device connected.\r\n");
    } else {

    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  manufacturer_string: manufacturer string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    LCD_DevInformation("Manufacturer: %s\r\n", (char *)manufacturer_string);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    LCD_DevInformation("Product: %s\r\n", (char *)product_string);
}

/*!
    \brief      user operation when serialNum string exists
    \param[in]  serial_num_string: serialNum string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    LCD_DevInformation("Serial Number: %s\r\n", (char *)serial_num_string);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    LCD_UsrLog("> Enumeration completed.\r\n");
    LCD_UsrLog(">>> To see the disk information: \r\n");
    LCD_UsrLog(">>> Press CET Key...\r\n");
}

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    LCD_ErrLog("> Device not supported.\r\n");
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     user response for user key
*/
usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USBH_USER_NO_RESP;

    /*key B3 is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_TAMPER_WAKEUP)) {
        usbh_usr_status = USBH_USER_RESP_OK;
    }

    return usbh_usr_status;
}

/*!
    \brief      user operation for device overcurrent detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
    LCD_ErrLog("> Overcurrent detected.\r\n");
}

/*!
    \brief      demo application for mass storage
    \param[in]  none
    \param[out] none
    \retval     status
*/
int usbh_usr_msc_application(void)
{
    FRESULT res;
    msc_lun info;
    uint8_t WriteTextBuff[] = "GD32 Connectivity line Host Demo application using FAT_FS   ";
    uint16_t bytesWritten, bytesToWrite;

    switch(usbh_usr_application_state)
    {
        case USBH_USR_FS_INIT:
            /* initializes the file system*/
            if (FR_OK != f_mount(&fatfs, "0:/", 0U)) {
                LCD_ErrLog("> Cannot initialize File System.\r\n");

                return(-1);
            }

            LCD_UsrLog("> File System initialized.\r\n");
            if (USBH_OK == usbh_msc_lun_info_get(&usb_host, 0U, &info))
            {
                LCD_UsrLog("> Disk capacity: %ud Bytes.\r\n", info.capacity.block_nbr * info.capacity.block_size);
            }

            usbh_usr_application_state = USBH_USR_FS_READLIST;
            break;

        case USBH_USR_FS_READLIST:
            LCD_UsrLog("> Exploring disk flash ...\r\n");
            LCD_UsrLog(">>> To see the root content of disk \r\n");
            LCD_UsrLog(">>> Press CET Key...\r\n");

            /*Key B3 in polling*/
            while ((usbh_core.host.connect_status) && \
                (SET == gd_eval_key_state_get (KEY_TAMPER_WAKEUP))) {
                toggle_leds();
            }

            explore_disk("0:/", 1U);
            line_idx = 0U;
            usbh_usr_application_state = USBH_USR_FS_WRITEFILE;
            break;

        case USBH_USR_FS_WRITEFILE:
            usb_mdelay(100U);
            LCD_UsrLog(">>> Press C Key to write file\r\n");

            /*key b3 in polling*/
            while ((usbh_core.host.connect_status) && \
                    (SET == gd_eval_key_state_get (KEY_TAMPER_WAKEUP))) {
                toggle_leds();
            }

            LCD_UsrLog("> Writing File to disk flash ...\r\n");

            /* register work area for logical drives */
            f_mount(&fatfs, "0:/", 1U);

            if (FR_OK == f_open(&file, "0:GD32.TXT", (uint8_t)(FA_CREATE_ALWAYS | FA_WRITE))) {
                /* write buffer to file */
                bytesToWrite = sizeof(WriteTextBuff); 
                res = f_write (&file, WriteTextBuff, (uint32_t)bytesToWrite, (void *)&bytesWritten);
                /* EOF or error */
                if ((0U == bytesWritten) || (FR_OK != res)) {
                    LCD_ErrLog("> GD32.TXT CANNOT be writen.\r\n");
                } else {
                    LCD_UsrLog("> GD32.TXT created in the disk.\r\n");
                }

                /* close file and filesystem */
                f_close(&file);
                f_mount(NULL, "0:/", 1U); 
            } else {
                LCD_UsrLog("> GD32.TXT created in the disk.\r\n");
            }

            usbh_usr_application_state = USBH_USR_FS_DEMOEND;
            LCD_UsrLog("> The MSC host demo is end.\r\n");
            break;

        case USBH_USR_FS_DEMOEND:
            break;

        default:
            break;
    }

    return 0;
}

/*!
    \brief      displays disk content
    \param[in]  path: pointer to root path
    \param[in]  recu_level: recursive level
    \param[out] none
    \retval     status
*/
static uint8_t explore_disk (char* path, uint8_t recu_level)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    char *fn;

    res = f_opendir(&dir, path);

    if (FR_OK == res) {
        while ((usbh_core.host.connect_status)) {
            res = f_readdir(&dir, &fno);
            if ((FR_OK != res) || (0U == fno.fname[0])) {
                break;
            }

            if ('.' == fno.fname[0]) {
                continue;
            }

            fn = fno.fname;

            line_idx++;

            if (line_idx > 4U) {
                line_idx = 0U;
                LCD_UsrLog(">>> Press CET Key to continue\r\n");

                /*key B3 in polling*/
                while ((usbh_core.host.connect_status) && \
                    (SET == gd_eval_key_state_get (KEY_TAMPER_WAKEUP))) {
                    toggle_leds();
                }
            }

            if (1U == recu_level) {
                LCD_UsrLog("   |__");
            } else if (2U == recu_level) {
                LCD_UsrLog("   |   |__");
            } else {
            
            }

            if (AM_DIR == fno.fattrib) {
                LCD_UsrLog("%s\r\n", fno.fname);
            } else {
                LCD_UsrLog("%s\r\n", fno.fname);
            }

            if (((uint8_t)AM_DIR == fno.fattrib) && (1U == recu_level)) {
                explore_disk(fn, 2U);
            }
        }
    }

    return res;
}

/*!
    \brief      toggle LEDs to shows user input state
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void toggle_leds(void)
{
    static uint32_t i;

    if (0x10000U == i++) {
        gd_eval_led_toggle(LED2);
        gd_eval_led_toggle(LED3);
        i = 0U;
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC));
    return ch;
}
