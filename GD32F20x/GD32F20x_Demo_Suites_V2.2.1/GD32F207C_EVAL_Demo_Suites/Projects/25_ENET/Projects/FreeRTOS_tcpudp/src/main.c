/*!
    \file  main.c
    \brief enet demo 

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

#include "gd32f20x.h"
#include "netconf.h"
#include "main.h"
#include "lwip/tcp.h"
#include "gd32f207c_eval.h"
#include "hello_gigadevice.h"
#include "tcp_client.h"
#include "udp_echo.h"

#define INIT_TASK_PRIO   ( tskIDLE_PRIORITY + 1 )
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 4 )
#define LED_TASK_PRIO    ( tskIDLE_PRIORITY + 2 )


extern struct netif g_mynetif;
 
void led_task(void * pvParameters); 
void init_task(void * pvParameters);


__IO uint32_t g_localtime = 0; /* for creating a time reference incremented by 10ms */

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure 4 bits pre-emption priority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    /* init task */
    xTaskCreate(init_task, "INIT", configMINIMAL_STACK_SIZE * 2, NULL, INIT_TASK_PRIO, NULL);
    
    /* start scheduler */
    vTaskStartScheduler();

    while(1){
    }
}

/*!
    \brief      init task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void init_task(void * pvParameters)
{
    gd_eval_com_init(EVAL_COM0);
    gd_eval_led_init(LED3);
    
    /* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
    enet_system_setup();

    /* initilaize the LwIP stack */
    lwip_stack_init();

    /* initilaize the tcp server: telnet 8000 */
    hello_gigadevice_init();
    /* initilaize the tcp client: echo 1026 */
    tcp_client_init();
    /* initilaize the udp: echo 1025 */
    udp_echo_init();

#ifdef USE_DHCP
    /* start DHCP client */
    xTaskCreate(dhcp_task, "DHCP", configMINIMAL_STACK_SIZE * 2, NULL, DHCP_TASK_PRIO, NULL);
#endif /* USE_DHCP */

    /* start toogle LED task every 250ms */
    xTaskCreate(led_task, "LED", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);

    for( ;; ){
        vTaskDelete(NULL);
    }
}

/*!
    \brief      led task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void led_task(void * pvParameters)
{  
    for( ;; ){
        /* toggle LED3 each 250ms */
        gd_eval_led_toggle(LED3);
        vTaskDelay(250);
    }
}

/*!
    \brief      updates the system local time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_update(void)
{
    g_localtime += portTICK_PERIOD_MS;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
