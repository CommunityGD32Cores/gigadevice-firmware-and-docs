/*!
    \file  app.c
    \brief USB main routine for HID device(USB keyboard)
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include  "custom_hid_core.h"
#include  "usb_delay.h"

__IO uint8_t prev_transfer_complete = 1U;

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

usb_core_handle_struct usbhs_core_dev = 
{
    .dev = 
    {
        .dev_desc = (uint8_t *)&device_descripter,
        .config_desc = (uint8_t *)&configuration_descriptor,
        .strings = usbd_strings,
        .class_init = custom_hid_init,
        .class_deinit = custom_hid_deinit,
        .class_req_handler = custom_hid_req_handler,
        .class_data_handler = custom_hid_data_handler
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);

void key_config(void);
void led_config(void);

/*!
    \brief      main routine will construct a USB keyboard
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure USB clock */
    usb_clock_config();

    /* configure key */
    key_config();

    /* configure led */
    led_config();
  
    /* timer nvic initialization */
    timer_nvic_init();

    /* USB device stack configure */
    usbd_init(&usbhs_core_dev, USB_FS_CORE_ID);

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

#ifdef USBFS_LOW_PWR_MGMT_SUPPORT

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);

#elif defined(USBHS_LOW_PWR_MGMT_SUPPORT)

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_20);
    exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_20);

    nvic_irq_enable((uint8_t)USBHS_WKUP_IRQn, 1U, 0U);

#endif /* USBHS_LOW_PWR_MGMT_SUPPORT */
}

/*!
    \brief      configure key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
}

/*!
    \brief      configure the leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}
