/*!
    \file  netconf.c
    \brief network connection configuration 

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2019-04-01, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "main.h"
#include "netconf.h"
#include "lwip/tcpip.h"
#include <stdio.h>


#define MAX_DHCP_TRIES       4

typedef enum 
{ 
    DHCP_START = 0,
    DHCP_WAIT_ADDRESS,
    DHCP_ADDRESS_ASSIGNED,
    DHCP_TIMEOUT
}dhcp_state_enum;

#ifdef USE_DHCP
dhcp_state_enum dhcp_state = DHCP_START;
#endif /* USE_DHCP */

struct netif g_mynetif;

/*!
    \brief      initializes the LwIP stack
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lwip_stack_init(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    /* create tcp_ip stack thread */
    tcpip_init( NULL, NULL );

    /* IP address setting */
#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  
#endif /* USE_DHCP */

    netif_add(&g_mynetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* registers the default network interface */
    netif_set_default(&g_mynetif);

    /* when the netif is fully configured this function must be called */
    netif_set_up(&g_mynetif);
    netif_set_link_up(&g_mynetif);
}

#ifdef USE_DHCP
/*!
    \brief      dhcp_task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dhcp_task(void * pvParameters)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    ip_addr_t ip_address = {0};
    
    struct dhcp *dhcp_client;
    
    dhcp_client = netif_dhcp_data(&g_mynetif);
  
    for(;;){
        switch(dhcp_state){
        case DHCP_START:
            dhcp_start(&g_mynetif);
            /* IP address should be set to 0 every time we want to assign a new DHCP address*/
            ip_address.addr = 0;
            dhcp_state = DHCP_WAIT_ADDRESS;
            break;

        case DHCP_WAIT_ADDRESS:    
            /* read the new IP address */
            ip_address.addr = g_mynetif.ip_addr.addr;

            if(0 != ip_address.addr){ 
                dhcp_state = DHCP_ADDRESS_ASSIGNED;
                /* stop DHCP */
                dhcp_stop(&g_mynetif);
                printf("\r\nDHCP -- eval board ip address: %d.%d.%d.%d \r\n", ip4_addr1_16(&ip_address), \
                        ip4_addr2_16(&ip_address), ip4_addr3_16(&ip_address), ip4_addr4_16(&ip_address));
            }else{
                /* DHCP timeout */
                if(dhcp_client->tries > MAX_DHCP_TRIES){
                    dhcp_state = DHCP_TIMEOUT;
                    /* stop DHCP */
                    dhcp_stop(&g_mynetif);

                    /* static address used */
                    IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
                    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(&g_mynetif, &ipaddr , &netmask, &gw);
                }
            }
            break;

        default: 
            break;
        }
    
        /* wait 250 ms */
        vTaskDelay(250);
    }   
}
#endif /* USE_DHCP */

unsigned int sys_now(void)
{
    extern volatile unsigned int g_localtime;
    return g_localtime;
}
