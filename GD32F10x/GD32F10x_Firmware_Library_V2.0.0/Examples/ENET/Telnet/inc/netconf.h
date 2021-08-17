/*!
    \file  netconf.h
    \brief the header file of netconf 
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef NETCONF_H
#define NETCONF_H

#include "main.h"

#ifdef USE_DHCP
void lwip_dhcp_process_handle(void);
#endif /* USE_DHCP */

void lwip_stack_init(void);
void lwip_pkt_handle(void);
void lwip_periodic_handle(__IO uint32_t localtime);

#endif /* NETCONF_H */
