/*!
    \file    atcmd_wifi.c
    \brief   AT command WiFi part for GD32W51x WiFi SDK

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

/*!
    \brief      the AT command connect the AP
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_ap_cur_join(int argc, char **argv)
{
    int status = 0;

    if (argc == 1) {
        if (argv[0][strlen(argv[0]) - 1] == AT_QUESTION) {
            struct wifi_connect_result *ap_info = &p_wifi_netlink->connected_ap_info;
            AT_RSP("+CWJAP_CUR:%s,"MAC_FMT",%d,%d\r\n",
                    ap_info->ssid.ssid, MAC_ARG(ap_info->bssid_info.bssid),
                    ap_info->channel, ap_info->rssi);
        } else {
            goto Error;
        }
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION)
            goto Usage;
        else
            goto Error;
    } else if (argc == 3) {
        char *ssid = at_string_parse(argv[1]);
        char *password = at_string_parse(argv[2]);
        if (ssid == NULL) {
            goto Error;
        }
        status = wifi_management_connect((uint8_t *)ssid, (uint8_t *)password, TRUE);
        if (status != 0) {
            goto Error;
        }
        wifi_management_block_wait();
        if (p_wifi_netlink->link_status != WIFI_NETLINK_STATUS_LINKED_CONFIGED) {
            AT_TRACE("AT+CWJAP_CUR=%s failed(%d)\r\n", ssid, p_wifi_netlink->link_status);
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
    AT_RSP("+CWJAP_CUR=<ssid>,<pwd>\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command show scan information
    \param[in]  item: the pointer of WiFi scan information used in getting scan result
    \param[out] none
    \retval     none
*/
static void at_scan_info_print(struct wifi_scan_info *item)
{
    char *encrypt, *cipher;
    switch (item->encryp_protocol) {
        case WIFI_ENCRYPT_PROTOCOL_OPENSYS:
            encrypt = "Open";
            break;
        case WIFI_ENCRYPT_PROTOCOL_WEP:
            encrypt = "WEP";
            break;
        case WIFI_ENCRYPT_PROTOCOL_WPA:
            encrypt = "WPA";
            break;
        case WIFI_ENCRYPT_PROTOCOL_WPA2:
            encrypt = "WPA2";
            break;
        case WIFI_ENCRYPT_PROTOCOL_WAPI:
            encrypt = "WAPI";
            break;
        case WIFI_ENCRYPT_PROTOCOL_WPA3_TRANSITION:
            encrypt = "WPA2/WPA3";
            break;
        case WIFI_ENCRYPT_PROTOCOL_WPA3_ONLY:
            encrypt = "WPA3";
            break;
        default:
            encrypt = "";
            break;
    }
    switch (item->pairwise_cipher) {
        case WIFI_CIPHER_TKIP:
            cipher = "-TKIP";
            break;
        case WIFI_CIPHER_CCMP:
            cipher = "-CCMP";
            break;
        default:
            cipher = "";
            break;
    }
    AT_RSP("+CWLAP: %s,\t\t %d, "MAC_FMT", %d, %s%s\r\n",
        item->ssid.ssid, item->rssi, MAC_ARG(item->bssid_info.bssid),
        item->channel, encrypt, cipher);
}

/*!
    \brief      the AT command scan and show nearby AP information
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_ap_list(int argc, char **argv)
{
    if (argc <= 2) {
        if ((argc == 2) && (argv[1][0] == AT_QUESTION))
            goto Usage;

        if (wifi_management_scan(TRUE) != 0) {
            goto Error;
        }
        wifi_management_block_wait();
        if (p_wifi_netlink->scan_list_ready != 1)
            goto Error;

        if (argc == 1) {
            wifi_netlink_scan_list_get(at_scan_info_print);
        } else {
            int i;
            char *ssid = at_string_parse(argv[1]);

            if (ssid == NULL) {
                goto Error;
            }
            wifi_netlink_scan_list_get(NULL);
            for (i = 0; i < p_wifi_netlink->scan_ap_num; i++) {
                if (strcmp((char *)p_wifi_netlink->scan_list[i].ssid.ssid, ssid) == 0) {
                    at_scan_info_print(&p_wifi_netlink->scan_list[i]);
                }
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
    AT_RSP("+CWLAP[=ssid]\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command set wifi operation mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_mode_cur(int argc, char **argv)
{
    uint32_t mode;
    char *endptr = NULL;

    if (argc == 1) {
        AT_RSP("+CWMODE_CUR:%d\r\n", p_wifi_netlink->ap_started ? 2 : 1);
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            mode = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
            if ((*endptr != '\0') ||(mode > 2) || (mode < 1)) {
                goto Error;
            }
            if (mode == 2)
                wifi_management_ap_start("Gigadevice", "GDSZ@2021", 11, 0);
            else
                wifi_management_sta_start();
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
    AT_RSP("+CWMODE_CUR: <mode:1-2>\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command show wifi status
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_status(int argc, char **argv)
{
    struct wifi_connect_result *conn = &p_wifi_netlink->connected_ap_info;

    if (argc == 1) {
        if (p_wifi_netlink->ap_started) {
            AT_RSP("+CWSTATUS: SoftAP, %s, %s, %d\r\n",
                p_wifi_netlink->ap_conf.ssid, p_wifi_netlink->ap_conf.password, p_wifi_netlink->ap_conf.channel);
        } else {
            AT_RSP("+CWSTATUS: STA, ");
            if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_LINKED_CONFIGED)
                AT_RSP("connected, %s, %d, "MAC_FMT"\r\n",
                    conn->ssid.ssid, conn->channel, MAC_ARG(conn->bssid_info.bssid));
            else
                AT_RSP("disconnected\r\n");
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
    \brief      the AT command disconnect WiFi from AP
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_ap_quit(int argc, char **argv)
{
    if (argc == 1) {
        wifi_management_disconnect();
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
    \brief      the AT command start softAP
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_ap_cur_start(int argc, char **argv)
{
    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            goto Error;
        }
    } else if (argc == 5) {
        char *ssid = at_string_parse(argv[1]);
        char *pwd = at_string_parse(argv[2]);
        uint32_t chl, hidden;
        char *endptr = NULL;
        if ((ssid == NULL) || (pwd == NULL)) {
            goto Error;
        }

        chl = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if ((*endptr != '\0') || (chl > 13) || (chl < 1)) {
            goto Error;
        }
        hidden = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
        if ((*endptr != '\0') || (hidden > 1)) {
            goto Error;
        }
        wifi_management_ap_start(ssid, pwd, chl, hidden);
    } else {
        goto Error;
    }

    at_rsp_ok();
    return;

Error:
    at_rsp_error();
    return;
Usage:
    AT_RSP("+CWSAP_CUR: <ssid>,<pwd>,<chl:1-13>,<hidden:0-1>\r\n");
    at_rsp_ok();
    return;
}

/*!
    \brief      the AT command show station information that has been connected to the softAP
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_ap_client_list(int argc, char **argv)
{
    if (argc == 1) {
        if (p_wifi_netlink->ap_started) {
            uint8_t info[MAX_STATION_NUM * ETH_ALEN];
            uint32_t client_num, i;
            client_num = wifi_management_ap_assoc_info(info);
            for (i = 0; i < client_num; i++) {
                AT_RSP("+CWLIF: [%d] "MAC_FMT"\r\n", i, MAC_ARG(info + i * ETH_ALEN));
            }
#if 0
            while (p != '\0') {
                q = strchr(p, '@');
                *q++ = '\0'
                AT_RSP("+APLIF: [%d] %s\r\n", count, addr);
                p = addr = q;
                count++;
            }
#endif
        } else {
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
}

/*!
    \brief      the AT command configure whether to connect AP automatically after power on
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_cw_auto_connect(int argc, char **argv)
{
    char *endptr = NULL;
    uint8_t auto_conn;

    if (argc == 1) {
        auto_conn = wifi_netlink_auto_conn_get();
        AT_RSP("+CWAUTOCONN: %d\r\n", auto_conn ? 1 : 0);
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            auto_conn = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
            if ((*endptr != '\0') || (auto_conn > 1)) {
                goto Error;
            }
            auto_conn = auto_conn ? 1 : 0;
            wifi_netlink_auto_conn_set(auto_conn);
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
    AT_RSP("+CWAUTOCONN:(0-1)\r\n");
    at_rsp_ok();
    return;
}