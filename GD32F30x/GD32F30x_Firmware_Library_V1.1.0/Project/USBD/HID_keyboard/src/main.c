/*!
    \file  main.c
    \brief construct a USB keyboard
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "usbd_std.h"
#include "hid_core.h"

#define USB_PULLUP                      GPIOG
#define USB_PULLUP_PIN                  GPIO_PIN_8
#define RCC_AHBPeriph_GPIO_PULLUP       RCU_GPIOG

typedef enum
{
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
}Key_Char;

__IO uint8_t prev_transfer_complete = 1;

uint8_t key_buffer[HID_IN_PACKET] = {0};

usbd_core_handle_struct  usb_device_dev = 
{
    .dev_desc = (uint8_t *)&device_descripter,
    .config_desc = (uint8_t *)&configuration_descriptor,
    .strings = usbd_strings,
    .class_init = hid_init,
    .class_deinit = hid_deinit,
    .class_req_handler = hid_req_handler,
    .class_data_handler = hid_data_handler
};

void rcu_config(void);
void key_config(void);
void gpio_config(void);
void nvic_config(void);
uint8_t key_state(void);

#ifdef USE_IRC48M
    void ctc_config(void);
#endif

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();

    /* keys configuration */
    key_config();

    /* GPIO configuration */
    gpio_config();

    /* USB device configuration */
    usbd_core_init(&usb_device_dev);

    /* NVIC configuration */
    nvic_config();

    /* enabled USB pull-up */
    gpio_bit_set(USB_PULLUP, USB_PULLUP_PIN);

    /* now the usb device is connected */
    usb_device_dev.status = USBD_CONNECTED;

#ifdef USE_IRC48M
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC config */
    ctc_config();

    while (ctc_flag_get(CTC_FLAG_CKOK) == RESET) {
    }
#endif

    while(usb_device_dev.status != USBD_CONFIGURED);

    while (1) {
        if (prev_transfer_complete) {
            switch (key_state()) {
            case CHAR_A:
                key_buffer[2] = 0x04;
                break;
            case CHAR_B:
                key_buffer[2] = 0x05;
                break;
            case CHAR_C:
                key_buffer[2] = 0x06;
                break;
            default:
                break;
            }

            if (0 != key_buffer[2]) {
                hid_report_send(&usb_device_dev, key_buffer, HID_IN_PACKET);
            }
        }
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable USB pull-up pin clock */ 
    rcu_periph_clock_enable(RCC_AHBPeriph_GPIO_PULLUP);

    /* enable the power clock */
    rcu_periph_clock_enable(RCU_PMU);

#ifndef USE_IRC48M
    rcu_ck48m_clock_config(RCU_CK48MSRC_CKPLL);

    /* configure USB model clock from PLL clock */
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2_5);
#else
    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);

    /* wait till IRC48M is ready */
    while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
    }

    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
#endif

    /* enable USB APB1 clock */
    rcu_periph_clock_enable(RCU_USBD);
}

/*!
    \brief      configure the keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    /* keys configuration */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_USER1, KEY_MODE_GPIO);

    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
}

/*!
    \brief      configure the gpio peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure usb pull-up pin */
    gpio_init(USB_PULLUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, USB_PULLUP_PIN);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* 1 bit for pre-emption priority, 3 bits for subpriority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    /* enable the USB low priority interrupt */
    nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn, 1, 0);

    /* enable the USB Wake-up interrupt */
    nvic_irq_enable(USBD_WKUP_IRQn, 0, 0);
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

/*!
    \brief      to get usb keyboard state
    \param[in]  none
    \param[out] none
    \retval     the char
*/
uint8_t  key_state (void)
{
    /* have pressed tamper key */
    if (!gd_eval_key_state_get(KEY_TAMPER)) {
        return CHAR_A;
    }

    /* have pressed wakeup key */
    if (!gd_eval_key_state_get(KEY_WAKEUP)) {
        return CHAR_B;
    }

    /* have pressed user key */
    if (!gd_eval_key_state_get(KEY_USER1)) {
        return CHAR_C;
    }

    /* no pressed any key */
    return 0;
}
