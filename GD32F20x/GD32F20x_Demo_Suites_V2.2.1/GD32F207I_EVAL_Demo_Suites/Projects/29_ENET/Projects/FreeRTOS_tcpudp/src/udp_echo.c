/*!
    \file  udp_echo.c
    \brief UDP demo program 

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

#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "lwip/sys.h"
#include "udp_echo.h"  
#include <string.h>
#include <stdio.h>
#include "gd32f20x.h"
#include "lwip/memp.h"
#include "main.h"
#include "lwip/api.h"

#define UDP_TASK_PRIO       ( tskIDLE_PRIORITY + 5)
#define MAX_BUF_SIZE        50


#if ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1))

struct recev_packet 
{
    int length;
    char bytes[MAX_BUF_SIZE];
};

static err_t udp_echo_recv(struct netconn *conn, struct netbuf *buf, struct ip_addr *addr, u16_t port);

/*!
    \brief      called when a data is received 
    \param[in]  conn: the UDP netconn over which to send data
    \param[in]  buf: the received data
    \param[in]  addr: the IP address to send packet to
    \param[in]  port: the port number to send packet to
    \param[out] none
    \retval     err_t: error value
*/ 
static err_t udp_echo_recv(struct netconn *conn, struct netbuf *buf, struct ip_addr *addr, u16_t port)
{
    struct pbuf *q;
    struct recev_packet recev_packet;
    uint32_t data_len = 0;
    struct netbuf *udpbuf;

    /* we perform here any necessary processing on the buf */
    if(NULL != buf){    
        
        for(q = buf->p; q != NULL; q = q->next){
            /* if the received data size is larger than the size we want to get */     
            if(q->len > (MAX_BUF_SIZE - data_len)){
                /* only copy MAX_BUF_SIZE bytes data */
                memcpy((recev_packet.bytes + data_len), q->payload, (MAX_BUF_SIZE - data_len));
                data_len = MAX_BUF_SIZE;
            }else{
                /* copy q->len bytes data */
                memcpy((recev_packet.bytes + data_len), q->payload, q->len);
                data_len += q->len;
            }
                       
            if(data_len >= MAX_BUF_SIZE){ 
                break;
            }  
        }
        
        recev_packet.length = data_len;

        udpbuf = netbuf_new();
        netbuf_ref(udpbuf, recev_packet.bytes, recev_packet.length);
        netconn_sendto(conn, udpbuf, addr, port);

        netbuf_delete(udpbuf);
    }
  
    return ERR_OK;
}

/*!
    \brief      udp echo task 
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void udp_task(void *arg)
{
    struct netconn *conn;
    struct ip_addr ipaddr;
    struct netbuf *buf;
    err_t recv_err;

    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);
  
    /* creat UDP connection */
    conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP_ADDR_ANY, 1025);
  
    while(1){
        recv_err = netconn_recv(conn, &buf);
        while (ERR_OK == recv_err){ 

            udp_echo_recv(conn, buf, &ipaddr, 1025);
            netbuf_delete(buf);

            recv_err = netconn_recv(conn, &buf);
        }
    
        /* close connection and discard connection identifier */
        netconn_close(conn);
        netconn_delete(conn);
    }
}


#endif /* ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1)) */



#if LWIP_SOCKET
#include "lwip/sockets.h"

/*!
    \brief      udp echo task 
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void udp_task(void *arg)
{
    int  ret, recvnum, sockfd = -1;
    int udp_port = 1025;
    struct sockaddr_in rmt_addr, bod_addr;        
    char buf[100];
    u32_t len;
    ip_addr_t ipaddr;

    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);

    /* set up address to connect to */
    rmt_addr.sin_family = AF_INET;
    rmt_addr.sin_port = htons(udp_port);
    rmt_addr.sin_addr.s_addr = ipaddr.addr;
  
    bod_addr.sin_family = AF_INET;
    bod_addr.sin_port = htons(udp_port);
    bod_addr.sin_addr.s_addr = htons(INADDR_ANY);
    
    while (1){
        /* create the socket */
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd < 0){
            continue;
        }
        
        ret = bind(sockfd, (struct sockaddr *)&bod_addr, sizeof(bod_addr));
        
        if(ret < 0){ 
            lwip_close(sockfd);
            sockfd = -1;
            continue;
        }  
        
        len = sizeof(rmt_addr);
        /* reveive packets from rmt_addr, and limit a reception to MAX_BUF_SIZE bytes */
        recvnum = recvfrom(sockfd, buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&rmt_addr, &len);
        
        while(-1 != sockfd){
            if(recvnum > 0){
                /* send packets to rmt_addr */
                sendto(sockfd, buf, recvnum, 0, (struct sockaddr *)&rmt_addr, sizeof(rmt_addr));
            }
            recvnum = recvfrom(sockfd, buf, MAX_BUF_SIZE, 0, (struct sockaddr *)&rmt_addr, &len);
        }

        lwip_close(sockfd);
    } 
}

#endif /* LWIP_SOCKET */


/*!
    \brief      initialize the udp_echo application
    \param[in]  none
    \param[out] none
    \retval     none
*/
void udp_echo_init(void)
{
    xTaskCreate(udp_task, "UDP", DEFAULT_THREAD_STACKSIZE, NULL, UDP_TASK_PRIO, NULL);   
}
