/*!
    \file  app.c
    \brief USB main routine for DFU device
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "dfu_core.h"
#include "usb_delay.h"

extern uint8_t usbd_serial_string[];

pAppFunction Application;
uint32_t AppAddress;

usb_core_handle_struct usbfs_core_dev = 
{
    .dev = 
    {
        .class_init = dfu_init,
        .class_deinit = dfu_deinit,
        .class_req_handler = dfu_req_handler,
        .class_data_handler = dfu_data_handler,
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);
void serial_string_create(void);

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure tamper key to run firmware */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    timer_nvic_init();

    /* tamper key must be pressed on GD32F403Z-EVAL when power on */
    if (0 == gd_eval_key_state_get(KEY_TAMPER)) {
        /* test if user code is programmed starting from address 0x08004000 */
        if (0x20000000 == ((*(__IO uint32_t*)APP_LOADED_ADDR) & 0x2FFE0000)) {
            AppAddress = *(__IO uint32_t*) (APP_LOADED_ADDR + 4);
            Application = (pAppFunction) AppAddress;

            /* initialize user application's stack pointer */
            __set_MSP(*(__IO uint32_t*) APP_LOADED_ADDR);

            /* jump to user application */
            Application();
        }
    }

    /* configure USB clock */
    usb_clock_config();

    serial_string_create();

    usbfs_core_dev.dev.dev_desc = (uint8_t *)&device_descripter;
    usbfs_core_dev.dev.config_desc = (uint8_t *)&configuration_descriptor;
    usbfs_core_dev.dev.strings = usbd_strings;

    /* USB device stack configure */
    usbd_init(&usbfs_core_dev, USB_FS_CORE_ID);

    /* USB interrupt configure */
    usb_interrupt_config();

    /* check if USB device is enumerated successfully */
    while (usbfs_core_dev.dev.status != USB_STATUS_CONFIGURED) {
    }

    while (1) {
    }
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_clock_config(void)
{
    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);
  
    if (system_clock == 48000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1;
        timer_prescaler = 3;
    } else if (system_clock == 72000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
        timer_prescaler = 5;
    } else if (system_clock == 120000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2_5;
        timer_prescaler = 9;
    } else if (system_clock == 144000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV3;
        timer_prescaler = 11;
    } else if (system_clock == 168000000) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV3_5;
        timer_prescaler = 13;
    } else {
        /*  reserved  */
    }

    rcu_usb_clock_config(usbfs_prescaler);
    rcu_periph_clock_enable(RCU_USBFS);
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_interrupt_config(void)
{
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);
}

/*!
    \brief      create the serial number string descriptor
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  serial_string_create (void)
{
    uint32_t device_serial = *(uint32_t *)DEVICE_ID;

    if(0 != device_serial) {
        usbd_serial_string[2] = (uint8_t)device_serial;
        usbd_serial_string[3] = (uint8_t)(device_serial >> 8);
        usbd_serial_string[4] = (uint8_t)(device_serial >> 16);
        usbd_serial_string[5] = (uint8_t)(device_serial >> 24);
    }
}
