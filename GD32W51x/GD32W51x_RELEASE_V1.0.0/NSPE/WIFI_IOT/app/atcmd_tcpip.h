/*!
    \file    atcmd_tcpip.h
    \brief   AT command TCPIP part for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#ifndef __ATCMD_TCPIP_H__
#define __ATCMD_TCPIP_H__

#include "lwipopts.h"

#define CIP_TYPE_TCP                 0
#define CIP_TYPE_UDP                 1
#define CIP_ROLE_CLIENT              0
#define CIP_ROLE_SERVER              1
#define MAX_CLIENT_NUM               (MEMP_NUM_NETCONN - 1)  /* Reserved 1 netconn for tcp server */

#define CIP_RECV_STACK_SIZE          256
#define CIP_RECV_PRIO                TASK_PRIO_APP_BASE

typedef struct _client_info {
    int fd;
    uint8_t type;
    uint8_t role;
    uint32_t remote_ip;
    uint16_t remote_port;
    uint16_t local_port;
} client_info_t;

typedef struct _cip_info {
    int local_srv_fd;
    uint16_t local_srv_port;
    client_info_t cli[MAX_CLIENT_NUM];
    uint32_t cli_num;
} cip_info_t;

#endif  /* __ATCMD_TCPIP_H__ */
