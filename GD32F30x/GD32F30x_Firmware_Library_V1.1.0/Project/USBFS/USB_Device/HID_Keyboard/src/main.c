/*!
    \file  main.c
    \brief USB main routine for HID device(USB keyboard)
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include  "hid_core.h"
#include  "usb_delay.h"

typedef enum
{
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
}key_char_enum;

uint8_t key_buffer[HID_IN_PACKET] = {0};

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
        .class_init = usbd_hid_init,
        .class_deinit = usbd_hid_deinit,
        .class_req_handler = usbd_hid_classreq_handle,
        .class_data_handler = usbd_hid_data_handler,
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

void usb_clock_config(void);
void usb_gpio_config(void);
void usb_interrupt_config(void);

void key_config(void);
uint8_t key_state(void);

#ifdef USE_IRC48M
    void ctc_config(void);
#endif

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

    /* timer nvic initialization */
    timer_nvic_init();

    /* configure key */
    key_config();

    /* USB device stack configure */
    usbd_init(&usbhs_core_dev, USB_FS_CORE_ID);

    /* USB interrupt configure */
    usb_interrupt_config();

#ifdef USE_IRC48M
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC config */
    ctc_config();

    while (ctc_flag_get(CTC_FLAG_CKOK) == RESET) {
    }
#endif

    /* check if USB device is enumerated successfully */
    while (usbhs_core_dev.dev.status != USB_STATUS_CONFIGURED) {
    }

    while (1) {
        if (prev_transfer_complete) {
            switch (key_state()) {
            case CHAR_A:
                key_buffer[2] = 0x04U;
                break;
            case CHAR_B:
                key_buffer[2] = 0x05U;
                break;
            case CHAR_C:
                key_buffer[2] = 0x06U;
                break;
            default:
                break;
            }

            if (key_buffer[2] != 0U) {
                usbd_hid_report_send (&usbhs_core_dev, key_buffer, 8U);
            }
        }
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

#ifndef USE_IRC48M
    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);

    rcu_usbfs_clock_config(usbfs_prescaler);
#else
    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);

    /* wait till IRC48M is ready */
    while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
    }

    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
#endif /* USE_IRC48M */

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
    nvic_irq_enable((uint8_t)USBFS_IRQn, 3U, 0U);

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 0U, 0U);

}

/*!
    \brief      configure key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    /* configure the wakeup key in exti mode to remote wakeup */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);

    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);

    exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
}

/*!
    \brief      to get usb keyboard state
    \param[in]  none
    \param[out] none
    \retval     the char
*/
uint8_t key_state (void)
{
    /* have pressed tamper key */
    if (!gd_eval_key_state_get(KEY_TAMPER)) {
        delay_ms(100U);

        if (!gd_eval_key_state_get(KEY_TAMPER)) {
            return CHAR_A;
        }
    }

    /* have pressed wakeup key */
    if (!gd_eval_key_state_get(KEY_WAKEUP)) {
        delay_ms(100U);

        if (!gd_eval_key_state_get(KEY_WAKEUP)) {
            return CHAR_B;
        }
    }

    /* have pressed user key */
    if (!gd_eval_key_state_get(KEY_USER)) {
        delay_ms(100U);

        if (!gd_eval_key_state_get(KEY_USER)) {
            return CHAR_C;
        }
    }

    /* no pressed any key */
    return 0U;
}

#ifdef USE_IRC48M

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* config CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_USBSOF);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* config hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);
    
    /* config CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0xBB7F);
    /* config clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x1D);

    /* CTC counter enable */
    ctc_counter_enable();
}

#endif
