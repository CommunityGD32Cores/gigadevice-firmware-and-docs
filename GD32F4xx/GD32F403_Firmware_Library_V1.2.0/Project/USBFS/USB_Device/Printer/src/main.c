/*!
    \file  main.c
    \brief main routine will construct a Printer device
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "usb_delay.h"
#include "printer_core.h"

usb_core_handle_struct usb_core_dev = 
{
    .dev = 
    {
        .dev_desc = (uint8_t *)&device_descripter,
        .config_desc = (uint8_t *)&configuration_descriptor,
        .strings = usbd_strings,
        .class_init = printer_init,
        .class_deinit = printer_deinit,
        .class_req_handler = printer_req_handler,
        .class_data_handler = printer_data_handler
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure USB clock */
    usb_clock_config();

    /* timer nvic initialization */
    timer_nvic_init();

    /* USB device stack configure */
    usbd_init(&usb_core_dev, USB_FS_CORE_ID);

    /* USB interrupt configure */
    usb_interrupt_config();

    /* check if USB device is enumerated successfully */
    while (usb_core_dev.dev.status != USB_STATUS_CONFIGURED) {}

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
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);
}
