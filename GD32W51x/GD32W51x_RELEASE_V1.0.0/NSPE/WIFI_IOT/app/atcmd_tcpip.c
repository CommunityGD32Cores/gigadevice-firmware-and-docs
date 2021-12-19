/*!
    \file    atcmd_tcpip.c
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

#include "atcmd_tcpip.h"
#include "lwip/netdb.h"
static cip_info_t cip_info;
static int cip_task_started = 0;
static int cip_task_terminate = 0;

/*!
    \brief      initialize structure of tcpip information
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cip_info_init(void)
{
    int i;

    cip_task_terminate = 0;

    sys_memset(&cip_info, 0, sizeof(cip_info));
    cip_info.local_srv_fd = -1;
    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        cip_info.cli[i].fd = -1;
    }
}

/*!
    \brief      allocate storage space for tcpip information
    \param[in]  none
    \param[out] none
    \retval     the location of the array used to store tcpip information
*/
static int cip_info_cli_alloc(void)

{
    int i;

    if (cip_info.cli_num >= MAX_CLIENT_NUM) {
        return -1;
    }
    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd < 0)
            return i;
    }
    return -1;
}

/*!
    \brief      store tcpip information
    \param[in]  fd: the ID of the array used to store tcpip information
    \param[in]  type: the type of the client
    \param[in]  role: the role of the client
    \param[in]  remote_ip: remote ip
    \param[in]  remote_port: remote port
    \param[in]  local_port: local port
    \param[out] none
    \retval     the location of the array used to store tcpip information
*/
static int cip_info_cli_store(int fd, char *type, uint8_t role,
                            uint32_t remote_ip, uint16_t remote_port, uint16_t local_port)
{
    int idx = cip_info_cli_alloc();
    if ((idx < 0) || (fd < 0))
        return -1;
    cip_info.cli[idx].fd = fd;
    if (strncmp(type, "TCP", 3) == 0)
        cip_info.cli[idx].type = CIP_TYPE_TCP;
    else
        cip_info.cli[idx].type = CIP_TYPE_UDP;
    cip_info.cli[idx].role = role;
    cip_info.cli[idx].remote_ip = remote_ip;
    cip_info.cli[idx].remote_port = remote_port;
    cip_info.cli[idx].local_port = local_port;

    cip_info.cli_num++;

    return idx;
}

/*!
    \brief      free the specified array that stores tcpip information
    \param[in]  index: the location of the array used to store tcpip information
    \param[out] none
    \retval     none
*/
static void cip_info_cli_free(int index)
{
    if ((index >= 0) && (index < MAX_CLIENT_NUM)) {
        if (cip_info.cli[index].fd != -1) {
            sys_memset(&cip_info.cli[index], 0, sizeof(cip_info.cli[index]));
            cip_info.cli[index].fd = -1;
            cip_info.cli_num--;
        }
    }
}

/*!
    \brief      find the specified array that stores tcpip information
    \param[in]  fd: the ID of the array used to store tcpip information
    \param[out] none
    \retval     location of the array used to store tcpip information
*/
static int cip_info_cli_find(int fd)

{
    int i;

    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd == fd)
            return i;
    }
    return -1;
}

/*!
    \brief      reset tcpip information
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cip_info_reset(void)
{
    int i, fd;

    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd >= 0) {
            fd = cip_info.cli[i].fd;
            cip_info_cli_free(i);
            close(fd);
        }
    }
    if (cip_info.local_srv_fd >= 0) {
        fd = cip_info.local_srv_fd;
        cip_info.local_srv_fd = -1;
        cip_info.local_srv_port = 0;
        close(fd);
    }
    cip_task_terminate = 1;
}


/*!
    \brief      start a tcp client
    \param[in]  srv_ip: server IP address
    \param[in]  srv_port: server port
    \param[in]  bkeep_alive: time of keep alive
    \param[out] none
    \retval     the result of operation
*/
static int tcp_client_start(char *srv_ip, uint16_t srv_port, uint8_t bkeep_alive)
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    uint32_t nodelay = 0, keep_alive = 10; /* Seconds */
    int fd, ret, idx;
    uint32_t srv_ip_int = inet_addr(srv_ip);

    sys_memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(srv_port);
    saddr.sin_addr.s_addr = srv_ip_int;

    /* creating a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        AT_TRACE("Create tcp client socket fd error!\r\n");
        return -1;
    }
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
            (const char *) &nodelay, sizeof( nodelay ) );
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE,
            (const char *) &keep_alive, sizeof( keep_alive ) );

    AT_TRACE("TCP: server IP=%s port=%d.\r\n", srv_ip, srv_port);
    AT_TRACE("TCP: create socket %d.\r\n", fd);

    /* connecting to TCP server */
    ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        AT_TRACE("Tcp client connect server error!\r\n");
        ret = -2;
        goto Exit;
    }
    /* Get local port */
    sys_memset(&saddr, 0, sizeof(saddr));
    getsockname(fd, (struct sockaddr *)&saddr, &len);
    /* save client info */
    idx = cip_info_cli_store(fd, "TCP", CIP_ROLE_CLIENT,
                            srv_ip_int, srv_port, ntohs(saddr.sin_port));
    if (idx < 0) {
        AT_TRACE("Client num reached the maximum!\r\n");
        ret = -3;
        goto Exit;
    }
    return 0;

Exit:
    close(fd);
    return ret;
}

/*!
    \brief      send tcp packet
    \param[in]  fd: the socket of the tcp client
    \param[in]  tx_len: length of tcp packet to be sent
    \param[out] none
    \retval     the result of operation
*/
static int at_tcp_send(int fd, uint32_t tx_len)
{
    char *tx_buf = NULL;
    char ch;
    int cnt = 0, idx;

    tx_buf = sys_malloc(tx_len);
    if (NULL == tx_buf) {
        AT_TRACE("Allocate client buffer failed (len = %u).\r\n", tx_len);
        return -1;
    }
    AT_RSP(">");
    usart_interrupt_disable(log_uart_conf.usart_periph, USART_INT_RBNE);
    while (1) {
        while (RESET == usart_flag_get(log_uart_conf.usart_periph, USART_FLAG_RBNE));
        ch = usart_data_receive(log_uart_conf.usart_periph);
        if ((ch == AT_LF || ch == AT_CR) && (cnt >= tx_len))
            break;
        if (cnt < tx_len) {
            tx_buf[cnt] = ch;
            cnt++;
        } else {
            AT_RSP("%c", ch);
        }
    }
    usart_interrupt_enable(log_uart_conf.usart_periph, USART_INT_RBNE);
    //buffer_dump("TX:", (uint8_t *)tx_buf, cnt);
    cnt = send(fd, tx_buf, cnt, 0);
    if (cnt < 0) {
        AT_TRACE("TCP client send data error.\r\n");
        idx = cip_info_cli_find(fd);
        cip_info_cli_free(idx);
        close(fd);
    }

    sys_mfree(tx_buf);
    return cnt;
}

/*!
    \brief      start a udp client
    \param[in]  srv_ip: server IP address
    \param[in]  srv_port: server port
    \param[out] none
    \retval     the result of operation
*/
static int udp_client_start(char *srv_ip, uint16_t srv_port)
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    int reuse = 1;
    int fd, ret;
    /* creating a UDP socket */
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        AT_TRACE("Create udp client socket fd error!\r\n");
        return -1;
    }
    setsockopt(fd , SOL_SOCKET, SO_REUSEADDR,
            (const char *)&reuse, sizeof(reuse));

    sys_memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;
    saddr.sin_len         = sizeof(saddr);
    saddr.sin_port        = 0;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* binding the UDP socket to a random port */
    ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        AT_TRACE("Bind udp server socket fd error!\r\n");
        goto Exit;
    }
    /* Get local port */
    sys_memset(&saddr, 0, sizeof(saddr));
    getsockname(fd, (struct sockaddr *)&saddr, &len);
    //AT_TRACE("UDP local port %d\r\n", ntohs(saddr.sin_port));
    /* save client info */
    ret = cip_info_cli_store(fd, "UDP", CIP_ROLE_CLIENT,
                            inet_addr(srv_ip), srv_port, ntohs(saddr.sin_port));
    if (ret < 0) {
        AT_TRACE("Client num reached the maximum!\r\n");
        ret = -2;
        goto Exit;
    }

    AT_TRACE("UDP: create socket %d.\r\n", fd);
    return 0;

Exit:
    close(fd);
    return ret;
}

/*!
    \brief      send udp packet
    \param[in]  fd: the socket of the udp client
    \param[in]  tx_len: length of tcp packet to be sent
    \param[in]  srv_ip: server ip
    \param[in]  srv_port: server port
    \param[out] none
    \retval     the result of operation
*/
static int at_udp_send(int fd, uint32_t tx_len, char *srv_ip, uint16_t srv_port)
{
    char *tx_buf = NULL;
    char ch;
    int cnt = 0, ret, idx;
    struct sockaddr_in saddr;

    tx_buf = sys_malloc(tx_len);
    if (NULL == tx_buf) {
        AT_TRACE("Allocate client buffer failed (len = %u).\r\n", tx_len);
        return -1;
    }
    AT_RSP(">");
    usart_interrupt_disable(log_uart_conf.usart_periph, USART_INT_RBNE);
    while (1) {
        while (RESET == usart_flag_get(log_uart_conf.usart_periph, USART_FLAG_RBNE));
        ch = usart_data_receive(log_uart_conf.usart_periph);
        if ((ch == AT_LF || ch == AT_CR) && (cnt >= tx_len))
            break;
        if (cnt < tx_len) {
            tx_buf[cnt] = ch;
            cnt++;
        } else {
            AT_RSP("%c", ch);
        }
    }
    usart_interrupt_enable(log_uart_conf.usart_periph, USART_INT_RBNE);
    //buffer_dump("TX:", (uint8_t *)tx_buf, cnt);

    FD_ZERO(&saddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(srv_port);
    saddr.sin_addr.s_addr = inet_addr(srv_ip);

    //AT_TRACE("UDP: server IP=%s port=%d.\r\n", srv_ip, srv_port);
    ret = sendto(fd, tx_buf, cnt, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret <= 0) {
        /* low_level_output return ERR_MEM because wifi skb is not enough */
        AT_TRACE("UDP client send data error (ret = %d).\r\n", ret);
        idx = cip_info_cli_find(fd);
        cip_info_cli_free(idx);
        close(fd);
    }

    sys_mfree(tx_buf);
    return ret;
}
/*!
    \brief      start a tcp server
    \param[in]  srv_port: server port
    \param[out] none
    \retval     the result of operation
*/
static int tcp_server_start(uint16_t srv_port)
{
    struct sockaddr_in s_local_addr;
    int status, reuse;
    int srv_fd;

    srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_fd < 0) {
        return -1;
    }

    AT_TRACE("Create TCP server socket %d.\r\n", srv_fd);
    reuse = 1;
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR,
            (const char *) &reuse, sizeof(reuse) );
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEPORT,
            (const char *) &reuse, sizeof(reuse) );

    sys_memset((char *)&s_local_addr, 0, sizeof(s_local_addr));
    s_local_addr.sin_family      = AF_INET;
    s_local_addr.sin_len         = sizeof(s_local_addr);
    s_local_addr.sin_port        = htons(srv_port);
    s_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* binding the TCP socket to the TCP server address */
    status = bind(srv_fd, (struct sockaddr *)&s_local_addr, sizeof(s_local_addr));
    if( status < 0 ) {
        AT_TRACE("Bind tcp server socket fd error!\r\n");
        goto Exit;
    }
    AT_TRACE("Bind successfully.\r\n");

    /* putting the socket for listening to the incoming TCP connection */
    status = listen(srv_fd, 20);
    if( status != 0 ) {
        AT_TRACE("Listen tcp server socket fd error!\r\n");
        goto Exit;
    }
    cip_info.local_srv_fd = srv_fd;
    cip_info.local_srv_port = srv_port;
    AT_TRACE("TCP listen port %d\r\n", srv_port);

    return 0;

Exit:
    /* close the listening socket */
    close(srv_fd);
    return status;
}
/*!
    \brief      stop all tcp server
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tcp_server_stop(void)
{
    int i, fd;

    if (cip_info.local_srv_fd >= 0) {
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            if ((cip_info.cli[i].fd >= 0) && (cip_info.cli[i].role == CIP_ROLE_SERVER)) {
                fd = cip_info.cli[i].fd;
                cip_info_cli_free(i);
                close(fd);
            }
        }
        fd = cip_info.local_srv_fd;
        cip_info.local_srv_fd = -1;
        cip_info.local_srv_port = 0;
        close(fd);
    }
}
/*!
    \brief      receive task
    \param[in]  param: the pointer of user parameter
    \param[out] none
    \retval     none
*/
static void cip_recv_task(void *param)
{
    struct timeval timeout;
    int max_fd_num = 0;
    int cli_fd, i, j, recv_sz;
    char *rx_buf;
    uint32_t rx_len = 1500;
    struct sockaddr_in saddr;
    int addr_sz = sizeof(saddr);
    fd_set read_set, except_set;
    int status;

    rx_buf = sys_malloc(rx_len);
    if(NULL == rx_buf){
        AT_TRACE("Allocate client buffer failed (len = %u).\r\n", rx_len);
        return;
    }

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    while (1) {
        if (cip_task_terminate)
            break;

        FD_ZERO(&read_set);
        FD_ZERO(&except_set);
        if (cip_info.local_srv_fd >= 0) {
            FD_SET(cip_info.local_srv_fd, &read_set);
            FD_SET(cip_info.local_srv_fd, &except_set);
            if (cip_info.local_srv_fd > max_fd_num)
                max_fd_num = cip_info.local_srv_fd;
        }
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            //if ((cip_info.cli[i].fd >= 0) && (cip_info.cli[i].type == CIP_TYPE_TCP)) {
            if (cip_info.cli[i].fd >= 0) {
                FD_SET(cip_info.cli[i].fd, &read_set);
                FD_SET(cip_info.cli[i].fd, &except_set);
                if (cip_info.cli[i].fd > max_fd_num)
                    max_fd_num = cip_info.cli[i].fd;
            }
        }
        status = select(max_fd_num + 1, &read_set, NULL, &except_set, &timeout);
        if ((cip_info.local_srv_fd >= 0) && FD_ISSET(cip_info.local_srv_fd, &read_set)) {
            if (cip_info.cli_num >= MAX_CLIENT_NUM) {
                AT_RSP("client full\r\n");
            } else {
                /* waiting for an incoming TCP connection */
                /* accepts a connection form a TCP client, if there is any. otherwise returns SL_EAGAIN */
                cli_fd = accept(cip_info.local_srv_fd,
                                (struct sockaddr *)&saddr,
                                (socklen_t*)&addr_sz);
                if (cli_fd >= 0) {
                    AT_RSP("new client %d\r\n", cli_fd);
                    status = cip_info_cli_store(cli_fd, "TCP", CIP_ROLE_SERVER,
                                        saddr.sin_addr.s_addr, saddr.sin_port, cip_info.local_srv_port);
                    if (status < 0) {
                        AT_TRACE("Store client info error %d!\r\n", status);
                        close(cli_fd);
                    }
                }
            }
        }
        /*if ((cip_info.local_srv_fd >= 0) && FD_ISSET(cip_info.local_srv_fd, &except_set)) {
            tcp_server_stop();
        }*/
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            if ((cip_info.cli[i].fd >= 0) && FD_ISSET(cip_info.cli[i].fd, &read_set)) {
                if (cip_info.cli[i].type == CIP_TYPE_TCP) {
                    recv_sz = recv(cip_info.cli[i].fd, rx_buf, rx_len, 0);
                } else {
                    sys_memset(&saddr, 0, sizeof(saddr));
                    recv_sz = recvfrom(cip_info.cli[i].fd, rx_buf, rx_len,
                                        0, (struct sockaddr *)&saddr, (socklen_t*)&addr_sz);
                }
                AT_TRACE("RX:%d, %d\r\n", cip_info.cli[i].fd, recv_sz);
                if (recv_sz < 0) { /* Recv error */
                    AT_RSP("rx error %d\r\n", recv_sz);
                    close(cip_info.cli[i].fd);
                    cip_info_cli_free(i);
                } else if (recv_sz == 0) {
                    AT_RSP("remote close %d\r\n", cip_info.cli[i].fd);
                    close(cip_info.cli[i].fd);
                    cip_info_cli_free(i);
                } else {
                    AT_RSP("+IPD,%d,%d: ", cip_info.cli[i].fd, recv_sz);
                    for (j = 0; j < recv_sz; j++)
                        AT_RSP("%c", rx_buf[j]);
                    AT_RSP("\r\nOK\r\n");
                }
            }
            if ((cip_info.cli[i].fd >= 0) && FD_ISSET(cip_info.cli[i].fd, &except_set)) {
                AT_RSP("error %d\r\n", cip_info.cli[i].fd);
                close(cip_info.cli[i].fd);
                cip_info_cli_free(i);
            }
        }
    }

    /* Exit */
    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd >= 0) {
            close(cip_info.cli[i].fd);
            cip_info_cli_free(i);
        }
    }
    /* close the listening socket */
    close(cip_info.local_srv_fd);
    cip_info.local_srv_fd = -1;
    cip_info.local_srv_port = 0;

    sys_mfree(rx_buf);
    sys_task_delete(NULL);
}

/*!
    \brief      the AT command ping
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_ping(int argc, char **argv)
{
    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            char *domain = at_string_parse(argv[1]);
            struct hostent *hptr;
            char ip_addr[32];
            struct ping_info_t ping_info;

            if (domain == NULL) {
                goto Error;
            }
            hptr = lwip_gethostbyname(domain);
            if (hptr == NULL) {
                goto Error;
            }
            inet_ntop(hptr->h_addrtype, (const ip_addr_t *)hptr->h_addr, ip_addr, sizeof(ip_addr));
            memset(&ping_info, 0, sizeof(ping_info));
            memcpy(ping_info.ping_ip, ip_addr, sizeof(ping_info.ping_ip));
            ping_info.ping_cnt = 5;
            ping_info.ping_size = 120;
            ping_info.ping_interval = 1000;
            if (ping(&ping_info) != ERR_OK)
                goto Error;
        }
    } else {
        goto Error;
    }

    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+PING=<ip or domain name>\r\n");
    return;
}

/*!
    \brief      the AT command start a tcp or udp client
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_start(int argc, char **argv)
{
    char *type, *srv_ip;
    uint16_t srv_port;
    uint8_t bkeep_alive = 0;
    char *endptr = NULL;

    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            goto Error;
        }
    } else if ((argc == 4) || (argc == 5)) {
        type = at_string_parse(argv[1]);
        srv_ip = at_string_parse(argv[2]);
        if ((type == NULL) || (srv_ip == NULL)) {
            goto Error;
        }
        srv_port = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        if (argc == 5) {
            bkeep_alive = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
            if (*endptr != '\0') {
                goto Error;
            }
        }
        if (cip_info.cli_num >= MAX_CLIENT_NUM) {
            AT_RSP("client full\r\n");
            goto Error;
        }

        if (strncmp(type, "TCP", 3) == 0) {
            if (tcp_client_start(srv_ip, srv_port, bkeep_alive) < 0)
                goto Error;
        } else if (strncmp(type, "UDP", 3) == 0) {
            if (udp_client_start(srv_ip, srv_port) < 0)
                goto Error;
        } else {
            goto Error;
        }
        if (cip_task_started == 0) {
            if (sys_task_create(NULL, (const uint8_t *)"Cip Rcv", NULL,
                            CIP_RECV_STACK_SIZE, 0, CIP_RECV_PRIO,
                            (task_func_t)cip_recv_task, NULL) == NULL)
                goto Error;
            cip_task_started = 1;
        }
    } else {
        goto Error;
    }
    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+CIPSTART=<type>,<remote ip>,<remote port>,[tcp keep alive]\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command send a tcp or udp packet
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_send(int argc, char **argv)
{
    int fd, idx;
    uint32_t tx_len;
    char *srv_ip;
    uint16_t srv_port;
    char *endptr = NULL;

    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            goto Error;
        }
    } else if (argc == 3) {
        fd = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        idx = cip_info_cli_find(fd);
        if ((*endptr != '\0') || idx < 0) {
            goto Error;
        }
        tx_len = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if ((*endptr != '\0') || (tx_len > 2048)) {
            goto Error;
        }
        //AT_TRACE("FD: %d, len %d\r\n", fd, tx_len);
        if (cip_info.cli[idx].type == CIP_TYPE_TCP) {
            if (at_tcp_send(fd, tx_len) <= 0) {
                goto Error;
            }
        } else {
            AT_RSP("ip error\r\n");
            goto Error;
        }
    } else if (argc == 5) {
        fd = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        idx = cip_info_cli_find(fd);
        if ((*endptr != '\0') || idx < 0) {
            goto Error;
        }
        tx_len = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if ((*endptr != '\0') || (tx_len > 2048)) {
            goto Error;
        }
        srv_ip = at_string_parse(argv[3]);
        if (srv_ip == NULL) {
            goto Error;
        }
        srv_port = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        //AT_TRACE("FD: %d, len %d, ip %s, port %d\r\n", fd, tx_len, srv_ip, srv_port);
        if (cip_info.cli[idx].type == CIP_TYPE_TCP) {
            if (at_tcp_send(fd, tx_len) <= 0) {
                goto Error;
            }
        } else {
            if (at_udp_send(fd, tx_len, srv_ip, srv_port) <= 0) {
                goto Error;
            }
        }
    } else {
        goto Error;
    }
    AT_RSP("SEND OK\r\n");
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+CIPSEND=<fd>,<len>,[remote ip],[remote port]\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command start or stop a tcp server
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_server(int argc, char **argv)
{
    uint8_t enable;
    uint16_t port = 0;
    char *endptr = NULL;


    if ((argc == 2) || (argc == 3)) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            enable = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
            if ((*endptr != '\0') || (enable > 1)) {
                goto Error;
            }
            if (argc == 3) {
                port = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
                if (*endptr != '\0') {
                    goto Error;
                }
            }
            if (enable) {
                if (cip_info.local_srv_fd >= 0) {
                    AT_RSP("Already run\r\n");
                    goto Error;
                }
                if (tcp_server_start(port) < 0) {
                    goto Error;
                }
                if (cip_task_started == 0) {
                    if (sys_task_create(NULL, (const uint8_t *)"Cip Rcv", NULL,
                                    CIP_RECV_STACK_SIZE, 0, CIP_RECV_PRIO,
                                    (task_func_t)cip_recv_task, NULL) == NULL)
                        goto Error;
                    cip_task_started = 1;
                }
            } else {
                tcp_server_stop();
            }
        }
    } else {
        goto Error;
    }

    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+CIPSERVER:<mode:0-1>,[port]\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command free tcpip information, close client and server
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_close(int argc, char **argv)
{
    int fd, i;
    char *endptr = NULL;

    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            fd = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
            if ((*endptr != '\0') || (fd < 0)) {
                goto Error;
            }
            for (i = 0; i < MAX_CLIENT_NUM; i++) {
                if (fd == cip_info.cli[i].fd) {
                    cip_info_cli_free(i);
                    AT_RSP("close %d\r\n", fd);
                    break;
                }
            }
            if (fd == cip_info.local_srv_fd) {
                AT_RSP("server fd\r\n");
                goto Error;
            } else if (i < MAX_CLIENT_NUM) {
                close(fd);
            } else {
                goto Error;
            }
        }
    } else {
        goto Error;
    }
    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+CIPCLOSE: <fd>\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command show tcpip information status
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_status(int argc, char **argv)
{
    int i;
    char type[4];

    if (argc == 1) {
        if (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_LINKED_CONFIGED) {
            if (cip_info.cli_num > 0) {
                AT_RSP("STATUS: 3\r\n");
            } else {
                AT_RSP("STATUS: 2\r\n");
            }
        } else if (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_NO_LINK) {
            AT_RSP("STATUS: 5\r\n");
        } else {
            AT_RSP("STATUS: 4\r\n");
        }
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            if (cip_info.cli[i].fd >= 0) {
                if (cip_info.cli[i].type == CIP_TYPE_TCP)
                    strcpy(type, "TCP");
                else
                    strcpy(type, "UDP");
                AT_RSP("+CIPSTATUS:%d,%s,"IP_FMT",%d,%d,%d\r\n",
                        cip_info.cli[i].fd, type, IP_ARG(cip_info.cli[i].remote_ip),
                        cip_info.cli[i].remote_port, cip_info.cli[i].local_port, cip_info.cli[i].role);
            }
        }
    } else {
        goto Error;
    }
    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
}

/*!
    \brief      the AT command set station ip
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_sta_ip(int argc, char **argv)
{
    char *ip_str;
    ip_addr_t *ip_get;
    ip_addr_t ip, netmask, gw;

    if (argc == 1) {
        ip_get = wifi_netif_get_ip();
        AT_RSP("+CIPSTA: "IP_FMT"\r\n", IP_ARG(ip_get->addr));
        ip_get = wifi_netif_get_netmask();
        AT_RSP("+CIPSTA: "IP_FMT"\r\n", IP_ARG(ip_get->addr));
        ip_get = wifi_netif_get_gw();
        AT_RSP("+CIPSTA: "IP_FMT"\r\n", IP_ARG(ip_get->addr));
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION)
            goto Usage;
        else
            goto Error;
    } else if (argc == 4) {
        ip_str = at_string_parse(argv[1]);
        ipaddr_aton(ip_str, &ip);
        ip_str = at_string_parse(argv[2]);
        ipaddr_aton(ip_str, &netmask);
        ip_str = at_string_parse(argv[3]);
        ipaddr_aton(ip_str, &gw);
        AT_TRACE("+CIPSTA: set "IP_FMT", "IP_FMT", "IP_FMT"\r\n",
                    IP_ARG(ip.addr), IP_ARG(netmask.addr), IP_ARG(gw.addr));
        wifi_netif_set_ip(&ip, &netmask, &gw);
    } else {
        goto Error;
    }

    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+CIPSTA:<ip>,<netmask>,<gw>\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command show ap and station ip
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cip_ip_addr_get(int argc, char **argv)
{
    uint8_t *addr;
    uint32_t *ip;
    if (argc == 1) {
        addr = wifi_netif_get_hwaddr();
        ip = (uint32_t *)wifi_netif_get_ip();
        if (p_wifi_netlink->ap_started) {
            AT_RSP("+CIFSR:APIP,"IP_FMT"\r\n", IP_ARG(*ip));
            AT_RSP("+CIFSR:APMAC,"MAC_FMT"\r\n", MAC_ARG(addr));
        } else {
            AT_RSP("+CIFSR:STAIP,"IP_FMT"\r\n", IP_ARG(*ip));
            AT_RSP("+CIFSR:STAMAC,"MAC_FMT"\r\n", MAC_ARG(addr));
        }
    } else {
        goto Error;
    }
    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
}
