/*!
    \file    atcmd.c
    \brief   AT command for GD32W51x WiFi SDK

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

#if defined(CONFIG_ATCMD)

#include "wifi_version.h"
#include "atcmd.h"
#include "uart.h"
#include "bsp_inc.h"
#include "ping/ping.h"
#include "lwip/sockets.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif

#if defined(CONFIG_ATCMD) && !defined(CONFIG_WIFI_MANAGEMENT_TASK)
    #error CONSOLE_ATCMD and CONFIG_WIFI_MANAGEMENT_TASK should enabled at the same time
#endif

static uart_config_t log_uart_conf = {
    .usart_periph = LOG_UART,
    .baudrate = 115200U,
    .databits = USART_WL_8BIT,
    .stopbits = USART_STB_1BIT,
    .parity = USART_PM_NONE,
    .flow_ctrl = (USART_CTS_DISABLE | USART_RTS_DISABLE),
};

uint8_t GlobalDebugLevel_backup;
uint8_t atcmd_mode = 0;

/*!
    \brief      get the current AT command mode
    \param[in]  none
    \param[out] none
    \retval     current command mode
*/
int atcmd_mode_get(void)
{
    return atcmd_mode;
}

/*!
    \brief      convert command to serial port configuration parameters
    \param[in]  cmd: serial port configuration parameters included in the command
    \param[out] gd: serial port configuration parameters
    \retval     none
*/
static void uart_param_at2gd(uart_config_t *cmd, uart_config_t *gd)
{
    gd->baudrate = cmd->baudrate;
    if (cmd->databits == 8)
        gd->databits = USART_WL_8BIT;
    else
        gd->databits = USART_WL_9BIT;
    if (cmd->stopbits == 0)
        gd->stopbits = USART_STB_0_5BIT;
    else if (cmd->stopbits == 1)
        gd->stopbits = USART_STB_1BIT;
    else if (cmd->stopbits == 2)
        gd->stopbits = USART_STB_1_5BIT;
    else
        gd->stopbits = USART_STB_2BIT;
    if (cmd->parity == 0)
        gd->parity = USART_PM_NONE;
    else if (cmd->parity == 1)
        gd->parity = USART_PM_ODD;
    else
        gd->parity = USART_PM_EVEN;
    if (cmd->flow_ctrl == 0)
        gd->flow_ctrl = 0;
    else if (cmd->flow_ctrl == 1)
        gd->flow_ctrl = USART_RTS_ENABLE;
    else if (cmd->flow_ctrl == 2)
        gd->flow_ctrl = USART_CTS_ENABLE;
    else
        gd->flow_ctrl = (USART_CTS_ENABLE | USART_RTS_ENABLE);
}
/*!
    \brief      convert serial port configuration parameters to command
    \param[in]  gd: serial port configuration parameters
    \param[out] cmd: serial port configuration parameters included in the command
    \retval     none
*/
static void uart_param_gd2at(uart_config_t *gd, uart_config_t *cmd)
{
    cmd->baudrate = gd->baudrate;
    if (gd->databits == USART_WL_8BIT)
        cmd->databits = 8;
    else
        cmd->databits = 9;
    if (gd->stopbits == USART_STB_0_5BIT)
        cmd->stopbits = 0;
    else if (gd->stopbits == USART_STB_1BIT)
        cmd->stopbits = 1;
    else if (gd->stopbits == USART_STB_1_5BIT)
        cmd->stopbits = 2;
    else
        cmd->stopbits = 3;
    if (gd->parity == USART_PM_NONE)
        cmd->parity = 0;
    else if (gd->parity == USART_PM_ODD)
        cmd->parity = 1;
    else
        cmd->parity = 2;
    cmd->flow_ctrl = 0;
    if ((gd->flow_ctrl & USART_RTS_ENABLE) == USART_RTS_ENABLE)
        cmd->flow_ctrl |= BIT(0);
    if ((gd->flow_ctrl & USART_CTS_ENABLE) == USART_CTS_ENABLE)
        cmd->flow_ctrl |= BIT(1);
}

/*!
    \brief      the AT command replies OK
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void at_rsp_ok(void)
{
    AT_RSP("OK\r\n");
}

/*!
    \brief      the AT command replies ERROR
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void at_rsp_error(void)
{
    AT_RSP("ERROR\r\n");
}

/*!
    \brief      parse string
    \param[in]  param: pointer to a string waiting to be parsed
    \param[out] none
    \retval     none
*/
static char *at_string_parse(char *param)
{
    char *find = strchr(param, AT_QUOTE);
    char *head = NULL, *ptr, *tail = NULL;
    if (find == NULL)
        return NULL;
    head = ptr = ++find;
    while (1) {
        find = strchr(ptr, AT_QUOTE);
        if (find == NULL)
            break;
        else
            tail = find;
        ptr = find + 1;
    }
    if (tail != NULL)
        *tail = '\0';
    else
        return NULL;
    if (*head == '\0')
        head = NULL;
    return head;
}

/*!
    \brief      enter AT command mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_entry(int argc, char **argv)
{
    atcmd_mode = 1;
#ifdef CONFIG_DEBUG_SUPPORT
#ifndef CONFIG_ATCMD_DEBUG
    /* WiFi debug level */
    GlobalDebugLevel_backup = GlobalDebugLevel;
    GlobalDebugLevel = _drv_err_;
#endif
#endif
    cip_info_init();

    at_rsp_ok();
}

/*!
    \brief      exit AT command mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_exit(int argc, char **argv)
{
    cip_info_reset();
    sys_msleep(500);

    atcmd_mode = 0;
#ifdef CONFIG_DEBUG_SUPPORT
#ifndef CONFIG_ATCMD_DEBUG
    GlobalDebugLevel = GlobalDebugLevel_backup;
#endif
#endif
    at_rsp_ok();
}

/*!
    \brief      the AT command echo
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_echo(int argc, char **argv)
{
//TODO
    at_rsp_ok();
}

/*!
    \brief      the AT command reset system
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_reset(int argc, char **argv)
{
    usart_tx_idle_wait(log_uart_conf.usart_periph);
    wifi_rcu_deinit();
    NVIC_SystemReset();
    at_rsp_ok();
}

/*!
    \brief      the AT command show version of the software
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_version(int argc, char **argv)
{
    AT_RSP("==================================\r\n");
    AT_RSP("SDK version: "WIFI_RELEASE_VERSION" \r\n");
    AT_RSP("SDK build revision: "WIFI_BUILD_REVISION" \r\n");
    AT_RSP("SDK build date: "BUILD_DATE" \r\n");
    at_rsp_ok();
}

/*!
    \brief      the AT command show all of the tasks
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_task(int argc, char **argv)
{
    char buf[448] = {0};
    sys_task_list(buf);
    AT_RSP("==================================\r\n");
    AT_RSP("%s\r\n", buf);
    at_rsp_ok();
}

/*!
    \brief      the AT command show heap usage
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_heap(int argc, char **argv)
{
    AT_RSP("==================================\r\n");
    AT_RSP("Total free heap size = %d\r\n", sys_free_heap_size());
    AT_RSP("Total min free heap size = %d\r\n", sys_min_free_heap_size());
    at_rsp_ok();
}

/*!
    \brief      the AT command show system ram usage
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_sysram(int argc, char **argv)
{
#if defined (__CC_ARM) || defined (__ARMCC_VERSION)
    extern uint32_t Image$$RW_IRAM2$$ZI$$Limit;
    uint32_t nspe_zi_limit = (uint32_t)&Image$$RW_IRAM2$$ZI$$Limit;
#elif defined (__ICCARM)
    extern uint32_t DATA$$Limit;
    uint32_t nspe_zi_limit = (uint32_t)&DATA$$Limit;
#else
    uint32_t nspe_zi_limit = 0;
#endif
    uint32_t free_sram = TOTAL_SRAM_SIZE - (nspe_zi_limit - SRAM_BASE_NS);

    AT_RSP("==================================\r\n");
    AT_RSP("Data limit = 0x%08x\r\n", nspe_zi_limit);
    AT_RSP("Free SRAM size = %u\r\n", free_sram);

    at_rsp_ok();
}

/*!
    \brief      the AT command show system status
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_sys_status(int argc, char **argv)
{
#if !defined(CONFIG_TZ_ENABLED)
    mbl_sys_status_show();
#endif
    at_rsp_ok();
}

/*!
    \brief      the AT command show  the configuration of log usart or conifgurate log usart
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_log_uart(int argc, char **argv)
{
    uart_config_t cmd;

    if (argc == 1) {
        if (argv[0][strlen(argv[0]) - 1] == AT_QUESTION) {
            uart_param_gd2at(&log_uart_conf, &cmd);
            AT_RSP("+UART: %d, %d, %d, %d, %d\r\n",
                cmd.baudrate, cmd.databits, cmd.stopbits, cmd.parity, cmd.flow_ctrl);
        } else {
            goto Error;
        }
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION)
            goto Usage;
        else
            goto Error;
    } else if (argc == 6) {
        char *endptr = NULL;
        cmd.baudrate = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.databits = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.stopbits = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.parity = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.flow_ctrl = (uint32_t)strtoul((const char *)argv[5], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        if ((cmd.databits != 8 && cmd.databits != 9)
            || (cmd.stopbits > 3) || (cmd.parity > 2) || (cmd.flow_ctrl > 3))
            goto Error;

        uart_param_at2gd(&cmd, &log_uart_conf);
        usart_reconfig(&log_uart_conf);
    } else {
        goto Error;
    }

    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+UART=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>\r\n");
    at_rsp_ok();
    return;
}

#include "atcmd_wifi.c"
#include "atcmd_tcpip.c"
static void at_help(int argc, char **argv);
static const atcmd_entry_t atcmd_table[] = {
    /* ====== Base ====== */
    //{"ATE", at_echo},
    {"ATQ", at_exit},
    {"AT+HELP", at_help},
    {"AT+RST", at_reset},
    {"AT+GMR", at_version},
    {"AT+TASK", at_task},
    {"AT+HEAP", at_heap},
    {"AT+SYSRAM", at_sysram},
    {"AT+SYSSTATUS", at_sys_status},
    //{"AT+GSLP", at_sys_deep_sleep},
    //{"AT+SLEEP", at_sys_sleep},
    //{"AT+WAKEUPGPIO", at_sys_wakeup_gpio},
    //{"AT+RFPOWER", at_sys_rf_power},
    //{"AT+RFVDD", at_sys_rf_vdd},
    //{"AT+SYSADC", at_sys_adc},
    {"AT+UART", at_log_uart},
#if 0
    //{"AT+UART_DEF", at_sys_uart_def},
    //{"AT+SYSADC", at_sys_adc},
    {"AT+SYSIOSETCFG", at_sys_io_set_cfg},
    {"AT+SYSIOGETCFG", at_sys_io_get_cfg},
    {"AT+SYSGPIODIR", at_sys_gpio_dir},
    {"AT+SYSGPIOWRITE", at_sys_gpio_write},
    {"AT+SYSGPIOREAD", at_sys_gpio_read},
    {"AT+SYSMSG_CUR", at_sys_msg_cur},
    //{"AT+SYSMSG_DEF", at_sys_msg_def},
#endif
    /* ====== WLAN ====== */
    {"AT+CWMODE_CUR", at_cw_mode_cur},
    {"AT+CWJAP_CUR", at_cw_ap_cur_join},
    {"AT+CWLAP", at_cw_ap_list},
    {"AT+CWSTATUS", at_cw_status},
    {"AT+CWQAP", at_cw_ap_quit},
    {"AT+CWSAP_CUR", at_cw_ap_cur_start},
    {"AT+CWLIF", at_cw_ap_client_list},
    {"AT+CWAUTOCONN", at_cw_auto_connect},
#if 0
    {"AT+CWMODE_DEF", at_cw_mode_def},
    {"AT+CWJAP_DEF", at_cw_join_ap_def},
    {"AT+CWLAPOPT", at_cw_list_ap_opt},
    {"AT+CWSAP_DEF", at_cw_start_ap_def},
    {"AT+CWDHCP_CUR", at_cw_dhcp_cur},
    {"AT+CWDHCP_DEF", at_cw_dhcp_def},
    {"AT+CWDHCPS_CUR", at_cw_ap_dhcps_cur},
    {"AT+CWDHCPS_DEF", at_cw_ap_dhcps_def},
    {"AT+CWSTARTSMART", at_cw_start_smart},
    {"AT+CWSTOPSMART", at_cw_stop_smart},
    //{"AT+CWSTARTDISCOVER", at_cw_start_discover},
    //{"AT+CWSTOPDISCOVER", at_cw_stop_discover},
    //{"AT+WPS", at_cw_wps},
    {"AT+MDNS", at_cw_mdns},
    {"AT+CWHOSTNAME", at_cw_set_host_name},
    //{"AT+CWCOUNTRY_CUR", at_cw_get_country_cur},
    {"AT+CWCOUNTRY_DEF", at_cw_get_country_def},
#endif
    /* ====== TCPIP ====== */
    {"AT+PING", at_cip_ping},
    {"AT+CIPSTA", at_cip_sta_ip},
    {"AT+CIPSTART", at_cip_start},
    {"AT+CIPSEND", at_cip_send},
    {"AT+CIPSERVER", at_cip_server},
    {"AT+CIPCLOSE", at_cip_close},
    {"AT+CIPSTATUS", at_cip_status},
    {"AT+CIFSR", at_cip_ip_addr_get},
#if 0
    //{"AT+CIUPDATE", at_cip_update},
    {"AT+CIPSTAMAC_CUR", at_cip_sta_mac_cur},
    {"AT+CIPMODE", at_cip_mode},
    //{"AT+CIPAPMAC_DEF", at_cip_sta_mac_def},
    //{"AT+CIPSTA_DEF", at_cip_sta_ip_def},
    //{"AT+CIPAP_CUR", at_cip_ap_ip_cur},
    //{"AT+CIPAP_DEF", at_cip_ap_ip_def},
    {"AT+CIPDOMAIN", at_cip_domain},
    {"AT+CIPSSLSIZE", at_cip_ssl_buf_size},
    {"AT+CIPSSLCCONF", at_cip_ssl_cli_conf},
    //{"AT+CIPSENDEX", at_cip_send_ex},
    {"AT+CIPSENDBUF", at_cip_send_buf},
    {"AT+CIPBUFRESET", at_cip_buf_reset},
    {"AT+CIPBUFSTATUS", at_cip_buf_status},
    //{"AT+CIPCHECKSEQ", at_cip_check_seq},
    //"AT+CIPMUX", at_cip_mux},
    {"AT+CIPSERVERMAXCONN", at_cip_server_max_conn},
    //{"AT+SAVETRANSLINK", at_cip_save_trans_link},
    {"AT+CIPSTO", at_cip_server_timeout},
    //{"AT+CIPDINFO", at_cip_show_info},
    //{"AT+CIPRECVMODE", at_cip_recv_mode},
    //{"AT+CIPRECVDATA", at_cip_recv_data},
    //{"AT+CIPRECVLEN", at_cip_recv_len},
    //{"AT+CIPSNTPCFG", at_cip_sntp_cfg},
    //{"AT+CIPSNTPTIME", at_cip_sntp_time},
    //{"AT+CIPDNS_CUR", at_cip_dns_cur},
    //{"AT+CIPDNS_DEF", at_cip_dns_def},
#endif
};

/*!
    \brief      the AT command show AT command list
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_help(int argc, char **argv)
{
    int i;

    AT_RSP("\rAT COMMAND LIST:\r\n");
    AT_RSP("==============================\r\n");

    for (i = 0; i < ARRAY_SIZE(atcmd_table); i++)
        AT_RSP("    %s\r\n", atcmd_table[i].command);
    AT_RSP("\r\nOK\r\n");
}

/*!
    \brief      parse AT command
    \param[in]  atcmd: the pointer of AT command
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static int atcmd_parse(char *atcmd, char **argv)
{
    int32_t i;
    int32_t argc = 0;
    char *buf = atcmd;
    uint32_t size = strlen(atcmd);

    /* Get command */
    argv[argc] = buf;
    for (i = 0; i < size; i++) {
        if (*buf == AT_EQU) {
            break;
        } else if ((*buf == AT_CR) || (*buf == AT_LF)) {
            goto Exit;
        } else {
            buf++;
        }
    }
    if (i == size)
        goto Exit;

    /* Get parameters */
    *buf++ = '\0';
    argc++;
    while((argc < AT_MAX_ARGC) && (*buf != '\0')) {
        argv[argc] = buf;
        argc++;
        buf++;

        while ((*buf != AT_SEPARATE) && (*buf != '\0')) {
            buf++;
        }
        if (*buf != '\0') {
            *buf = '\0';
            buf ++;
        }
        while (*buf == AT_SPACE) {
            *buf = '\0';
            buf ++;
        }
    }

    return argc;

Exit:
    *buf = '\0';
    argc++;
    return argc;
}

#if 0
void command_handler(void)
{
    int i, argc;
    char *argv[AT_MAX_ARGC];
    extern char uart_buf[];
    WIFI_MESSAGE_TYPE_E msg_type;
    static int at_started = 0;

    cip_info_init();

    while (1) {
        while (1) {
            if (sys_task_wait(0, &msg_type) == OS_OK) {
                if (msg_type == WIFI_MESSAGE_SHELL_COMMAND) {
                    AT_RSP("\r\n");
                    break;
                }
            } else {
                AT_TRACE("command_handler: sys_task_wait error\n");
            }
        }

        if ((argc = atcmd_parse(uart_buf, argv)) > 0) {
            int found = 0;
            if (strcmp(argv[0], "AT") == 0) {
#ifndef CONFIG_ATCMD_DEBUG
                /* WiFi debug level */
                GlobalDebugLevel = _drv_err_;
#endif
                at_started = 1;
                at_rsp_ok();
                continue;
            }
            if (at_started) {
                for (i = 0; i < ARRAY_SIZE(atcmd_table); i++) {
                    if (strncmp(argv[0], atcmd_table[i].command, strlen(atcmd_table[i].command)) == 0) {
                        atcmd_table[i].function(argc, argv);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found)
                at_rsp_error();
        }

        uart_buf[0] = '\0';
    }
}
#endif
#endif  /* defined(CONFIG_ATCMD) */
