/*!
    \file  main.c
    \brief USB CDC ACM device
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "cdc_acm_core.h"

extern uint8_t packet_sent, packet_receive;
extern uint32_t receive_length;
extern uint8_t usb_data_buffer[CDC_ACM_DATA_PACKET_SIZE];

usbd_core_handle_struct  usb_device_dev = 
{
    .dev_desc = (uint8_t *)&device_descriptor,
    .config_desc = (uint8_t *)&configuration_descriptor,
    .strings = usbd_strings,
    .class_init = cdc_acm_init,
    .class_deinit = cdc_acm_deinit,
    .class_req_handler = cdc_acm_req_handler,
    .class_data_handler = cdc_acm_data_handler
};

void rcu_config(void);
void gpio_config(void);
void nvic_config(void);

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

    while (1)
    {
        if (USBD_CONFIGURED == usb_device_dev.status) {
            if (1 == packet_receive && 1 == packet_sent) {
                packet_sent = 0;
                /* receive datas from the host when the last packet datas have sent to the host */
                cdc_acm_data_receive(&usb_device_dev);
            } else {
                if (0 != receive_length) {
                    /* send receive datas */
                    cdc_acm_data_send(&usb_device_dev, receive_length);
                    receive_length = 0;
                }
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

    /* configure USB model clock from PLL clock */
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2_5);

    /* enable USB APB1 clock */
    rcu_periph_clock_enable(RCU_USBD);
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
}
