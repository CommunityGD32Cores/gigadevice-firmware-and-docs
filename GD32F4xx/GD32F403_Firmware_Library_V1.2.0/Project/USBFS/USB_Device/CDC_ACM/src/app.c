/*!
    \file  app.c
    \brief USB main routine for CDC device
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "cdc_acm_core.h"
#include "usb_delay.h"

extern uint8_t packet_sent, packet_receive;
extern uint32_t receive_length;
extern uint8_t usb_data_buffer[CDC_ACM_DATA_PACKET_SIZE];

usb_core_handle_struct usbfs_core_dev =
{
    .dev = 
    {
        .dev_desc = (uint8_t *)&device_descriptor,
        .config_desc = (uint8_t *)&configuration_descriptor,
        .strings = usbd_strings,
        .class_init = cdc_acm_init,
        .class_deinit = cdc_acm_deinit,
        .class_req_handler = cdc_acm_req_handler,
        .class_data_handler = cdc_acm_data_handler
    },

    .udelay = delay_us,
    .mdelay = delay_ms
};

void usb_clock_config(void);
void usb_interrupt_config(void);

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

/*!
    \brief      main routine will construct a USB mass storage device
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
    usbd_init(&usbfs_core_dev, USB_FS_CORE_ID);

    /* USB interrupt configure */
    usb_interrupt_config();

    /* check if USB device is enumerated successfully */
    while (usbfs_core_dev.dev.status != USB_STATUS_CONFIGURED) {}
      
    /* delay 10 ms */
    if(NULL != usbfs_core_dev.mdelay){
        usbfs_core_dev.mdelay(10);
    }

    while (1) {
        if (USB_STATUS_CONFIGURED == usbfs_core_dev.dev.status) {
            if ((1 == packet_receive) && (1 == packet_sent)) {
                packet_sent = 0;
                /* receive datas from the host when the last packet datas have sent to the host */
                cdc_acm_data_receive(&usbfs_core_dev);
            } else {
              if (0 != receive_length) {
                    /* send receive datas */
                    cdc_acm_data_send(&usbfs_core_dev, receive_length);
                    receive_length = 0;
                }
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
