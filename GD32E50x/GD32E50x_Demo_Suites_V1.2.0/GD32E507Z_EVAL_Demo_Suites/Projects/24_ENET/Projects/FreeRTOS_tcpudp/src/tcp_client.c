/*!
    \file    tcp_client.c
    \brief   TCP client demo program 

    \version 2020-06-30, V1.0.0, demo for GD32E50x
    \version 2020-08-26, V1.1.0, demo for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
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

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "tcp_client.h"
#include <string.h>
#include <stdio.h>
#include "gd32e50x.h"
#include "main.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/api.h"

#define TCP_CLIENT_TASK_PRIO            ( tskIDLE_PRIORITY + 5)
#define MAX_BUF_SIZE                    50
#define TIME_WAITING_FOR_CONNECT        ( ( portTickType ) 500 )

#if ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1))

struct recev_packet 
{
    int length;
    char bytes[MAX_BUF_SIZE];
};

static err_t tcp_client_recv(struct netconn *conn, struct netbuf *buf);

/*!
    \brief      called when a data is received 
    \param[in]  conn: the TCP netconn over which to send data
    \param[in]  buf: the received data
    \param[out] none
    \retval     err_t: error value
*/ 
static err_t tcp_client_recv(struct netconn *conn, struct netbuf *buf)
{
    struct pbuf *q;
    struct recev_packet recev_packet;
    uint32_t data_len = 0;


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
        netconn_write(conn, (void *)&(recev_packet.bytes), recev_packet.length, NETCONN_COPY);
    }
  
    return ERR_OK;
}

/*!
    \brief      tcp_client task 
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void tcp_client_task(void *arg)
{
    struct netconn *conn;
    struct ip_addr ipaddr;
    struct netbuf *buf;
    err_t ret, recv_err;

    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);
  
    while(1){
        /* creat TCP connection */
        conn = netconn_new(NETCONN_TCP);
        /* bind the new netconn to any IP address and port 10260 */
        recv_err = netconn_bind(conn, IP_ADDR_ANY, 10260);
      
        if((ERR_USE != recv_err)&&(ERR_ISCONN != recv_err)){
          
            /* connect the new netconn to remote server and port 10260 */
            ret = netconn_connect(conn, &ipaddr, 10260);
            if(ERR_OK == ret) {
              
                recv_err = netconn_recv(conn, &buf);
                while(ERR_OK == recv_err){ 

                    /* handle the received data */
                    tcp_client_recv(conn, buf);
                    netbuf_delete(buf);

                    recv_err = netconn_recv(conn, &buf);
                }
            }              
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
    \brief      tcp_client task 
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void tcp_client_task(void *arg)
{
    int  ret, recvnum, sockfd = -1;
    int tcp_port = 10260;
    struct sockaddr_in svr_addr, clt_addr;        
    char buf[100];
    ip_addr_t ipaddr;

    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);

    /* set up address to connect to */
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(tcp_port);
    svr_addr.sin_addr.s_addr = ipaddr.addr;
  
    clt_addr.sin_family = AF_INET;
    clt_addr.sin_port = htons(tcp_port);
    clt_addr.sin_addr.s_addr = htons(INADDR_ANY);
  
    while(1){
        vTaskDelay(1000);
        /* create the socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
               continue;
        }
        
        ret = bind(sockfd, (struct sockaddr *)&clt_addr, sizeof(clt_addr));
        if(ret < 0){ 
            lwip_close(sockfd);
            sockfd = -1;
            continue;
        }
        
        /* connect */
        ret = connect(sockfd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
        if(ret < 0){ 
            lwip_close(sockfd);
            sockfd = -1;
            continue;
        }

        while(-1 != sockfd){
            /* reveive packets, and limit a reception to MAX_BUF_SIZE bytes */
            recvnum = recv(sockfd, buf, MAX_BUF_SIZE, 0);
            if(recvnum <= 0){
                lwip_close(sockfd);
                sockfd = -1;
                break;
            }
            send(sockfd, buf, recvnum, 0);
        }

        lwip_close(sockfd);
        sockfd = -1;
    } 
    
}

#endif /* LWIP_SOCKET */


/*!
    \brief      initialize the tcp_client application
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tcp_client_init(void)
{
    xTaskCreate(tcp_client_task, "TCP_CLIENT", DEFAULT_THREAD_STACKSIZE, NULL, TCP_CLIENT_TASK_PRIO, NULL);   
}
