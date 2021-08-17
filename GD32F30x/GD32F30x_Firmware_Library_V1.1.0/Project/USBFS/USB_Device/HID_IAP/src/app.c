/*!
    \file  app.c
    \brief USB main routine for HID device(USB keyboard)
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "iap_core.h"
#include "usb_delay.h"

usb_core_handle_struct usbhs_core_dev = 
{
    .dev = 
    {
        .class_init = iap_init,
        .class_deinit = iap_deinit,
        .class_req_handler = iap_req_handler,
        .class_data_handler = iap_data_handler,
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

pAppFunction application;
uint32_t app_address;

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

extern uint8_t usbd_serial_string[];

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);
void serial_string_create(void);


/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure Tamper key to run firmware */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
  
    timer_nvic_init();

    /* tamper key must be pressed on GD32150R-EVAL when power on */
    if (gd_eval_key_state_get(KEY_TAMPER) != 0) {
        /* test if user code is programmed starting from address 0x8004000 */
        if (((*(__IO uint32_t*)APP_LOADED_ADDR) & 0x2FFE0000) == 0x20000000) {
            app_address = *(__IO uint32_t*) (APP_LOADED_ADDR + 4);
            application = (pAppFunction) app_address;

            /* initialize user application's stack pointer */
            __set_MSP(*(__IO uint32_t*) APP_LOADED_ADDR);

            /* jump to user application */
            application();
        }
    }

    /* configure USB clock */
    usb_clock_config();

    serial_string_create();

    usbhs_core_dev.dev.dev_desc = (uint8_t *)&device_descripter;
    usbhs_core_dev.dev.config_desc = (uint8_t *)&configuration_descriptor;
    usbhs_core_dev.dev.strings = usbd_strings;

    /* USB device stack configure */
    usbd_init(&usbhs_core_dev, 
#ifdef USE_USBFS
    USB_FS_CORE_ID
#elif defined(USE_USBHS)
    USB_HS_CORE_ID
#endif
    );

    /* USB interrupt configure */
    usb_interrupt_config();

    /* check if USB device is enumerated successfully */
    while (usbhs_core_dev.dev.status != USB_STATUS_CONFIGURED) {
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

    if (0 != device_serial) {
        usbd_serial_string[2] = (uint8_t)device_serial;
        usbd_serial_string[3] = (uint8_t)(device_serial >> 8);
        usbd_serial_string[4] = (uint8_t)(device_serial >> 16);
        usbd_serial_string[5] = (uint8_t)(device_serial >> 24);
    }
}
