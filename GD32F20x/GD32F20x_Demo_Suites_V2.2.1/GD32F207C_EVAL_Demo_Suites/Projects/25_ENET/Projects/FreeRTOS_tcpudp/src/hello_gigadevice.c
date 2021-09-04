/*!
    \file  hello_gigadevice.c
    \brief TCP server demo program 

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2018-10-31, V2.1.0, demo for GD32F20x
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
#include "hello_gigadevice.h"  
#include <string.h>
#include <stdio.h>
#include "gd32f20x.h"
#include "lwip/api.h"

#define HELLO_TASK_PRIO  ( tskIDLE_PRIORITY + 5 )
#define MAX_NAME_SIZE      32

#define GREETING           "\n\r======= HelloGigaDevice =======\
                            \n\r== GD32 ==\
                            \n\r== Telnet SUCCESS==\
                            \n\rHello. What is your name?\r\n"
#define HELLO              "\n\rGigaDevice¡¾8000¡¿PORT Hello "



#if ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1))

static err_t hello_gigadevice_recv(struct netconn *conn, void *data, u16_t len);
static err_t hello_gigadevice_accept(struct netconn *conn);

/*!
    \brief      called when a data is received on the telnet connection 
    \param[in]  conn: the TCP netconn over which to send data
    \param[in]  data: pointer to the data received
    \param[in]  len: size of the data
    \param[out] none
    \retval     err_t: error value
*/ 
static err_t hello_gigadevice_recv(struct netconn *conn, void *data, u16_t len)
{
    int done;
    char *c;
    int i;
  
    done = 0;
    c = (char *)data;
    /* a telnet communication packet is ended with an enter key */
    for(i=0; i<len && !done; i++){ 
        done = ((c[i] == '\r') || (c[i] == '\n'));
    } 
    
    if(done){ 
        if(c[len-2] != '\r' || c[len-1] != '\n'){
            /* limit the received data length to MAX_NAME_SIZE - 2('\r' and '\n' will be put into the buffer) */
            if((c[len-1] == '\r' || c[len-1] == '\n') && (len+1 <= MAX_NAME_SIZE)){ 
                /* calculate the buffer size to be sent(including '\r' and '\n') */
                len += 1;
            }else if(len+2 <= MAX_NAME_SIZE){ 
                len += 2;
            }else{  
                len = MAX_NAME_SIZE;
            }
            /* save the received data to a new buffer */
            c[len-2] = '\r';
            c[len-1] = '\n';
        }
        netconn_write(conn, (void *)&HELLO, sizeof(HELLO), NETCONN_COPY);
        netconn_write(conn, data, len, NETCONN_COPY);
    }
 
    return ERR_OK;
}

/*!
    \brief      this function when the telnet connection is established 
    \param[in]  conn: the TCP netconn over which to send data
    \param[out] none
    \retval     err_t: error value
*/
static err_t hello_gigadevice_accept(struct netconn *conn)
{     
    u32_t ipaddress;
    u8_t iptxt[50];
    volatile u8_t iptab[4];
    struct tcp_pcb *pcb;
  
    pcb = conn->pcb.tcp;
    ipaddress = pcb->remote_ip.addr;

    /* read its IP address */
    iptab[0] = (u8_t)(ipaddress >> 24);
    iptab[1] = (u8_t)(ipaddress >> 16);
    iptab[2] = (u8_t)(ipaddress >> 8);
    iptab[3] = (u8_t)(ipaddress);

    sprintf((char*)iptxt, "Telnet:%d.%d.%d.%d   ", iptab[3], iptab[2], iptab[1], iptab[0]);

    /* send out the first message */
    netconn_write(conn, (void *)&iptxt, sizeof(iptxt), NETCONN_COPY);
    netconn_write(conn, (void *)&GREETING, sizeof(GREETING), NETCONN_COPY);
 
    return ERR_OK;
}

/*!
    \brief      hello task 
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void hello_task(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err, accept_err;
    struct netbuf *buf;
    void *data;
    u16_t len;
    err_t recv_err;
        
    LWIP_UNUSED_ARG(arg);

    conn = netconn_new(NETCONN_TCP);
    
    if(NULL != conn){
        /* bind connection to well known port number 8000 */
        err = netconn_bind(conn, NULL, 8000);
        
        if(ERR_OK == err){
        
            netconn_listen(conn);
          
            while(1){ 
                 /* grab new connection */
                 accept_err = netconn_accept(conn, &newconn);
            
                /* process the new connection */
                if(ERR_OK == accept_err){ 
                    hello_gigadevice_accept(newconn);
                    
                    recv_err = netconn_recv(newconn, &buf);
                    while (ERR_OK == recv_err){ 

                        do{        
                            netbuf_data(buf, &data, &len);
                        }while (netbuf_next(buf) >= 0); 
                        hello_gigadevice_recv(newconn, data, len);
                        netbuf_delete(buf);

                        recv_err = netconn_recv(newconn, &buf);
                    }
                
                    /* close connection and discard connection identifier */
                    netconn_close(newconn);
                    netconn_delete(newconn);
                }
            }
        }else{
            netconn_delete(newconn);
            printf(" can not bind TCP netconn");
        }
    }else{
        printf("can not create TCP netconn");
    }
}

#endif /* ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1)) */



#if LWIP_SOCKET
#include "lwip/sockets.h"

struct recev_packet 
{
    int length;
    char bytes[MAX_NAME_SIZE];
    int done;
}name_recv;

static err_t hello_gigadevice_recv(int fd, void *data, int len);

/*!
    \brief      called when a data is received on the telnet connection 
    \param[in]  fd: the socket id which to send data
    \param[in]  data: pointer to the data received
    \param[in]  len: size of the data
    \param[out] none
    \retval     err_t: error value
*/ 
static err_t hello_gigadevice_recv(int fd, void *data, int len)
{
    char *c;
    int i;
    int done;
  
    done = 0;
    c = (char *)data;

    /* a telnet communication packet is ended with an enter key, 
       in socket, here is to check whether the last packet is complete */
    for(i=0; i<len && !done; i++){ 
        done = ((c[i] == '\r') || (c[i] == '\n'));
    } 
    
    /* when the packet length received is no larger than MAX_NAME_SIZE */
    if(0 == name_recv.done){
        /* havn't received the end of the packet, so the received data length
           is the configured socket reception limit--MAX_NAME_SIZE */
        if(0 == done){
            memcpy(name_recv.bytes, data, MAX_NAME_SIZE);
            name_recv.length = MAX_NAME_SIZE;
            name_recv.done = 1; 
        /* have received the end of the packet */  
        }else{
            memcpy(name_recv.bytes, data, len);
            name_recv.length = len;        
        }
    }

    if(1 == done){ 
        if(c[len-2] != '\r' || c[len-1] != '\n'){
            /* limit the received data length to MAX_NAME_SIZE - 2('\r' and '\n' will be put into the buffer) */
            if((c[len-1] == '\r' || c[len-1] == '\n') && (len+1 <= MAX_NAME_SIZE)){ 
                /* calculate the buffer size to be sent(including '\r' and '\n') */
                name_recv.length += 1;
            }else if(len+2 <= MAX_NAME_SIZE){ 
                name_recv.length += 2;
            }else{  
                name_recv.length = MAX_NAME_SIZE;
            }
            
            /* save the received data to name_recv.bytes */
            name_recv.bytes[name_recv.length - 2] = '\r';
            name_recv.bytes[name_recv.length - 1] = '\n';
        }

        send(fd, (void *)&HELLO, sizeof(HELLO), 0);
        send(fd, name_recv.bytes, name_recv.length, 0);
        
        name_recv.done = 0;
        name_recv.length = 0;
    }
 
    return ERR_OK;
}


/*!
    \brief      hello task 
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void hello_task(void *arg)
{
    int ret;
    int sockfd = -1, newfd = -1;
    uint32_t len;
    int tcp_port = 8000;
    int recvnum;
    struct sockaddr_in svr_addr, clt_addr;        
    char buf[50];

    /* bind to port 8000 at any interface */
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(tcp_port);
    svr_addr.sin_addr.s_addr = htons(INADDR_ANY);
  
    name_recv.length = 0;
    name_recv.done = 0;
         
    while(1){
        /* create a TCP socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0){
               continue;
        }
        
        ret = bind(sockfd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
        if (ret < 0){ 
            lwip_close(sockfd);
            sockfd = -1;
            continue;
        }

        /* listen for incoming connections (TCP listen backlog = 1) */
        ret = listen( sockfd, 1 );
        if (ret < 0){ 
            lwip_close(sockfd);
            continue;
        }

        len = sizeof(clt_addr);

        /* grab new connection */
        newfd = accept(sockfd, (struct sockaddr *)&clt_addr, (socklen_t *)&len);
        if (-1 != newfd){ 
            send(newfd, (void *)&GREETING, sizeof(GREETING), 0);
        }

        while(-1 != newfd){
            /* reveive packets, and limit a reception to MAX_NAME_SIZE bytes */
            recvnum = recv(newfd, buf, MAX_NAME_SIZE, 0);
            if (recvnum <= 0){
                lwip_close(newfd);
                newfd = -1;
                break;
            }
            hello_gigadevice_recv(newfd, buf, recvnum);
        }

        lwip_close(sockfd);
        sockfd = -1;
    }    
}




#endif /* LWIP_SOCKET */


/*!
    \brief      initialize the hello application
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hello_gigadevice_init(void)
{
    xTaskCreate(hello_task, "HELLO", DEFAULT_THREAD_STACKSIZE, NULL, HELLO_TASK_PRIO, NULL);  
}
