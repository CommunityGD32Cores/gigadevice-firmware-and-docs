/*!
    \file    drv_usb_dev.c
    \brief   USB device mode low level driver

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "drv_usb_hw.h"
#include "drv_usb_core.h"
#include "drv_usb_dev.h"

/* endpoint 0 max packet length */
static const uint8_t EP0_MAXLEN[4] = {
    [DSTAT_EM_HS_PHY_30MHZ_60MHZ] = EP0MPL_64,
    [DSTAT_EM_FS_PHY_30MHZ_60MHZ] = EP0MPL_64,
    [DSTAT_EM_FS_PHY_48MHZ] = EP0MPL_64,
    [DSTAT_EM_LS_PHY_6MHZ] = EP0MPL_8
};

/* USB endpoint Tx FIFO size */
uint16_t USBHS_TX_FIFO_SIZE[USBHS_MAX_EP_COUNT] = 
{
    (uint16_t)TX0_FIFO_SIZE,
    (uint16_t)TX1_FIFO_SIZE,
    (uint16_t)TX2_FIFO_SIZE,
    (uint16_t)TX3_FIFO_SIZE,
    (uint16_t)TX4_FIFO_SIZE,
    (uint16_t)TX5_FIFO_SIZE
};

/*!
    \brief      initialize USB core registers for device mode
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     operation status
*/
usb_status usb_devcore_init (usb_core_driver *udev)
{
    uint8_t i;

    /* force to peripheral mode */
    udev->regs.gr->GUSBCS &= ~(GUSBCS_FDM | GUSBCS_FHM);
    udev->regs.gr->GUSBCS |= GUSBCS_FDM;

    /* restart the PHY clock (maybe don't need to...) */
    *udev->regs.PWRCLKCTL = 0U;

    /* configure periodic frame interval to default value */
    udev->regs.dr->DCFG &= ~DCFG_EOPFT;
    udev->regs.dr->DCFG |= FRAME_INTERVAL_80;

    udev->regs.dr->DCFG &= ~DCFG_DS;

    if (udev->bp.phy_itf == USB_EMBEDDED_PHY_HS) {
        udev->regs.dr->DCFG |= USB_SPEED_INP_HIGH;
    } else if (udev->bp.phy_itf == USB_EMBEDDED_PHY_FS) {
        udev->regs.dr->DCFG |= USB_SPEED_INP_FULL;
    } else {
        /* no operation */
    }

    /* Set RX FIFO size */
    usb_set_rxfifo(&udev->regs, (uint16_t)RX_FIFO_SIZE);

    /* Set endpoint 0 to 3's TX FIFO length and RAM address */
    for (i = 0U; i < USBHS_MAX_EP_COUNT; i++) {
        usb_set_txfifo(&udev->regs, i, USBHS_TX_FIFO_SIZE[i]);
    }

    /* make sure all FIFOs are flushed */

    /* flush all TX FIFOs */
    (void)usb_txfifo_flush (&udev->regs, 0x10U);

    /* flush entire RX FIFO */
    (void)usb_rxfifo_flush (&udev->regs);

    /* clear all pending device interrupts */
    udev->regs.dr->DIEPINTEN = 0U;
    udev->regs.dr->DOEPINTEN = 0U;
    udev->regs.dr->DAEPINT = 0xFFFFFFFFU;
    udev->regs.dr->DAEPINTEN = 0U;

    /* configure all IN/OUT endpoints */
    for (i = 0U; i < udev->bp.num_ep; i++) {
        if (udev->regs.er_in[i]->DIEPCTL & DEPCTL_EPEN) {
            udev->regs.er_in[i]->DIEPCTL |= DEPCTL_EPD | DEPCTL_SNAK;
        } else {
            udev->regs.er_in[i]->DIEPCTL = 0U;
        }

        /* set IN endpoint transfer length to 0 */
        udev->regs.er_in[i]->DIEPLEN = 0U;

        /* clear all pending IN endpoint interrupts */
        udev->regs.er_in[i]->DIEPINTF = 0xFFU;

        if (udev->regs.er_out[i]->DOEPCTL & DEPCTL_EPEN) {
            udev->regs.er_out[i]->DOEPCTL |= DEPCTL_EPD | DEPCTL_SNAK;
        } else {
            udev->regs.er_out[i]->DOEPCTL = 0U;
        }

        /* set OUT endpoint transfer length to 0 */
        udev->regs.er_out[i]->DOEPLEN = 0U;

        /* clear all pending OUT endpoint interrupts */
        udev->regs.er_out[i]->DOEPINTF = 0xFFU;
    }

    udev->regs.dr->DIEPINTEN |= DIEPINTEN_EPTXFUDEN;

    (void)usb_devint_enable (udev);

    /* configure LPM function */
    if(1U == udev->bp.lpm_enable){
        udev->dev.pm.lpm_state = LPM_L0;
        udev->regs.gr->GLPMCFG = GLPMCFG_LPMEN | GLPMCFG_ACKLPM | GLPMCFG_BESLEN;
    }

    return USB_OK;
}

/*!
    \brief      enable the USB device mode interrupts
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     operation status
*/
usb_status usb_devint_enable (usb_core_driver *udev)
{
    /* clear any pending USB OTG interrupts */
    udev->regs.gr->GOTGINTF = 0xFFFFFFFFU;

    /* clear any pending interrupts */
    udev->regs.gr->GINTF = 0xBFFFFFFFU;

    /* enable the USB wakeup and suspend interrupts */
    udev->regs.gr->GINTEN = GINTEN_WKUPIE | GINTEN_SPIE;

    /* enable device_mode-related interrupts */
    if ((uint8_t)USB_USE_FIFO == udev->bp.transfer_mode) {
        udev->regs.gr->GINTEN |= GINTEN_RXFNEIE;
    }

    udev->regs.gr->GINTEN |= GINTEN_RSTIE | GINTEN_ENUMFIE | GINTEN_IEPIE |\
                             GINTEN_OEPIE | GINTEN_SOFIE | GINTEN_ISOONCIE | GINTEN_ISOINCIE;

#ifdef VBUS_SENSING_ENABLED
    udev->regs.gr->GINTEN |= GINTEN_SESIE | GINTEN_OTGIE;
#endif /* VBUS_SENSING_ENABLED */

    if(1U == udev->bp.lpm_enable){
        udev->regs.gr->GINTEN |= GINTEN_LPMIE;
    }

    /* enable USB global interrupt */
    udev->regs.gr->GAHBCS |= GAHBCS_GINTEN;

    return USB_OK;
}

/*!
    \brief      active the USB endpoint0 transaction
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB endpoint0 transaction
    \param[out] none
    \retval     operation status
*/
usb_status usb_transc0_active (usb_core_driver *udev, usb_transc *transc)
{
    __IO uint32_t *reg_addr = NULL;

    /* get the endpoint number */
    uint8_t ep_num = transc->ep_addr.num;

    if (ep_num) {
        /* not endpoint 0 */
        return USB_FAIL;
    }

    if (transc->ep_addr.dir) {
        reg_addr = &udev->regs.er_in[0]->DIEPCTL;
    } else {
        reg_addr = &udev->regs.er_out[0]->DOEPCTL;
    }

    /* endpoint 0 is activated after USB clock is enabled */

    *reg_addr &= ~(DEPCTL_MPL | DEPCTL_EPTYPE | DIEPCTL_TXFNUM);

    /* set endpoint 0 maximum packet length */
    *reg_addr |= EP0_MAXLEN[udev->regs.dr->DSTAT & DSTAT_ES];

    /* activate endpoint */
    *reg_addr |= ((uint32_t)transc->ep_type << 18U) | ((uint32_t)ep_num << 22U) | DEPCTL_SD0PID | DEPCTL_EPACT;

    return USB_OK;
}

/*!
    \brief      active the USB transaction
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB transaction
    \param[out] none
    \retval     status
*/
usb_status usb_transc_active (usb_core_driver *udev, usb_transc *transc)
{
    __IO uint32_t *reg_addr = NULL;
    __IO uint32_t epinten = 0U;

    /* get the endpoint number */
    uint8_t ep_num = transc->ep_addr.num;

    /* enable endpoint interrupt number */
    if (transc->ep_addr.dir) {
        reg_addr = &udev->regs.er_in[ep_num]->DIEPCTL;

        epinten = 1U << ep_num;
    } else {
        reg_addr = &udev->regs.er_out[ep_num]->DOEPCTL;

        epinten = 1U << (16U + ep_num);
    }

    /* if the endpoint is not active, need change the endpoint control register */
    if (!(*reg_addr & DEPCTL_EPACT)) {
        *reg_addr &= ~(DEPCTL_MPL | DEPCTL_EPTYPE | DIEPCTL_TXFNUM);

        /* set endpoint maximum packet length */
        if (0U == ep_num) {
            *reg_addr |= EP0_MAXLEN[udev->regs.dr->DSTAT & DSTAT_ES];
        } else {
            *reg_addr |= transc->max_len;
        }

        /* activate endpoint */
        *reg_addr |= ((uint32_t)transc->ep_type << 18U) | ((uint32_t)ep_num << 22U) | DEPCTL_SD0PID | DEPCTL_EPACT;
    }

#ifdef USB_DEDICATED_EP1_ENABLED
    if (ep_num == 1U) {
        udev->regs.dr->DEP1INTEN |= epinten;
    }
    else
#endif
    {
        /* enable the interrupts for this endpoint */
        udev->regs.dr->DAEPINTEN |= epinten;
    }

    return USB_OK;
}

/*!
    \brief      deactivate the USB transaction
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB transaction
    \param[out] none
    \retval     status
*/
usb_status usb_transc_deactivate(usb_core_driver *udev, usb_transc *transc)
{
    uint32_t epinten = 0U;

    uint8_t ep_num = transc->ep_addr.num;

    /* disable endpoint interrupt number */
    if (transc->ep_addr.dir) {
        epinten = 1U << ep_num;

        udev->regs.er_in[ep_num]->DIEPCTL &= ~DEPCTL_EPACT;
    } else {
        epinten = 1U << (ep_num + 16U);

        udev->regs.er_out[ep_num]->DOEPCTL &= ~DEPCTL_EPACT;
    }

#ifdef USB_DEDICATED_EP1_ENABLED
    if (1U == ep_num) {
        udev->regs.dr->DEP1INTEN &= ~epinten;
    }
    else
#endif
    {
        /* disable the interrupts for this endpoint */
        udev->regs.dr->DAEPINTEN &= ~epinten;
    }

    return USB_OK;
}

/*!
    \brief      configure USB transaction to start IN transfer
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB IN transaction
    \param[out] none
    \retval     operation status
*/
usb_status usb_transc_inxfer (usb_core_driver *udev, usb_transc *transc)
{
    usb_status status = USB_OK;
    uint8_t ep_num = transc->ep_addr.num;

    __IO uint32_t epctl = udev->regs.er_in[ep_num]->DIEPCTL;
    __IO uint32_t eplen = udev->regs.er_in[ep_num]->DIEPLEN;

    eplen &= ~(DEPLEN_TLEN | DEPLEN_PCNT);

    /* zero length packet or endpoint 0 */
    if (0U == transc->xfer_len) {
        /* set transfer packet count to 1 */
        eplen |= 1U << 19U;
    } else {
        /* set transfer packet count */
        if (0U == ep_num) {
            transc->xfer_len = USB_MIN(transc->xfer_len, transc->max_len);

            eplen |= 1U << 19U;
        } else {
            eplen |= (((transc->xfer_len - 1U) + transc->max_len) / transc->max_len) << 19U;
        }

        /* set endpoint transfer length */
        eplen |= transc->xfer_len;

        if (transc->ep_type == (uint8_t)USB_EPTYPE_ISOC) {
            eplen |= DIEPLEN_MCNT & (1U << 29U);
        }
    }

    udev->regs.er_in[ep_num]->DIEPLEN = eplen;

    if (transc->ep_type == (uint8_t)USB_EPTYPE_ISOC) {
        if (((udev->regs.dr->DSTAT & DSTAT_FNRSOF) >> 8U) & 0x01U) {
            epctl |= DEPCTL_SEVNFRM;
        } else {
            epctl |= DEPCTL_SODDFRM;
        }
    }

    if ((uint8_t)USB_USE_DMA == udev->bp.transfer_mode) {
        udev->regs.er_in[ep_num]->DIEPDMAADDR = transc->dma_addr;
    }

    /* enable the endpoint and clear the NAK */
    epctl |= DEPCTL_CNAK | DEPCTL_EPEN;

    udev->regs.er_in[ep_num]->DIEPCTL = epctl;

    if ((uint8_t)USB_USE_FIFO == udev->bp.transfer_mode) {
        udev->regs.er_in[ep_num]->DIEPCTL = epctl;

        if (transc->ep_type != (uint8_t)USB_EPTYPE_ISOC) {
            /* enable the TX FIFO empty interrupt for this endpoint */
            if (transc->xfer_len > 0U) {
                udev->regs.dr->DIEPFEINTEN |= 1U << ep_num;
            }
        } else {
            (void)usb_txfifo_write (&udev->regs, transc->xfer_buf, ep_num, (uint16_t)transc->xfer_len);
        }
    }

    return status;
}

/*!
    \brief      configure USB transaction to start OUT transfer
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB OUT transaction
    \param[out] none
    \retval     status
*/
usb_status usb_transc_outxfer (usb_core_driver *udev, usb_transc *transc)
{
    usb_status status = USB_OK;

    uint8_t ep_num = transc->ep_addr.num;

    uint32_t epctl = udev->regs.er_out[ep_num]->DOEPCTL;
    uint32_t eplen = udev->regs.er_out[ep_num]->DOEPLEN;

    eplen &= ~(DEPLEN_TLEN | DEPLEN_PCNT);

    /* zero length packet or endpoint 0 */
    if ((0U == transc->xfer_len) || (0U == ep_num)) {
        /* set the transfer length to max packet size */
        eplen |= transc->max_len;

        /* set the transfer packet count to 1 */
        eplen |= 1U << 19U;
    } else {
        /* configure the transfer size and packet count as follows:
         * pktcnt = N
         * xfersize = N * maxpacket
         */
        uint32_t packet_count = (transc->xfer_len + transc->max_len - 1U) / transc->max_len;

        eplen |= packet_count << 19U;
        eplen |= packet_count * transc->max_len;
    }

    udev->regs.er_out[ep_num]->DOEPLEN = eplen;

    if ((uint8_t)USB_USE_DMA == udev->bp.transfer_mode) {
        udev->regs.er_out[ep_num]->DOEPDMAADDR = transc->dma_addr;
    }

    if (transc->ep_type == (uint8_t)USB_EPTYPE_ISOC) {
        if (transc->frame_num) {
            epctl |= DEPCTL_SD1PID;
        } else {
            epctl |= DEPCTL_SD0PID;
        }
    }

    /* enable the endpoint and clear the NAK */
    epctl |= DEPCTL_EPEN | DEPCTL_CNAK;

    udev->regs.er_out[ep_num]->DOEPCTL = epctl;

    return status;
}

/*!
    \brief      set the USB transaction STALL status
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB transaction
    \param[out] none
    \retval     status
*/
usb_status usb_transc_stall (usb_core_driver *udev, usb_transc *transc)
{
    __IO uint32_t *reg_addr = NULL;

    uint8_t ep_num = transc->ep_addr.num;

    if (transc->ep_addr.dir) {
        reg_addr = &(udev->regs.er_in[ep_num]->DIEPCTL);

        /* set the endpoint disable bit */
        if (*reg_addr & DEPCTL_EPEN) {
            *reg_addr |= DEPCTL_EPD;
        }
    } else {
        /* set the endpoint stall bit */
        reg_addr = &(udev->regs.er_out[ep_num]->DOEPCTL);
    }

    /* set the endpoint stall bit */
    *reg_addr |= DEPCTL_STALL;

    return USB_OK;
}

/*!
    \brief      clear the USB transaction STALL status
    \param[in]  udev: pointer to USB device
    \param[in]  transc: the USB transaction
    \param[out] none
    \retval     operation status
*/
usb_status usb_transc_clrstall(usb_core_driver *udev, usb_transc *transc)
{
    __IO uint32_t *reg_addr = NULL;

    uint8_t ep_num = transc->ep_addr.num;

    if (transc->ep_addr.dir) {
        reg_addr = &(udev->regs.er_in[ep_num]->DIEPCTL);
    } else {
        reg_addr = &(udev->regs.er_out[ep_num]->DOEPCTL);
    }

    /* clear the endpoint stall bits */
    *reg_addr &= ~DEPCTL_STALL;

    /* reset data PID of the periodic endpoints */
    if ((transc->ep_type == (uint8_t)USB_EPTYPE_INTR) || (transc->ep_type == (uint8_t)USB_EPTYPE_BULK)) {
        *reg_addr |= DEPCTL_SD0PID;
    }

    return USB_OK;
}

/*!
    \brief      read device IN endpoint interrupt flag register
    \param[in]  udev: pointer to USB device
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     interrupt value
*/
uint32_t usb_iepintr_read (usb_core_driver *udev, uint8_t ep_num)
{
    uint32_t value = 0U, fifoemptymask, commonintmask;

    commonintmask = udev->regs.dr->DIEPINTEN;
    fifoemptymask = udev->regs.dr->DIEPFEINTEN;

    /* check FIFO empty interrupt enable bit */
    commonintmask |= ((fifoemptymask >> ep_num) & 0x1U) << 7U;

    value = udev->regs.er_in[ep_num]->DIEPINTF & commonintmask;

    return value;
}

/*!
    \brief      configures OUT endpoint 0 to receive SETUP packets
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     none
*/
void usb_ctlep_startout (usb_core_driver *udev)
{
    /* set OUT endpoint 0 receive length to 24 bytes, 1 packet and 3 setup packets */
    udev->regs.er_out[0]->DOEPLEN = DOEP0_TLEN(8U * 3U) | DOEP0_PCNT(1U) | DOEP0_STPCNT(3U);

    if ((uint8_t)USB_USE_DMA == udev->bp.transfer_mode) {
        udev->regs.er_out[0]->DOEPDMAADDR = (uint32_t)&udev->dev.control.req;

        /* endpoint enable */
        udev->regs.er_out[0]->DOEPCTL |= DEPCTL_EPACT | DEPCTL_EPEN;
    }
}

/*!
    \brief      active remote wakeup signaling
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     none
*/
void usb_rwkup_active (usb_core_driver *udev)
{
    if (udev->dev.pm.dev_remote_wakeup)  {
        if (udev->regs.dr->DSTAT & DSTAT_SPST) {
            if (udev->bp.low_power) {
                /* ungate USB core clock */
                *udev->regs.PWRCLKCTL &= ~(PWRCLKCTL_SHCLK | PWRCLKCTL_SUCLK);
            }

            /* active remote wakeup signaling */
            udev->regs.dr->DCTL |= DCTL_RWKUP;

            usb_mdelay(5U);

            udev->regs.dr->DCTL &= ~DCTL_RWKUP;
        }
    }
}

/*!
    \brief      active USB core clock
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     none
*/
void usb_clock_active (usb_core_driver *udev)
{
    if (udev->bp.low_power) {
        if (udev->regs.dr->DSTAT & DSTAT_SPST) {
            /* un-gate USB Core clock */
            *udev->regs.PWRCLKCTL &= ~(PWRCLKCTL_SHCLK | PWRCLKCTL_SUCLK);
        }
    }
}

/*!
    \brief      USB device suspend
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     none
*/
void usb_dev_suspend (usb_core_driver *udev)
{
    __IO uint32_t devstat = udev->regs.dr->DSTAT;

    if ((devstat & DSTAT_SPST) && (udev->bp.low_power)) {
        /* switch-off the USB clocks */
        *udev->regs.PWRCLKCTL |= PWRCLKCTL_SHCLK;

        /* enter DEEP_SLEEP mode with LDO in low power mode */
        pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
    }
}

/*!
    \brief      stop the device and clean up FIFOs
    \param[in]  udev: pointer to USB device
    \param[out] none
    \retval     none
*/
void usb_dev_stop (usb_core_driver *udev)
{
    uint32_t i;

    udev->dev.cur_status = 1U;

    /* clear all interrupt flag and enable bits */
    for (i = 0U; i < udev->bp.num_ep; i++) {
        udev->regs.er_in[i]->DIEPINTF = 0xFFU;
        udev->regs.er_out[i]->DOEPINTF = 0xFFU;
    }

    udev->regs.dr->DIEPINTEN = 0U;
    udev->regs.dr->DOEPINTEN = 0U;
    udev->regs.dr->DAEPINTEN = 0U;
    udev->regs.dr->DAEPINT = 0xFFFFFFFFU;

    /* flush the FIFO */
    (void)usb_rxfifo_flush (&udev->regs);
    (void)usb_txfifo_flush (&udev->regs, 0x10U);
}

/*!
    \brief      usb battery charging detect operation
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     bcd type
*/
bcd_type usbd_bcd_detect (usb_core_driver *udev)
{
    udev->regs.gr->GCCFG |= GCCFG_DCDEN;

    usb_mdelay(1000);

    if (0U == (udev->regs.gr->GCCFG & GCCFG_DCDF)) {
        udev->regs.gr->GCCFG &= ~GCCFG_DCDEN;

        return BCD_ERROR;
    }

    udev->regs.gr->GCCFG &= ~GCCFG_DCDEN;
    usb_mdelay(20);
    udev->regs.gr->GCCFG |= GCCFG_PEMEN;
    usb_mdelay(50);

    if (udev->regs.gr->GCCFG & GCCFG_PDF) {
        udev->regs.gr->GCCFG &= ~GCCFG_PEMEN;
        usb_mdelay(20);
        udev->regs.gr->GCCFG |= GCCFG_SDMEN;
        usb_mdelay(50);

        if (udev->regs.gr->GCCFG & GCCFG_SDF) {
            udev->regs.gr->GCCFG &= ~GCCFG_SDMEN;
            usb_mdelay(20);

            return BCD_DCP;
        } else {
            udev->regs.gr->GCCFG &= ~GCCFG_SDMEN;
            usb_mdelay(20);

            return BCD_CDP;
        }
    } else if (udev->regs.gr->GCCFG & GCCFG_PS2F) {
        udev->regs.gr->GCCFG &= ~GCCFG_PEMEN;
        usb_mdelay(20);

        return BCD_PS2;
    } else {
        udev->regs.gr->GCCFG &= ~GCCFG_PEMEN;
        usb_mdelay(20);

        return BCD_SDP;
    }

    return BCD_NONE;
}
