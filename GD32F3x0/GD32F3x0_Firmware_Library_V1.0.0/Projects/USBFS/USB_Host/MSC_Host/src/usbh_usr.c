/*!
    \file  usbh_usr.c
    \brief user application layer for USBFS host-mode MSC class operation
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include <string.h>
#include "usbh_usr.h"
#include "ff.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

extern usb_core_handle_struct          usb_core_dev;

FATFS fatfs;
FIL file;

uint8_t line_idx = 0;
uint8_t usbh_usr_application_state = USBH_USR_FS_INIT;

/*  points to the DEVICE_PROP structure of current device */
usbh_user_callback_struct usr_cb =
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

const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized.";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached.";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected.";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed.";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected.";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected.";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected.";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault.";

const uint8_t MSG_HOST_HEADER[]      = "> USBFS MSC Host";
const uint8_t MSG_HOST_FOOTER[]      = "> USB Host Library v1.0.0";

const uint8_t MSG_LIB_START[]        = "##### USB Host library started #####";
const uint8_t MSG_DEV_NOSUP[]        = "> Device not supported.";
const uint8_t MSG_OVERCURRENT[]      = "> Overcurrent detected.";
const uint8_t MSG_RESET_DEV[]        = "> Reset the USB device.";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected.";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected.";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: ";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected.";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE.";
const uint8_t MSG_FILE_NOTINIT[]     = "> Cannot initialize File System.";
const uint8_t MSG_FILE_INIT[]        = "> File System initialized.";
const uint8_t MSG_Write_File[]       = "> Writing File to disk flash ...";
const uint8_t MSG_Write_Protect[]    = "> Disk flash is write protected ";
const uint8_t MSG_NOT_WRITE[]        = "> GD32.TXT CANNOT be writen.";
const uint8_t MSG_CREATE_FILE[]      = "> GD32.TXT created in the disk.";

static uint8_t explore_disk     (char* path, uint8_t recu_level);
static void toggle_leds         (void);

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
        gd_eval_led_init(LED1);
        gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);
        gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
        gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
        
        printf("\r\n %s ",(uint8_t *)MSG_LIB_START);
    }
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
      printf("\r\n %s ",(uint8_t *)MSG_DEV_ATTACHED);
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    printf("\r\n %s ",(uint8_t *)MSG_UNREC_ERROR);
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    printf("\r\n %s ",(uint8_t *)MSG_DEV_DISCONNECTED);
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    printf("\r\n %s ",(uint8_t *)MSG_RESET_DEV);
}

/*!
    \brief      user operation for detectting device speed
    \param[in]  device_speed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint8_t device_speed)
{
    
    if (HPRT_PRTSPD_HIGH_SPEED == device_speed) {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_HIGHSPEED);
    } else if(HPRT_PRTSPD_FULL_SPEED == device_speed) {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_FULLSPEED);
    } else if(HPRT_PRTSPD_LOW_SPEED == device_speed) {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_LOWSPEED);
    } else {
        printf("\r\n %s ",(uint8_t *)MSG_DEV_ERROR);
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
    usb_descriptor_device_struct *pDevStr = device_desc;
  
    printf("\r\n > VID: %04Xh ",(uint32_t)pDevStr->idVendor);

    printf("\r\n > PID: %04Xh ",(uint32_t)pDevStr->idProduct);
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
void usbh_user_configuration_descavailable(usb_descriptor_configuration_struct *cfg_desc,
                                          usb_descriptor_interface_struct *itf_desc,
                                          usb_descriptor_endpoint_struct *ep_desc)
{
    usb_descriptor_interface_struct *id = itf_desc;  

    if(0x08U  == (*id).bInterfaceClass) {
        printf("\r\n %s ",(uint8_t *)MSG_MSC_CLASS);
    } else if(0x03U  == (*id).bInterfaceClass) {
        printf("\r\n %s ",(uint8_t *)MSG_HID_CLASS);
    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  manufacturer_string: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
  printf("\r\n > manufacture string is : %s ",(uint8_t *)manufacturer_string);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    printf("\r\n > product string is : %s ",(uint8_t *)product_string);
}

/*!
    \brief      user operatin when serialNum string exists
    \param[in]  serial_num_string: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    printf("\r\n > Serial Number string is : %s ",(uint8_t *)serial_num_string);

}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    uint8_t Str1[] = "> To see the disk information: ";
    uint8_t Str2[] = "> Press User Key...";
  
    printf("\r\n %s ",(uint8_t *)MSG_DEV_ENUMERATED);
    printf("\r\n %s ",(uint8_t *)Str1);
    printf("\r\n %s ",(uint8_t *)Str2);
}

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    printf("\r\n %s ",(uint8_t *)MSG_DEV_NOSUP);
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     user response for user key
*/
usbh_user_status_enum usbh_user_userinput(void)
{
    usbh_user_status_enum usbh_usr_status = USBH_USER_NO_RESP;

    /*key B3 is in polling mode to detect user action */
    if (RESET == gd_eval_key_state_get(KEY_USER)) {
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
    printf("\r\n %s ",(uint8_t *)MSG_OVERCURRENT);
}

/*!
    \brief      demo application for mass storage
    \param[in]  pudev: pointer to device
    \param[in]  id: no use here
    \param[out] none
    \retval     status
*/
int usbh_usr_msc_application(usb_core_handle_struct *pudev, uint8_t id)
{
    FRESULT res;
    uint8_t WriteTextBuff[] = "GD32 Connectivity line Host Demo application using FAT_FS   ";
    uint16_t bytesWritten, bytesToWrite;

    uint8_t Str1[] = "> To see the root content of the disk ";
    uint8_t Str2[] = "> Press Wakeup Key to write file";
    uint8_t Str3[] = "> The MSC host demo is end.";
    uint8_t Str4[] = "> Press Tamper key...";

    switch(usbh_usr_application_state)
    {
        case USBH_USR_FS_INIT:
            /* initialises the file system*/
            if (FR_OK != f_mount(0, &fatfs)) {
                printf("\r\n %s ",(uint8_t *)MSG_FILE_NOTINIT);

                return(-1);
            }
            
            printf("\r\n %s ",(uint8_t *)MSG_FILE_INIT);
            
            printf("\r\n > Disk capacity: %ud Bytes. ",usbh_msc_param.msc_capacity * usbh_msc_param.msc_page_length);

            if (DISK_WRITE_PROTECTED == usbh_msc_param.msc_write_protect) {
                printf("\r\n %s ",(uint8_t *)MSG_WR_PROTECT);
            }

            usbh_usr_application_state = USBH_USR_FS_READLIST;
            break;

        case USBH_USR_FS_READLIST:
            printf("\r\n %s ",(uint8_t *)MSG_ROOT_CONT);
        
            printf("\r\n %s ",(uint8_t *)Str1);
            printf("\r\n %s ",(uint8_t *)Str4);

            /*Key B3 in polling*/
            while ((hcd_is_device_connected(&usb_core_dev)) && \
                (SET == gd_eval_key_state_get (KEY_TAMPER))) {
                toggle_leds();
            }

            explore_disk("0:/", 1);
            line_idx = 0;
            usbh_usr_application_state = USBH_USR_FS_WRITEFILE;
            break;

        case USBH_USR_FS_WRITEFILE:
            if ((void *)0 == pudev->mdelay) {
                pudev->mdelay(100U);
            }
            
            printf("\r\n %s ",(uint8_t *)Str2);

            /*key b3 in polling*/
            while ((hcd_is_device_connected(&usb_core_dev)) && \
                    (SET == gd_eval_key_state_get (KEY_WAKEUP))) {
                toggle_leds();
            }
            
            printf("\r\n %s ",(uint8_t *)MSG_Write_File);


            if (DISK_WRITE_PROTECTED == usbh_msc_param.msc_write_protect) {
                printf("\r\n %s ",(uint8_t *)MSG_Write_Protect);

                usbh_usr_application_state = USBH_USR_FS_DEMOEND;
                break;
            }

            /* register work area for logical drives */
            f_mount(0, &fatfs);

            if (FR_OK == f_open(&file, "0:GD32.TXT", FA_CREATE_ALWAYS | FA_WRITE)) {
                /* write buffer to file */
                bytesToWrite = sizeof(WriteTextBuff); 
                res = f_write (&file, WriteTextBuff, bytesToWrite, (void *)&bytesWritten);   
                /* EOF or error */
                if ((0U == bytesWritten) || (FR_OK != res)) {
                  
                    printf("\r\n %s ",(uint8_t *)MSG_NOT_WRITE);
                } else {
                    printf("\r\n %s ",(uint8_t *)MSG_CREATE_FILE);
                }

                /* close file and filesystem */
                f_close(&file);
                f_mount(0, NULL); 
            } else {
                printf("\r\n %s ",(uint8_t *)MSG_CREATE_FILE);
            }

            usbh_usr_application_state = USBH_USR_FS_DEMOEND;
            printf("\r\n %s ",(uint8_t *)Str3);
            break;

        case USBH_USR_FS_DEMOEND:
          
            break;

        default:
            break;
    }

    return(0);
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

    uint8_t Str2[] = "Press User Key to continue";

    res = f_opendir(&dir, path);

    if (res == FR_OK) {
        while (hcd_is_device_connected(&usb_core_dev)) {
            res = f_readdir(&dir, &fno);
            if (FR_OK != res || 0U == fno.fname[0]) {
                break;
            }

            if ('.' == fno.fname[0]) {
                continue;
            }

            fn = fno.fname;

            line_idx++;

            if (line_idx > 4) {
                line_idx = 0;
              
                printf("\r\n %s ",(uint8_t *)Str2);


                /*key B3 in polling*/
                while ((hcd_is_device_connected(&usb_core_dev)) && \
                    (SET == gd_eval_key_state_get (KEY_USER))) {
                    toggle_leds();
                }
            } 

            if (1U == recu_level) {
                uint8_t temp[] = "   |__";
                printf("\r\n %s ",(uint8_t *)temp);
            } else if(2U == recu_level) {
                uint8_t temp[] = "   |   |__";
                printf("\r\n %s ",(uint8_t *)temp);
            }

            if (AM_DIR == (fno.fattrib & AM_MASK)) {
                printf("\r\n %s ",(uint8_t *)fno.fname);
            } else {
                printf("\r\n %s ",(uint8_t *)fno.fname);
            }

            if ((AM_DIR == (fno.fattrib & AM_MASK)) && (1U == recu_level)) {
                explore_disk(fn, 2);
            }
        }
    }

    return res;
}

/*!
    \brief      toggle leds to shows user input state
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void toggle_leds(void)
{
    static uint32_t i;

    if (0x10000U == i++) {
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        gd_eval_led_toggle(LED3);
        i = 0;
    }
}

/*!
    \brief      deinit user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
    usbh_usr_application_state = USBH_USR_FS_INIT;
}

