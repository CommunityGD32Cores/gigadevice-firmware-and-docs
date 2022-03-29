/*!
    \file    main.c
    \brief   construct a USB keyboard

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

#include "standard_hid_core.h"
#include "usbd_hw.h"

usb_dev usb_hid;

extern hid_fop_handler fop_handler;

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    
    /* system clocks configuration */
    rcu_config();

    /* GPIO configuration */
    gpio_config();

    hid_itfop_register (&usb_hid, &fop_handler);
    
    /* USB device configuration */
    usbd_init(&usb_hid, &hid_desc, &hid_class);

    /* NVIC configuration */
    nvic_config();
    
    usbd_connect(&usb_hid);

#ifdef USE_IRC48M
    /* LXTAL clock config */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    /* enable LXTAL clock */
    RCU_BDCTL |= RCU_BDCTL_LXTALEN;
    /* wait till LXTAL is ready */
    while(0 == (RCU_BDCTL & RCU_BDCTL_LXTALSTB)){
    }
    
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC configure */
    ctc_config();

    while(RESET == ctc_flag_get(CTC_FLAG_CKOK)){
    }
#endif

    /* wait for standard USB enumeration is finished */
    while(usb_hid.cur_status != USBD_CONFIGURED){
    }
    
    nvic_irq_enable(KEY_A_EXTI_IRQn, 0U, 0U);

    while (1) {
        fop_handler.hid_itf_data_process(&usb_hid);
    }
}

