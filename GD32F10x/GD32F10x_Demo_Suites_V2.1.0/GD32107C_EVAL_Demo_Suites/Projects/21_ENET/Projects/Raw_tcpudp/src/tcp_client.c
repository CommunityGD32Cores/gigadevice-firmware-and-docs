/*!
    \file  tcp_client.c
    \brief TCP client demo program 

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

#include "tcp_client.h"  
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <string.h>
#include <stdio.h>
#include "gd32f10x.h"
#include "main.h"

#define MAX_BUF_SIZE    50

struct recev_packet 
{
    int length;
    char bytes[MAX_BUF_SIZE];
};

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/*!
    \brief      called when a data is received on the tcp connection 
    \param[in]  arg: the user argument
    \param[in]  pcb: the tcp_pcb that has received the data
    \param[in]  p: the packet buffer
    \param[in]  err: the error value linked with the received data
    \param[out] none
    \retval     err_t: error value
*/ 
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    struct recev_packet *recev_packet = (struct recev_packet *)arg;
    int buf_full;
    char *c;
    int i;

    /* we perform here any necessary processing on the pbuf */
    if (p != NULL){    
        /* we call this function to tell the LwIp that we have processed the data */
        /* this lets the stack advertise a larger window, so more data can be received*/
        tcp_recved(pcb, p->tot_len);

        /* check the name if NULL, no data passed, return withh illegal argument error */
        if(!recev_packet){ 
            pbuf_free(p);
            return ERR_ARG;
        }

        buf_full = 0;
        
        for(q=p; q != NULL; q = q->next){ 
          
            c = q->payload;
            for(i=0; i<q->len && !buf_full; i++){
                /* if the received data size is larger than the size we want to get */
                if(recev_packet->length < MAX_BUF_SIZE){ 
                    recev_packet->bytes[recev_packet->length] = c[i];
                    recev_packet->length++;
                }else{
                    buf_full = 1;
                }
            } 
        }      
      
        /* send out the message */
        tcp_write(pcb, recev_packet->bytes, recev_packet->length, 1);       
        recev_packet->length = 0;

        pbuf_free(p);
        
    }else if(ERR_OK == err){  
      
        mem_free(recev_packet);
        return tcp_close(pcb);
    }
  
    return ERR_OK;
}

/*!
    \brief      this function when the connection is established 
    \param[in]  arg: user supplied argument
    \param[in]  pcb: the tcp_pcb which accepted the connection
    \param[in]  err: error value
    \param[out] none
    \retval     err_t: error value
*/
static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    tcp_arg(pcb, mem_calloc(sizeof(struct recev_packet), 1));
    /* configure LwIP to use our call back functions */
    tcp_recv(pcb, tcp_client_recv);

    return ERR_OK;  
}

/*!
    \brief      initialize the tcp_client application
    \param[in]  none
    \param[out] none
    \retval     none
*/

void tcp_client_init(void)
{
    struct tcp_pcb *pcb;
    ip_addr_t ipaddr;

    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);
  
    /* create a new TCP control block  */
    pcb = tcp_new();

    /* assign to the new pcb a local IP address and a port number */
    /* using IP_ADDR_ANY allow the pcb to be used by any local interface */
    if(ERR_USE != tcp_bind(pcb, IP_ADDR_ANY, 1026)){
        /* Specify the function to be called when a connection is established */    
        tcp_connect(pcb, &ipaddr, 1026, tcp_client_connected);
    }else{
        printf("connect is still alive \r\n ");
        memp_free(MEMP_TCP_PCB, pcb);
    }       
    
}
