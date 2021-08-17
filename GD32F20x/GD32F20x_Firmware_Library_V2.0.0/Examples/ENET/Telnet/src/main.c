/*!
    \file  main.c
    \brief enet demo 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "netconf.h"
#include "main.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "gd32f207i_eval.h"
#include "hello_gigadevice.h"

#define SYSTEMTICK_PERIOD_MS  10

__IO uint32_t g_localtime = 0;      /* for creating a time reference incremented by 10ms */
uint32_t timingdelay;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM1);

    /* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */ 
    enet_system_setup();

    /* initilaize the LwIP stack */
    lwip_stack_init();

    /* initilaize the helloGigadevice module telnet 23 */
    hello_gigadevice_init();

    while (1){
      
#ifndef USE_ENET_INTERRUPT      
        /* check if any packet received */
        if (enet_rxframe_size_get()){
            /* process received ethernet packet */
            lwip_pkt_handle();
        }        
#endif /* USE_ENET_INTERRUPT */
        
        /* handle periodic timers for LwIP */
#ifdef TIMEOUT_CHECK_USE_LWIP
        sys_check_timeouts();
        
#ifdef USE_DHCP
        lwip_dhcp_process_handle();
#endif /* USE_DHCP */ 
        
#else
        lwip_periodic_handle(g_localtime);
#endif /* TIMEOUT_CHECK_USE_LWIP */
    }   
}

/*!
    \brief      insert a delay time
    \param[in]  ncount: number of 10ms periods to wait for
    \param[out] none
    \param[out] none
*/
void delay_10ms(uint32_t ncount)
{
    /* capture the current local time */
    timingdelay = g_localtime + ncount;  

    /* wait until the desired delay finish */  
    while(timingdelay > g_localtime){
    }
}

/*!
    \brief      updates the system local time
    \param[in]  none
    \param[out] none
    \param[out] none
*/
void time_update(void)
{
    g_localtime += SYSTEMTICK_PERIOD_MS;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));
    return ch;
}
