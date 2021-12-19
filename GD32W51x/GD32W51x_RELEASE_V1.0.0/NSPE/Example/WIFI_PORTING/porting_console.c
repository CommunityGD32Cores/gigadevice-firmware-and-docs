/*!
    \file    console.c
    \brief   Command console for GD32W51x WiFi SDK

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

#include "gd32w51x.h"
#include "wrapper_os.h"
#include "debug_print.h"
#include "console.h"
#include "uart.h"
#include "wifi_netlink.h"
#include "app_type.h"
#include "wifi_management.h"
#include "lwip/netifapi.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/stats.h"
#include "mbedtls/config.h"

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#define MAX_ARGC    8
/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
os_task_t console_task_tcb;
uint32_t console_task_stk[CONSOLE_TASK_STK_SIZE] __ALIGNED(8);
/*============================ LOCAL VARIABLES ===============================*/
static uint8_t exit_cmd_mode = 0;
/*============================ PROTOTYPES ====================================*/
static void cmd_help(int argc, char **argv);
extern void cmd_ping(int argc, char **argv);
extern void dump_mem_block_list(void);
#ifdef MBEDTLS_SELF_TEST
extern void cmd_ssl_selftest(int argc, char **argv);
#endif
/*============================ IMPLEMENTATION ================================*/
/*!
    \brief      initialize console
    \param[in]  none
    \param[out] none
    \retval     none
*/
void console_init(void)
{
    void *handle;

    handle = sys_task_create(&console_task_tcb, (const uint8_t *)"console", &console_task_stk[0],
                    CONSOLE_TASK_STK_SIZE, CONSOLE_TASK_QUEUE_SIZE, CONSOLE_TASK_PRIO,
                    (task_func_t)console_task, NULL);
    if (handle == NULL) {
        DEBUGPRINT("create console task failed\r\n");
    }
}

/*!
    \brief      start console task
    \param[in]  p_arg: the pointer of parameters
    \param[out] none
    \retval     none
*/
void console_task(void *p_arg)
{
    uint32_t cycle_cnt = 0;

    DEBUGPRINT("\r\n# ");

    for (;;) {
        command_handler();
        sys_ms_sleep(10000);
        cycle_cnt++;
        if (cycle_cnt >= 1000) {
            break;
        }
    }

    sys_task_delete(NULL);
}

/*!
    \brief      open wifi
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_open(int argc, char **argv)
{
    if (!p_wifi_netlink->device_opened) {
        wifi_pmu_config();
        wifi_rcu_config();
        wifi_setting_config();
        wifi_netlink_dev_open();
    } else {
        DEBUGPRINT("wifi device had been opened!\r\n");
    }
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    if (eloop_event_send(ELOOP_EVENT_WAKEUP) != OS_OK) {
        wifi_management_start();
    }
#endif
}

/*!
    \brief      close wifi
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_close(int argc, char **argv)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    eloop_event_send(ELOOP_EVENT_TERMINATE);
#endif
    if (p_wifi_netlink->device_opened) {
        wifi_netlink_dev_close();
        /* wlan related clock disable */
        wifi_clock_gate();
        /* set PMU to power off wlan */
        wifi_power_state_set(PMU_WIFI_SLEEP);
    } else {
        DEBUGPRINT("wifi device had been closed!\r\n");
    }
}

/*!
    \brief      show wifi scan information
    \param[in]  scan_item: the pointer of WiFi scan information used in getting scan result
    \param[out] none
    \retval     none
*/
static void scan_info_print(struct wifi_scan_info *scan_item)
{
    DEBUGPRINT("----------------------------------------------------\r\n");
    DEBUGPRINT("SSID:        %s\r\n", &scan_item->ssid.ssid);
    DEBUGPRINT("Channel:     %d\r\n", scan_item->channel);
    switch (scan_item->encryp_protocol) {
    case WIFI_ENCRYPT_PROTOCOL_OPENSYS:
        DEBUGPRINT("Security:    Open\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WEP:
        DEBUGPRINT("Security:    WEP\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA:
        DEBUGPRINT("Security:    WPA\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA2:
        DEBUGPRINT("Security:    WPA2\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA3_TRANSITION:
        DEBUGPRINT("Security:    WPA2/WPA3\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA3_ONLY:
        DEBUGPRINT("Security:    WPA3\r\n");
        break;
    default:
        DEBUGPRINT("Security:    Unknown\r\n");
        break;
    }

    if (scan_item->network_mode == WIFI_NETWORK_MODE_INFRA) {
        DEBUGPRINT("Network:     Infrastructure\r\n");
    } else if (scan_item->network_mode == WIFI_NETWORK_MODE_ADHOC) {
        DEBUGPRINT("Network:     Adhoc\r\n");
    } else {
        DEBUGPRINT("Network:     Unknown\r\n");
    }

    // bitrate_print(scan_item->rate);
    DEBUGPRINT("Rate:        %d Mbps\r\n", scan_item->rate);
    DEBUGPRINT("RSSI:        %d dbm\r\n", scan_item->rssi);
    DEBUGPRINT("BSSID:       "MAC_FMT"\r\n", MAC_ARG(scan_item->bssid_info.bssid));

    DEBUGPRINT("\r\n");
}

/*!
    \brief      callback function after scan done
    \param[in]  eloop_data: the pointer of related data after the loop has stopped
    \param[in]  user_ctx: the pointer of user parameter
    \param[out] none
    \retval     none
*/
void cmd_cb_scan_done(void *eloop_data, void *user_ctx)
{
    DEBUGPRINT("[Scanned AP list]\r\n");

    wifi_netlink_scan_list_get(scan_info_print);

    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
}

/*!
    \brief      callback function after scan failed
    \param[in]  eloop_data: the pointer of related data after the loop has stopped
    \param[in]  user_ctx: the pointer of user parameter
    \param[out] none
    \retval     none
*/
void cmd_cb_scan_fail(void *eloop_data, void *user_ctx)
{
    DEBUGPRINT("WIFI_SCAN: failed\r\n");
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
}

/*!
    \brief      start wifi scan and show scan result
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_scan(int argc, char **argv)
{
    eloop_event_register(WIFI_MGMT_EVENT_SCAN_DONE, cmd_cb_scan_done, NULL, NULL);
    eloop_event_register(WIFI_MGMT_EVENT_SCAN_FAIL, cmd_cb_scan_fail, NULL, NULL);

    if (wifi_management_scan(FALSE) != 0) {
        eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
        eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
        DEBUGPRINT("start wifi_scan failed\r\n");
    }
}

/*!
    \brief      wifi connect to ap
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_connect(int argc, char **argv)
{
    int status = 0;
    uint8_t *ssid;
    uint8_t *password;

    if(argc == 2) {
        ssid = (uint8_t *)argv[1];
        password = NULL;
    } else if(argc == 3) {
        ssid = (uint8_t *)argv[1];
        password = (uint8_t *)argv[2];
    } else {
        DEBUGPRINT("\rUsage: wifi_connect SSID [PASSWORD]\r\n");
        return;
    }

    status = wifi_management_connect(ssid, password, FALSE);
    if (status != 0)
        DEBUGPRINT("start wifi_connect failed\r\n");
}

/*!
    \brief      wifi disconnect from ap
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_disconnect(int argc, char **argv)
{
    if (wifi_management_disconnect() != 0)
        DEBUGPRINT("start wifi_disconnect failed\r\n");
}

/*!
    \brief      show wifi status
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_status(int argc, char **argv)
{
    uint8_t *mac = wifi_netif_get_hwaddr();
    uint8_t *ip = (uint8_t *)wifi_netif_get_ip();
    uint8_t *gw = (uint8_t *)wifi_netif_get_gw();

    wifi_netlink_status_get();

    DEBUGPRINT("\rNetwork Interface Status\r\n");
    DEBUGPRINT("==============================\r\n");
    DEBUGPRINT("\rMAC:         [%02x:%02x:%02x:%02x:%02x:%02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    DEBUGPRINT("\rIP:          [%d.%d.%d.%d]\r\n", ip[0], ip[1], ip[2], ip[3]);
    DEBUGPRINT("\rGW:          [%d.%d.%d.%d]\r\n", gw[0], gw[1], gw[2], gw[3]);
}

/*!
    \brief      show wifi signal strength
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_rssi(int argc, char **argv)
{
    int rssi;

    rssi = wifi_netlink_bss_rssi_get();
    if (rssi < 0) {
        DEBUGPRINT("wifi rssi: current AP RSSI is %d dbm\r\n", rssi);
    } else {
        DEBUGPRINT("wifi rssi: wifi is not connected!\r\n");
    }
}


/*!
    \brief      set wifi channel
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_channel_set(int argc, char **argv)
{
    char *endptr = NULL;
    uint32_t channel = 0;
    uint32_t bandwidth = CHANNEL_WIDTH_20;
    uint32_t ch_offset = HT_SEC_CHNL_OFFSET_NONE;

    if (!p_wifi_netlink->device_opened) {
        DEBUGPRINT("wifi device is closed! please use 'wifi_open' to open device\r\n");
        return;
    }

    if (argc >= 4) {
        ch_offset = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if (*endptr != '\0' || !((ch_offset == HT_SEC_CHNL_OFFSET_ABOVE) || (ch_offset == HT_SEC_CHNL_OFFSET_BELOW))) {
            DEBUGPRINT("wifi_set_channel: invalid channel offset\r\n");
            goto usage;
        }
    }

    if (argc >= 3) {
        bandwidth = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if (*endptr != '\0') {
            DEBUGPRINT("wifi_set_channel: invalid channel bandwidth\r\n");
            goto usage;
        }
    }

    if (argc >= 2) {
        channel = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        if (*endptr == '\0') {
            if (bandwidth == 1) {
                if ((channel <= 4 && ch_offset == HT_SEC_CHNL_OFFSET_BELOW) || (channel <= 14 && channel >= 11 && ch_offset == HT_SEC_CHNL_OFFSET_ABOVE)) {
                    DEBUGPRINT("wifi_set_channel: channel 1-4 can't set 2nd channel below, channel 11-14 can't set 2nd channel above\r\n");
                    goto usage;
                }
            }
            wifi_netlink_channel_set(channel, bandwidth, ch_offset);
            return;
        } else {
            DEBUGPRINT("wifi_set_channel: invalid channel\r\n");
        }
    }
usage:
    DEBUGPRINT("Usage: wifi_set_channel <channel> [bandwidth] [offset]\r\n");
    DEBUGPRINT("\tchannel: 1 - 14\r\n");
    DEBUGPRINT("\tbandwidth: 0: 20M, 1: 40M, default 0\r\n");
    DEBUGPRINT("\toffset: 1: 2nd channel above, 3: 2nd channel below (only use for 40M, 20M will ignore)\r\n");
}

/*!
    \brief      configure wifi as AP
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ap(int argc, char **argv)
{
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
#endif
    char *ssid = NULL;
    char *passwd = NULL;
    uint8_t channel = 0;

    if (!p_wifi_netlink->device_opened) {
        DEBUGPRINT("wifi device is closed! please use 'wifi_open' to open device\r\n");
        return;
    }

    if (argc == 4) {
        ssid = argv[1];
        passwd = argv[2];
        if (strlen(ssid) > WIFI_SSID_MAX_LEN
                || strlen(passwd) > WPA_MAX_PSK_LEN
                || strlen(passwd) < WPA_MIN_PSK_LEN)
            goto usage;
        channel = atoi(argv[3]) & 0xFF;
        if (channel > 13 || channel < 1)
            goto usage;
    } else {
        goto usage;
    }

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_ap_start(ssid, passwd, channel, 0);
#else
    wifi_netif_stop_dhcp();

    wifi_netlink_ap_start(ssid, passwd, channel, 0);

    CONFIG_IP4_ADDR(&ipaddr);
    CONFIG_IP4_ADDR_NM(&netmask);
    CONFIG_IP4_ADDR_GW(&gw);

    wifi_netif_set_addr(&ipaddr, &netmask, &gw);

    if (!ap_dhcpd_started) {
        dhcpd_daemon();
        ap_dhcpd_started = 1;
    }

    wifi_netif_set_up();
#endif
    return;

usage:
    DEBUGPRINT("Usage: wifi_ap <SSID> <PASSWORD> <CHANNEL>\r\n");
    DEBUGPRINT("<SSID>: The length should be less than 32.\r\n");
    DEBUGPRINT("<PASSWORD>: The length should be between 8 and 63.\r\n");
    DEBUGPRINT("<CHANNEL>: 1~13.\r\n");
}

/*!
    \brief      stop wifi ap mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ap_stop(int argc, char **argv)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_sta_start();
#else
    if (p_wifi_netlink->ap_started){
        /* Stop DHCPD */
        if (ap_dhcpd_started) {
            stop_dhcpd_daemon();
            ap_dhcpd_started = 0;
        }
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }
#endif
}


/*!
    \brief      show memory usage
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void cmd_mem_status(int argc, char **argv)
{
    uint8_t i;

    DEBUGPRINT("\rHeap Information\r\n");
    DEBUGPRINT("==================================\r\n");
    DEBUGPRINT("\rheap status: total free heap size = %d\r\n", sys_free_heap_size());
    DEBUGPRINT("\rheap status: total min free heap size = %d\r\n", sys_min_free_heap_size());

    DEBUGPRINT("Task Stack Information\r\n");
    DEBUGPRINT("==================================\r\n");
    DEBUGPRINT("\ros idle task: free stack = %d\r\n", sys_stack_free_get(sys_idle_task_handle_get()));
    DEBUGPRINT("\ros timer task: free stack = %d\r\n", sys_stack_free_get(sys_timer_task_handle_get()));
    DEBUGPRINT("\rconsole task: free stack = %d\r\n", sys_stack_free_get(&console_task_tcb));
    for (i = 0; i < LWIP_TASK_MAX; i++) {
        DEBUGPRINT("\rlwip task %d: free stack = %d\r\n", i, sys_stack_free_get(&lwip_task_tcb[i]));
    }

    wifi_netlink_task_stack_get();
#ifdef CONFIG_GAGENT_TEST
    DEBUGPRINT("\rgagent test task: free stack = %d\r\n", sys_stack_free_get(&gagent_task_tcb));
#endif
    DEBUGPRINT("\r\n");

    DEBUGPRINT("\r\n\rMemory List\r\n");
    DEBUGPRINT("==================================\r\n");
    dump_mem_block_list();
}


/*!
    \brief      configure wifi power save mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ps(int argc, char **argv)
{
    char *endptr = NULL;
    uint32_t ps_enable = 1;

    if (argc >= 2) {
        ps_enable = (uint32_t)strtoul((const char *)argv[1], &endptr, 0);
        if (*endptr != '\0' || ps_enable > 2) {
            goto usage;
        }
    } else {
        DEBUGPRINT("Current ps mode: %d\r\n", wifi_netlink_ps_get());
        goto usage;
    }

    wifi_netlink_ps_set(ps_enable);
    if (ps_enable) {
        DEBUGPRINT("wifi_ps: power save enabled!\r\n");
    } else {
        DEBUGPRINT("wifi_ps: power save disabled!\r\n");
    }
    return;

usage:
    DEBUGPRINT("Usage: wifi_ps <0, 1 or 2>\r\n");
    DEBUGPRINT("\t0: Wifi not sleep and CPU not sleep\r\n");
    DEBUGPRINT("\t1: WiFi in sleep and CPU not sleep\r\n");
    DEBUGPRINT("\t2: WiFi in sleep and CPU in deep sleep\r\n");
}

/*!
    \brief      parse command
    \param[in]  buf: the pointer to the buffer that holds commands
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     number of commands
*/
static int cmd_parse(char *buf, char **argv)
{
    int argc = 0;

    while((argc < MAX_ARGC) && (*buf != '\0')) {
        argv[argc] = buf;
        argc ++;
        buf ++;

        while((*buf != ' ') && (*buf != '\0'))
            buf ++;

        while(*buf == ' ') {
            *buf = '\0';
            buf ++;
        }
    }

    return argc;
}
/*!
    \brief      restart system
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_reboot(int argc, char **argv)
{
    if (p_wifi_netlink->device_opened) {
        /* clock: wlan disable, wlan run disable */
        wifi_clock_gate();
        /* set PMU to power off wlan */
        wifi_power_state_set(PMU_WIFI_SLEEP);
    }

    NVIC_SystemReset();
}

static const cmd_entry cmd_table[] = {
    {"wifi_open", cmd_wifi_open},
    {"wifi_close", cmd_wifi_close},
    {"wifi_scan", cmd_wifi_scan},
    {"wifi_connect", cmd_wifi_connect},
    {"wifi_disconnect", cmd_wifi_disconnect},
    {"wifi_status", cmd_wifi_status},
    {"wifi_rssi", cmd_wifi_rssi},
    {"wifi_set_channel", cmd_wifi_channel_set},
    {"wifi_ps", cmd_wifi_ps},
    {"wifi_ap", cmd_wifi_ap},
    {"wifi_stop_ap", cmd_wifi_ap_stop},
    {"mem_status", cmd_mem_status},
    {"ping", cmd_ping},
    {"reboot", cmd_reboot},
#ifdef MBEDTLS_SELF_TEST
    {"ssl_selftest", cmd_ssl_selftest},
#endif
    {"help", cmd_help}
};

/*!
    \brief      show help command list
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_help(int argc, char **argv)
{
    int i;

    DEBUGPRINT("\rCOMMAND LIST:\r\n");
    DEBUGPRINT("==============================\r\n");

    for (i = 0; i < ARRAY_SIZE(cmd_table); i++)
        DEBUGPRINT("    %s\r\n", cmd_table[i].command);
}

/*!
    \brief      command handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void command_handler(void)
{
    int i, argc;
    char *argv[MAX_ARGC];
    extern char uart_buf[];
    char local_uart_buf[UART_BUFFER_SIZE];
    WIFI_MESSAGE_TYPE_E msg_type;

    while (1) {
        while (1) {
            if (sys_task_wait(0, &msg_type) == OS_OK) {
                if (msg_type == WIFI_MESSAGE_SHELL_COMMAND) {
                    sys_memcpy(local_uart_buf, uart_buf, UART_BUFFER_SIZE);
                    uart_buf[0] = '\0';
                    DEBUGPRINT("\r\n");
                    break;
                }
            } else {
                DEBUGPRINT("command_handler: sys_task_wait error\n");
            }
        }

        if ((argc = cmd_parse(local_uart_buf, argv)) > 0) {
            int found = 0;
            for (i = 0; i < ARRAY_SIZE(cmd_table); i++) {
                if (strcmp(argv[0], cmd_table[i].command) == 0) {
                    cmd_table[i].function(argc, argv);
                    found = 1;
                    break;
                }
            }
            if (!found)
                DEBUGPRINT("Unknown command '%s'\r\n", argv[0]);
        }
        DEBUGPRINT("# ");
    }
}
