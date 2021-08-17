/*!
    \file  main.c 
    \brief this file realizes the MSC host
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "usb_delay.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "usbh_std.h"
#include "usbh_ctrl.h"
#include "GD32F403_eval.h"

#define HOST_POWERSW_PORT_RCC     RCU_GPIOD
#define HOST_POWERSW_PORT         GPIOD
#define HOST_POWERSW_VBUS         GPIO_PIN_13

void usb_rcu_init                 (void);
void usb_gpio_init                (void);
void usb_hwp_interrupt_enable     (usb_core_handle_struct *pudev);
void usb_hwp_vbus_config          (usb_core_handle_struct *pudev);
void usb_hwp_vbus_drive           (void *pudev, uint8_t state);
void system_clock_config          (void);

uint8_t timer_prescaler = 0;
uint32_t usbfs_prescaler = 0;

usb_core_handle_struct usb_core_dev =
{
    .host = 
    {
        .vbus_drive = usb_hwp_vbus_drive,
    },
    .udelay = delay_us,
    .mdelay = delay_ms,
};

usbh_host_struct usb_host = 
{
    .usbh_backup_state.host_backup_state      = HOST_IDLE,
    .usbh_backup_state.enum_backup_state      = ENUM_IDLE,
    .usbh_backup_state.ctrl_backup_state      = CTRL_IDLE,
    .usbh_backup_state.class_req_backup_state = 0,
    .usbh_backup_state.class_backup_state     = 0,

    .usr_cb         = &usr_cb,
    .class_init     = usbh_msc_interface_init,
    .class_deinit   = usbh_msc_interface_deinit,

};

usbh_state_handle_struct usbh_state_core;

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init COM4 */
    gd_eval_com_init(EVAL_COM4);

    /* usb rcu init */
    usb_rcu_init();

    /* timer nvic initialization */
    timer_nvic_init();

    /* configure GPIO pin used for switching VBUS power */
    usb_hwp_vbus_config(&usb_core_dev);

    /* host de-initializations */
    usbh_deinit(&usb_core_dev, &usb_host, &usbh_state_core);

    /* start the USB core */
    hcd_init(&usb_core_dev, USB_FS_CORE_ID);

    /* init usr call back */
    usb_host.usr_cb->init();

    /* enable interrupts */
    usb_hwp_interrupt_enable(&usb_core_dev);
  
    while (1) {
        host_state_polling_fun(&usb_core_dev, &usb_host, &usbh_state_core);
    }
}

/*!
    \brief      initialize usb rcu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_init (void)
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
    \brief      configure usb global interrupt
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void usb_hwp_interrupt_enable(usb_core_handle_struct *pudev)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);
}

/*!
    \brief      drives the Vbus signal through GPIO
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void usb_hwp_vbus_drive(void *pudev, uint8_t state)
{
    if (0U == state) {
        /* disable is needed on output of the power switch */
        gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    } else {
        /* enable the power switch by driving the enable low */
        gpio_bit_reset(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    }
}

/*!
    \brief      configures the gpio for the vbus and overcurrent
    \param[in]  pudev: pointer to usb device
    \param[out] none
    \retval     none
*/
void  usb_hwp_vbus_config(usb_core_handle_struct *pudev)
{
    rcu_periph_clock_enable(HOST_POWERSW_PORT_RCC);

    gpio_init(HOST_POWERSW_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, HOST_POWERSW_VBUS);
  
    /* by default, disable is needed on output of the power switch */
    gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);

    /* delay is need for stabilising the vbus low in reset condition, 
     * when vbus = 1 and reset-button is pressed by user 
     */
    if (NULL != pudev->mdelay) {
        pudev->mdelay(200U);
    }
}
