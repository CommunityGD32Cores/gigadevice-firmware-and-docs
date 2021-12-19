/*
 * netutils: ping implementation
 */

#include "lwip/opt.h"

#if LWIP_PING
#include "app_cfg.h"
#include "app_type.h"
#include "wrapper_os.h"
#include "malloc.h"
#include "lwip/mem.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "ping.h"
#include "atcmd.h"

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping receive timeout - in milliseconds */
#define PING_RCV_TIMEO 2000
/** ping delay - in milliseconds */
#define PING_DELAY     10 //100

#define BUF_SIZE        1500

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

struct _ip_addr {
    uint8_t addr0, addr1, addr2, addr3;
};

static uint8_t ping_terminate;
static void *ping_task_hdl = NULL;

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct ping_info_t *ping_info, struct icmp_echo_hdr *iecho, uint16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_info->ping_seq_num);

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the socket ip */
static err_t ping_send(struct ping_info_t *ping_info, int s, ip_addr_t *addr, size_t size)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size;

    if (size == 0) {
        ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    } else {
        ping_size = sizeof(struct icmp_echo_hdr) + size;
    }

    LWIP_ASSERT("ping_size is too big\n\r", ping_size <= 0xffff);

    iecho = (struct icmp_echo_hdr *)sys_malloc(ping_size);
    if (iecho == NULL) {
        return ERR_MEM;
    }

    ping_prepare_echo(ping_info, iecho, (u16_t)ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = addr->addr;

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));
    sys_mfree(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(struct ping_info_t *ping_info, int s)
{
    uint32_t reply_time;
    int len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    uint32_t fromlen = sizeof(struct sockaddr);
    uint32_t delay;
    //struct _ip_addr *addr;
    //static uint32_t ping_recv_count = 0;
    uint32_t start_time = 0;
    uint32_t recv_size;
    if ((ping_info->ping_size + sizeof(struct icmp_echo_hdr) + sizeof(struct ip_hdr)) > BUF_SIZE)
        recv_size = (ping_info->ping_size + sizeof(struct icmp_echo_hdr) + sizeof(struct ip_hdr));
    else
        recv_size = BUF_SIZE;
    start_time = sys_current_time_get();
    while ((len = lwip_recvfrom(s, ping_info->reply_buf, recv_size, 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
        if (len >= (sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
            //addr = (struct _ip_addr *)&(from.sin_addr);
            //LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("@@@ping: recv %d.%d.%d.%d, len = %d, count = %d\n",
            //             addr->addr0, addr->addr1, addr->addr2, addr->addr3, len, ++ping_recv_count));
            reply_time = sys_current_time_get();
            delay = reply_time - ping_info->ping_time;

            iphdr = (struct ip_hdr *)ping_info->reply_buf;
            iecho = (struct icmp_echo_hdr *)(ping_info->reply_buf+(IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_info->ping_seq_num))) {
#if defined(CONFIG_ATCMD)
                if (atcmd_mode_get())
                    AT_RSP("+%d\r\n", delay);
                else
#endif
                    LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("[ping_test] %d bytes from %s: icmp_seq=%d time=%d ms\n\r",
                        len - sizeof(struct ip_hdr) - sizeof(struct icmp_echo_hdr), inet_ntoa(from.sin_addr),
                        htons(iecho->seqno), delay));
                ping_info->ping_recv_count++;
                if (delay > ping_info->ping_max_delay) ping_info->ping_max_delay = delay;
                if (delay < ping_info->ping_min_delay) ping_info->ping_min_delay = delay;
                ping_info->ping_avg_delay += delay;
                return;
            } else {
                if (sys_current_time_get() - start_time > PING_RCV_TIMEO) {
                    len = -1;
                    break;
                }
            }

            if ((iecho->id == PING_ID) && (iecho->type == ICMP_ER)) {
/*
                int i;
                LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("\n\rping: drop"));
                LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("------------------------------------------- \n"));
                for (i = 0; i < len; i = i + 1) {
                    LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("%02x:", *(ping_info->reply_buf + i)));
                    if (i%16 == 15)
                        LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("\n"));
                }
                LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("\n"));
                LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("------------------------------------------- \n"));
*/
            }
        }
    }

    if (len <= 0) {
#if defined(CONFIG_ATCMD)
        if (atcmd_mode_get())
            AT_RSP("+timeout\r\n");
        else
#endif
            LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("[ping_test] timeout\n\r"));
    }
}

err_t ping(struct ping_info_t *ping_info)
{
    int s;
    int timeout = PING_RCV_TIMEO;
    ip_addr_t ping_target;
    uint32_t send_count;
    char *target = ping_info->ping_ip;
    uint32_t count = ping_info->ping_cnt;
    size_t size = ping_info->ping_size;
    uint32_t interval = ping_info->ping_interval;
    //static uint32_t ping_send_count = 0;
    //uint16_t size_random; //add for random size ping test
    uint32_t recv_size;

    ping_info->ping_seq_num = 0;
    ping_info->ping_max_delay = 0;
    ping_info->ping_min_delay = (u16_t)-1;
    ping_info->ping_avg_delay = 0;
    ping_info->ping_recv_count = 0;
    send_count = 0;

    if (inet_aton(target, (struct in_addr*)&ping_target) == 0)
        return ERR_VAL;
    //addr = (struct _ip_addr*)&ping_target;

    if ((ping_info->ping_size + sizeof(struct icmp_echo_hdr)+ sizeof(struct ip_hdr)) > BUF_SIZE)
        recv_size = (ping_info->ping_size + sizeof(struct icmp_echo_hdr)+ sizeof(struct ip_hdr));
    else
        recv_size = BUF_SIZE;
    ping_info->reply_buf = sys_malloc(recv_size);
    if (NULL == ping_info->reply_buf){
        LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("ping recv: allocate buffer failed.\n\r"));
        return ERR_MEM;
    }

    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
        LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("ping: create socket failled\n\r"));
        sys_mfree(ping_info->reply_buf);
        return ERR_MEM;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

#if defined(CONFIG_ATCMD)
    if (!atcmd_mode_get())
#endif
    {
        LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("[ping_test] PING %s %d bytes of data\n\r",
            inet_ntoa(ping_target), size));
    }

    while (1) {
        //add for random size ping test
        //size_random = 32 + rand()%(1460-32+1);
        if (ping_terminate) {
            break;
        }

        //if (ping_send(s, &ping_target, size_random) == ERR_OK)
        if (ping_send(ping_info, s, &ping_target, size) == ERR_OK) {
            //LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("@@@ping: send %d.%d.%d.%d, size = %d, count = %d\n",
            //            addr->addr0, addr->addr1, addr->addr2, addr->addr3, size, ++ping_send_count));
            ping_info->ping_time = sys_current_time_get();
            ping_recv(ping_info, s);

        } else {
            LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("ping: send %s - error\n\r",
                        inet_ntoa(ping_target)));
        }

        send_count ++;
        if (send_count >= count) break; /* send ping times reached, stop */

        sys_msleep(interval); /* take a delay */
    }

#if defined(CONFIG_ATCMD)
    if (!atcmd_mode_get())
#endif
    {
        LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("[ping_test] %d packets transmitted, %d received, %d%% packet loss\n\r",
            send_count, ping_info->ping_recv_count, (send_count-ping_info->ping_recv_count) * 100 / send_count));

        if (ping_info->ping_recv_count != 0) {
            LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("[ping_test] delay: min %u ms, max %u ms, avg %u ms\n\r",
                ping_info->ping_min_delay, ping_info->ping_max_delay, ping_info->ping_avg_delay / ping_info->ping_recv_count));
        }
    }
    lwip_close(s);

    sys_mfree(ping_info->reply_buf);

    return ERR_OK;
}

static void ping_task_func(void *param)
{
    err_t err;
    struct ping_info_t *ping_info = (struct ping_info_t *)param;

    err = ping(ping_info);
    if (err != ERR_OK) {
        LWIP_DEBUGP(PING_DEBUG | LWIP_DBG_TRACE, ("ping: exit because of error!\n\r"));
    }

    sys_mfree(ping_info);
    ping_task_hdl = NULL;
    sys_task_delete(NULL);
}
#endif

static int ping_parse_arguments(struct ping_info_t *ping_info, int argc, char **argv)
{
    uint32_t arg_cnt = 1, ip_len = 0;
    uint32_t ping_size;
    uint32_t ping_time;

    if (argc < 2) {
        goto Exit;
    } else {
        ip_len = (strlen((const char *)argv[1]) < 16)? (strlen((const char *)argv[1]) + 1) : 16;
        sys_memcpy(ping_info->ping_ip, (const char *)argv[1], ip_len);
        arg_cnt ++;
    }

    ping_info->ping_cnt = 5;
    ping_info->ping_size = 120;
    ping_info->ping_interval = 10;

    while (arg_cnt < argc) {
        if (strcmp(argv[arg_cnt], "-n") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_info->ping_cnt = (uint32_t)atoi(argv[arg_cnt + 1]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-l") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_size = (uint32_t)atoi(argv[arg_cnt + 1]);
            if ((ping_size > (MEM_SIZE - 256)) || (ping_size > (PBUF_POOL_SIZE * PBUF_POOL_BUFSIZE - 512))) {
                DEBUGPRINT("ping fail!!!\r\n");
                DEBUGPRINT("Please make sure:\r\n"\
                            "\t\t\tping size <= MEM_SIZE - 256(%d) in lwipopts.h\r\n"\
                            "\t\t\tping size <= (PBUF_POOL_SIZE * PBUF_POOL_BUFSIZE - 512)(%d) in lwipopts.h\r\n",\
                            (MEM_SIZE - 256), (PBUF_POOL_SIZE * PBUF_POOL_BUFSIZE - 512));
                return 0;
            }
            ping_info->ping_size = ping_size;
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-i") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_info->ping_interval = (uint32_t)atoi(argv[arg_cnt + 1]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-t") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_time = (uint32_t)atoi(argv[arg_cnt + 1]);
            if (ping_time > 0) {
                ping_info->ping_interval = 1000;
                ping_info->ping_cnt = ping_time;
            } else {
                DEBUGPRINT("invalid run time \r\n");
                goto Usage;
            }
            arg_cnt += 2;
        }
        else {
            goto Exit;
        }
    }
    return 1;

Exit:
    DEBUGPRINT("\rping: ping test command format error!\r\n");
Usage:
    DEBUGPRINT("\rUsage:\r\n");
    DEBUGPRINT("ping <target_ip | stop> [-n count] [-l size] [-i interval] [-t total time]\r\n");
    DEBUGPRINT("    stop ping stop\r\n");
    DEBUGPRINT("    -n   number of echo request to run(default 5)\r\n");
    DEBUGPRINT("    -l   ping size of single ping test(default 120B)\r\n");
    DEBUGPRINT("    -i   time interval between single echo request(unit:ms) (default 10ms)\r\n");
    DEBUGPRINT("    -t   total run time(unit:second) (default not use)\r\n");
    DEBUGPRINT("Note: when -t option is enabled, time interval's default value is 1000ms,"\
                "and ping test number equals to time\r\n");
    return 0;
}


void cmd_ping(int argc, char **argv)
{
#if LWIP_PING
    char *endptr = NULL;
    struct ping_info_t *ping_info;
    uint32_t ip_len;

    if (argc >= 2 && (strcmp(argv[1], "stop") == 0)) {
        ping_terminate = 1;
        return;
    }

    if (ping_task_hdl != NULL) {
        DEBUGPRINT("ping task has been running, return\n\r");
        return;
    }

    ping_info = (struct ping_info_t *)sys_malloc(sizeof(struct ping_info_t));
    if (ping_info == NULL) {
        DEBUGPRINT("ping: no memory for ping_info.\r\n");
        return;
    }

    if(ping_parse_arguments(ping_info, argc, argv)) {
        ping_task_hdl = sys_task_create(NULL, (const uint8_t *)"ping_task", NULL,
                        PING_TASK_STACK_SIZE, 0, PING_TASK_PRIO,
                        (task_func_t)ping_task_func, ping_info);
        if (ping_task_hdl == NULL) {
            DEBUGPRINT("ping: create ping task failed.\r\n");
            sys_mfree(ping_info);
            return;
        }
        ping_terminate = 0;
    } else {
        sys_mfree(ping_info);
    }
#else
    DEBUGPRINT("\n\rLWIP_PING should be defined in lwipopts.h!\n");
#endif
}
