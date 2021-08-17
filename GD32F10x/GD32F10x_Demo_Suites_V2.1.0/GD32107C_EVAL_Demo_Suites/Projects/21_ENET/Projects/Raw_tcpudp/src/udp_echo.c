/*!
    \file  udp_echo.c
    \brief UDP demo program 

    \version 2014-12-26, V1.0.0, demo for GD32F10x
    \version 2017-06-20, V2.0.0, demo for GD32F10x
    \version 2018-07-31, V2.1.0, demo for GD32F10x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "udp_echo.h"  
#include "lwip/udp.h"
#include <string.h>
#include <stdio.h>
#include "gd32f10x.h"

static void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);


/*!
    \brief      called when a data is received on the udp connection 
    \param[in]  args: the user argument
    \param[in]  pcb: the udp_pcb that has received the data
    \param[in]  p: the packet buffer
    \param[in]  addr: pointer on the receive IP address
    \param[in]  port: receive port number
    \param[out] none
    \retval     none
*/ 
static void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    ip_addr_t destaddr = *addr;

    if(p != NULL){
        udp_sendto(pcb, p, &destaddr, port); 
        /* end of processing, we free the pbuf */
        pbuf_free(p);
    }
}

/*!
    \brief      initialize the udp_client application
    \param[in]  none
    \param[out] none
    \retval     none
*/
void udp_echo_init(void)
{
    struct udp_pcb *udppcb;  
  
    /* create a new UDP control block */
    udppcb = udp_new();
    /* assign to the new pcb a local IP address and a port number */
    udp_bind(udppcb, IP_ADDR_ANY, 1025);
    udp_recv(udppcb,udp_echo_recv,NULL);

}

