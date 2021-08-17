/*!
    \file  main.c
    \brief construct a USB keyboard

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "usbd_std.h"
#include "hid_core.h"

#define USB_PULLUP                      GPIOA
#define USB_PULLUP_PIN                  GPIO_PIN_8
#define RCC_AHBPeriph_GPIO_PULLUP       RCU_GPIOA

__IO uint8_t prev_transfer_complete = 1;

uint8_t key_buffer[HID_IN_PACKET] = {0};

usbd_core_handle_struct usb_device_dev =
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

    while(usb_device_dev.status != USBD_CONFIGURED);

    while(1){
        if(!gpio_input_bit_get(GPIOA, GPIO_PIN_0)){
            key_buffer[2] = 0x04;

            if(0 != key_buffer[2]){
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

    /* configure USB model clock from PLL clock */
    rcu_usbd_clock_config(RCU_USBD_CKPLL_DIV1_5);

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
    /* enable the key clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure button pin as input */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_0);
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
    gpio_mode_set(USB_PULLUP, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, USB_PULLUP_PIN);
    gpio_output_options_set(USB_PULLUP, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USB_PULLUP_PIN);

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
    nvic_irq_enable(USBD_LP_IRQn, 1, 0);

    /* enable the USB Wake-up interrupt */
    nvic_irq_enable(USBDWakeUp_IRQChannel, 0, 0);
}
