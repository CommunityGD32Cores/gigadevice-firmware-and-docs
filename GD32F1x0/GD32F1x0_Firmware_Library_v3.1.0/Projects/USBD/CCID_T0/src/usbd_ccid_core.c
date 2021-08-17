/*!
    \file  cdc_acm_core.c
    \brief CDC ACM driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#include "usbd_ccid_core.h"
#include "usbd_int.h"
#include "usbd_ccid_if.h"
#include "smartcard.h"

#define USBD_VID                          0x28E9
#define USBD_PID                          0x3258

static __IO uint32_t usbd_ccid_altset = 0U;
extern uint32_t pre_packet_send;

uint8_t usb_data_buffer[CDC_ACM_DATA_PACKET_SIZE];
uint8_t usb_cmd_buffer[CDC_ACM_CMD_PACKET_SIZE];
static uint8_t USBD_CCID_EP0_Buff[CCID_EP0_BUFF_SIZ];

uint8_t packet_sent = 1U;
uint8_t packet_receive = 1U;
uint32_t receive_length = 0U;

usbd_int_cb_struct *usbd_int_fops = NULL;

typedef struct
{
    uint32_t dwDTERate;   /* data terminal rate */
    uint8_t  bCharFormat; /* stop bits */
    uint8_t  bParityType; /* parity */
    uint8_t  bDataBits;   /* data bits */
}line_coding_struct;

line_coding_struct linecoding =
{
    115200, /* baud rate     */
    0x00,   /* stop bits - 1 */
    0x00,   /* parity - none */
    0x08    /* num of bits 8 */
};


/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_descriptor_device_struct device_descriptor =
{
    .Header = 
     {
         .bLength = USB_DEVICE_DESC_SIZE, 
         .bDescriptorType = USB_DESCTYPE_DEVICE
     },
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = USBD_EP0_MAX_SIZE,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_MFC_STR_IDX,
    .iProduct = USBD_PRODUCT_STR_IDX,
    .iSerialNumber = USBD_SERIAL_STR_IDX,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

/* USB device configuration descriptor */
const usb_descriptor_configuration_set_struct configuration_descriptor = 
{
    .config = 
    {
        .Header = 
         {
            .bLength = sizeof(usb_descriptor_configuration_struct), 
            .bDescriptorType = USB_DESCTYPE_CONFIGURATION
         },
        .wTotalLength = USB_CCID_CONFIG_DESC_SIZE,
        .bNumInterfaces = 0x01,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x04,
        .bmAttributes = 0x80,
        .bMaxPower = 0x32
    },

    .ccid_interface = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_interface_struct), 
             .bDescriptorType = USB_DESCTYPE_INTERFACE 
         },
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x03,
        .bInterfaceClass = 0x0B,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x05
    },
    
		{
		/*******************  CCID class descriptor ********************/
    0x36,   /* bLength: CCID Descriptor size */
    0x21,   /* bDescriptorType: Functional Descriptor type. */
    0x10,   /* bcdCCID(LSB): CCID Class Spec release number (1.10) */
    0x01,   /* bcdCCID(MSB) */
    0x00,   /* bMaxSlotIndex :highest available slot on this device */
    0x03,   /* bVoltageSupport: bit Wise OR for 01h-5.0V 02h-3.0V 04h-1.8V, others for future use */

#if defined(SC_PROTOCOL_T0)
    0x01,0x00,0x00,0x00,    /* dwProtocols: 0001h = Protocol T = 0 */
#elif defined(SC_PROTOCOL_T1)
    0x02,0x00,0x00,0x00,    /* dwProtocols: 0002h = Protocol T = 1 */
#endif

    0x10,0x0E,0x00,0x00,    /* dwDefaultClock: 3.6Mhz = 3600kHz = 0x0E10, 
                               for 4 Mhz the value is (0x00000FA0): 
                               This is used in ETU and waiting time calculations */

    0x10,0x0E,0x00,0x00,    /* dwMaximumClock: Maximum supported ICC clock frequency 
                               in KHz. So, 3.6Mhz = 3600kHz = 0x0E10, 4 Mhz (0x00000FA0) */

    0x00,                   /* bNumClockSupported : no setting from PC 
                               If the value is 00h, the 
                               supported clock frequencies are assumed to be the 
                               default clock frequency defined by dwDefaultClock 
                               and the maximum clock frequency defined by dwMaximumClock */

    0xCD,0x25,0x00,0x00,    /* dwDataRate: Default ICC I/O data rate in bps
                               9677 bps = 0x25CD 
                               for example 10752 bps (0x00002A00) */

    0xCD,0x25,0x00,0x00,    /* dwMaxDataRate: Maximum supported ICC I/O data rate in bps */

    0x00,                   /* bNumDataRatesSupported :
                               The number of data rates that are supported by the CCID
                               If the value is 00h, all data rates between the default 
                               data rate dwDataRate and the maximum data rate 
                               dwMaxDataRate are supported.
                               Don't support GET_CLOCK_FREQUENCIES
                             */

#if defined(SC_PROTOCOL_T0)
    0x00,0x00,0x00,0x00,    /* dwMaxIFSD: 0 (T = 1 only) */
#elif defined(SC_PROTOCOL_T1)
    0xFE,0x00,0x00,0x00,    /* dwMaxIFSD: 254 (T = 1 only) */
#endif

    0x00,0x00,0x00,0x00,    /* dwSynchProtocols */
    0x00,0x00,0x00,0x00,    /* dwMechanical: no special characteristics */

#if defined(SC_PROTOCOL_T0)
    0xF8,0x00,0x01,0x00,    /* dwFeatures: clk, baud rate, voltage: automatic
                               00000008h Automatic ICC voltage selection 
                               00000010h Automatic ICC clock frequency change
                               00000020h Automatic baud rate change according to 
                               active parameters provided by the Host or self 
                               determined
                               00000040h Automatic parameters negotiation made by 
                               the CCID (use of warm or cold resets or PPS according
                               to a manufacturer proprietary algorithm to select the
                               communication parameters with the ICC)
                               Only one of the following values may be present to 
                               select a level of exchange:
                               00010000h TPDU level exchanges with CCID
                               00020000h Short APDU level exchange with CCID
                               00040000h Short and Extended APDU level exchange 
                               If none of those values : character level of exchange
                             */
#elif defined(SC_PROTOCOL_T1)
    0xF8,0x04,0x01,0x00,    /* dwFeatures: clk, baud rate, voltage: automatic
                               00000008h Automatic ICC voltage selection 
                               00000010h Automatic ICC clock frequency change
                               00000020h Automatic baud rate change according to 
                               active parameters provided by the Host or self 
                               determined
                               00000040h Automatic parameters negotiation made by 
                               the CCID (use of warm or cold resets or PPS according
                               to a manufacturer proprietary algorithm to select the
                               communication parameters with the ICC)
                               00000400h Automatic IFSD exchange as first exchange 
                               (T = 1 protocol in use)
                               Only one of the following values may be present to 
                               select a level of exchange:
                               00010000h TPDU level exchanges with CCID
                               00020000h Short APDU level exchange with CCID
                               00040000h Short and Extended APDU level exchange 
                               If none of those values : character level of exchange
                             */
#endif

    0x0F,0x01,0x00,0x00,    /* dwMaxCCIDMessageLength: Maximum block size + header(261 + 10) */

    0x00,                   /* bClassGetResponse: */
    0x00,                   /* bClassEnvelope: */
    0x00,0x00,              /* wLcdLayout: 0000h no LCD. */
    0x00,                   /* bPINSupport: no PIN verif and modif */
    0x01                   /* bMaxCCIDBusySlots: */
		},
		
    .ccid_bulk_in_endpoint = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_endpoint_struct), 
             .bDescriptorType = USB_DESCTYPE_ENDPOINT 
         },
        .bEndpointAddress = CCID_BULK_IN_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = 0x40,
        .bInterval = 0x00
    },
		
		  .ccid_bulk_out_endpoint = 
    {
        .Header = 
         {
             .bLength = sizeof(usb_descriptor_endpoint_struct), 
             .bDescriptorType = USB_DESCTYPE_ENDPOINT 
         },
        .bEndpointAddress = CCID_BULK_OUT_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = 0x40,
        .bInterval = 0x00
    },
		
		   .ccid_intr_endpoint = 
    {
        .Header = 
         {
            .bLength = sizeof(usb_descriptor_endpoint_struct), 
            .bDescriptorType = USB_DESCTYPE_ENDPOINT
         },
        .bEndpointAddress = CCID_INTR_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = 0x08,
        .bInterval = 0x18
    }
};

/* USB language ID Descriptor */
const usb_descriptor_language_id_struct usbd_language_id_desc = 
{
    .Header = 
     {
         .bLength = sizeof(usb_descriptor_language_id_struct), 
         .bDescriptorType = USB_DESCTYPE_STRING
     },
    .wLANGID = ENG_LANGID
};

void *const usbd_strings[] = 
{
    [USBD_LANGID_STR_IDX] = (uint8_t *)&usbd_language_id_desc,
    [USBD_MFC_STR_IDX] = USBD_STRING_DESC("GD32Microelectronics"),
    [USBD_PRODUCT_STR_IDX] = USBD_STRING_DESC("GD32 USB Smart Card Reader"),
    [USBD_SERIAL_STR_IDX] = USBD_STRING_DESC("GD32F1x0-3.0.0-7z8x9yer")
};

/*!
    \brief      initialize the CCID TO device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
usbd_status_enum ccid_init (void *pudev, uint8_t config_index)
{
    /* initialize the CCID In/Out endpoint */
    usbd_ep_init(pudev, ENDP_SNG_BUF, &(configuration_descriptor.ccid_bulk_in_endpoint));
    usbd_ep_init(pudev, ENDP_SNG_BUF, &(configuration_descriptor.ccid_bulk_out_endpoint));

    /* initialize the interrupt endpoint */
    usbd_ep_init(pudev, ENDP_SNG_BUF, &(configuration_descriptor.ccid_intr_endpoint));

    CCID_Init (pudev);

    return USBD_OK;
}

/*!
    \brief      de-initialize the CCID TO device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
usbd_status_enum ccid_deinit (void *pudev, uint8_t config_index)
{
    /* deinitialize the CCID In/Out endpoint */
    usbd_ep_deinit(pudev, CCID_BULK_IN_EP);
    usbd_ep_deinit(pudev, CCID_BULK_OUT_EP);

    /* deinitialize the command Tx endpoint */
    usbd_ep_deinit(pudev, CCID_INTR_EP);
	
    CCID_DeInit (pudev);

    return USBD_OK;
}

/*!
    \brief      handle CCID TO data
    \param[in]  pudev: pointer to USB device instance
    \param[in]  rx_tx: data transfer direction:
      \arg        USBD_TX
      \arg        USBD_RX
    \param[in]  ep_id: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
usbd_status_enum  ccid_data_handler (void *pudev, usbd_dir_enum rx_tx, uint8_t ep_id)
{
    if ((USBD_RX == rx_tx) && ( ((CCID_BULK_OUT_EP & 0x7F) == ep_id) )) {
        CCID_BulkMessage_Out(pudev , ep_id);
        return USBD_OK;
    } else if((USBD_TX == rx_tx) && \
           (((CCID_BULK_IN_EP & 0x7F) == ep_id) || (( CCID_INTR_EP & 0x7F) == ep_id))) {
        CCID_BulkMessage_In(pudev, ep_id);
        return USBD_OK;
    } else {
    }

    return USBD_FAIL;
}


/*!
    \brief      handle the CCID TO class-specific requests
    \param[in]  pudev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
usbd_status_enum ccid_req_handler (void *pudev, usb_device_req_struct *req)
{
    uint16_t len = CCID_DESC_SIZE;
    uint8_t slot_nb;
    uint8_t seq_nb;

    switch (req->bmRequestType & USB_REQ_MASK) {
        case USB_CLASS_REQ:
            switch (req->bRequest) {
            case REQUEST_ABORT :
                if ((req->wLength == 0) && ((req->bmRequestType & 0x80) != 0x80))
                {
                    /* The wValue field contains the slot number (bSlot) in the low byte 
                       and the sequence number (bSeq) in the high byte. */
                    slot_nb = (uint8_t) ((req->wValue) & 0x00ff);
                    seq_nb = (uint8_t) (((req->wValue) & 0xff00) >> 8);

                    if (CCID_CmdAbort(slot_nb, seq_nb) != 0)
                    {
                        /* If error is returned by lower layer: Generally Slot# may not have matched */
                        usbd_enum_error(pudev, req);

                        return USBD_FAIL; 
                    }
                } else {
                    usbd_enum_error(pudev, req);

                    return USBD_FAIL; 
                }
                break;

            case REQUEST_GET_CLOCK_FREQUENCIES :
                if((req->wValue  == 0) && (req->wLength != 0) && ((req->bmRequestType & 0x80) == 0x80)) {
                    if(SC_Request_GetClockFrequencies(USBD_CCID_EP0_Buff, &len) != 0 ) {
                    /* If error is returned by lower layer */
                    usbd_enum_error(pudev, req);

                    return USBD_FAIL; 
                } else {
                    if (len > CCID_EP0_BUFF_SIZ) {
                        len = CCID_EP0_BUFF_SIZ;
                    }

                    usbd_ep_tx (pudev, EP0_IN,
                               USBD_CCID_EP0_Buff,
                               len);

                }
            }
            else
            {
                usbd_enum_error(pudev, req);

                return USBD_FAIL; 
            }
            break;

        case REQUEST_GET_DATA_RATES :
            if((req->wValue  == 0) && (req->wLength != 0) && ((req->bmRequestType & 0x80) == 0x80))
            {
                if(SC_Request_GetDataRates(USBD_CCID_EP0_Buff, &len) != 0)
                {
                    /* If error is returned by lower layer */
                    usbd_enum_error(pudev, req);

                    return USBD_FAIL; 
                }
                else
                {
                    if (len > CCID_EP0_BUFF_SIZ)
                    {
                        len = CCID_EP0_BUFF_SIZ;
                    }

                    usbd_ep_tx (pudev, EP0_IN,
                               USBD_CCID_EP0_Buff,
                               len);
                } 
            }
            else
            {
                usbd_enum_error(pudev, req);

                return USBD_FAIL; 
            }
            break;

        default:
            usbd_enum_error(pudev, req);

            return USBD_FAIL; 
    }

    case USB_STANDARD_REQ:
        /* standard device request */
        switch(req->bRequest) {
        case USBREQ_GET_INTERFACE:     
            break;
        case USBREQ_SET_INTERFACE:
            break;
        case USBREQ_CLEAR_FEATURE:
            usbd_ep_deinit (pudev, (uint8_t)req->wIndex);

            if((((uint8_t)req->wIndex) & 0x80) == 0x80) {
                usbd_ep_init(pudev, ENDP_SNG_BUF, &(configuration_descriptor.ccid_bulk_in_endpoint));
            } else {
                usbd_ep_init(pudev, ENDP_SNG_BUF, &(configuration_descriptor.ccid_bulk_out_endpoint));
            }

            return USBD_OK;
        default:
            break;
        }
    default:
        break;
    }

    return USBD_OK;
}

