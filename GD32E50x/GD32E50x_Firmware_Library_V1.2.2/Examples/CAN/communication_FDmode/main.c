/*!
    \file    main.c
    \brief   dual CAN communication in FD mode
    
    \version 2021-03-23, V1.0.0, firmware for GD32E50x
	\version 2021-04-18, V1.0.1, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "gd32e50x.h"
#include <stdio.h>
#include "gd32e507z_eval.h"
#include "systick.h"

#define DEV_CAN0_ID          0xaabb
#define DEV_CAN0_MASK        0x0000
#define DEV_CAN_BAUD_RATE    500000

can_trasnmit_message_struct g_transmit_message;
can_receive_message_struct g_receive_message;
FlagStatus can0_receive_flag;
/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOD,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_0);
    gpio_init(GPIOD,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_1);

    gpio_pin_remap_config(GPIO_CAN0_FULL_REMAP, ENABLE);

}

/*!
    \brief      configure BSP
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_board_config(void)
{
    /* configure USART */
    gd_eval_com_init(EVAL_COM0);
    
    /* configure KEY_B key */
    gd_eval_key_init(KEY_B, KEY_MODE_GPIO);
    
    /* configure leds */
    gd_eval_led_init(LED2);
    gd_eval_led_off(LED2);

}

/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config(void)
{
    can_parameter_struct can_parameter;
    can_fdframe_struct can_fd_parameter; 
    can_fd_tdc_struct can_fd_tdc_parameter;
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    /* initialize CAN register */
    can_deinit(CAN0);
    
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = ENABLE;
    can_parameter.trans_fifo_order = ENABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;  
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
   
    /* config CAN0 baud rate */
    can_frequency_set(CAN0, DEV_CAN_BAUD_RATE);
    /* config CAN1 baud rate */
    
    can_struct_para_init(CAN_FD_FRAME_STRUCT, &can_fd_parameter);
    can_fd_parameter.fd_frame = ENABLE;
    can_fd_parameter.excp_event_detect = ENABLE;
    can_fd_parameter.delay_compensation = ENABLE;
    can_fd_tdc_parameter.tdc_filter = 0x04; 
    can_fd_tdc_parameter.tdc_mode = CAN_TDCMOD_CALC_AND_OFFSET;
    can_fd_tdc_parameter.tdc_offset = 0x04;
    can_fd_parameter.p_delay_compensation = &can_fd_tdc_parameter;
    can_fd_parameter.iso_bosch = CAN_FDMOD_ISO;
    can_fd_parameter.esi_mode = CAN_ESIMOD_HARDWARE;
    can_fd_init(CAN0, &can_fd_parameter);
    can_fd_init(CAN1, &can_fd_parameter);
    
    can_fd_frequency_set(CAN0, 1000000);
    
    /* initialize filter */ 
    can1_filter_start_bank(14);
    can_filter_mask_mode_init(CAN0,DEV_CAN0_ID, DEV_CAN0_MASK, CAN_EXTENDED_FIFO0, 0);
    
    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn, 0, 0);

    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);

}

void communication_check(void)
{
    uint32_t i;
    uint32_t addr;
    
    /* CAN0 receive data correctly, the received data is printed */
    if(SET == can0_receive_flag){
        can0_receive_flag = RESET;
        addr = (uint32_t)g_receive_message.rx_data;
        for(i = 0U; i < 3U; i++){
            printf("\r\n can0 receive data:%08x", *(uint32_t *)addr);
            addr += 4U;
        }
        
        gd_eval_led_toggle(LED2);
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{ 
    uint32_t i;
    uint32_t addr;
    
    /* systick configuration */
    systick_config();
    /* configure board */
    bsp_board_config();
    /* configure GPIO */
    can_gpio_config();
    /* initialize CAN and filter */
    can_config();
                             
    printf("\r\n FD communication test CAN0, please press KEY_B key to start! \r\n");
    
    /* initialize transmit message */
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &g_transmit_message);
    g_transmit_message.tx_sfid = 0x00;
    g_transmit_message.tx_efid = 0x00;
    g_transmit_message.tx_ft = CAN_FT_DATA;
    g_transmit_message.tx_ff = CAN_FF_EXTENDED;
    g_transmit_message.tx_dlen = 12;
    g_transmit_message.fd_flag = 1;
    g_transmit_message.fd_brs = 1;
    g_transmit_message.fd_esi = 0;

    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &g_receive_message);
    
    while(1){
        /* test whether the KEY_B key is pressed */
        if(0 == gd_eval_key_state_get(KEY_B)){
            delay_1ms(100);
            if(0 == gd_eval_key_state_get(KEY_B)){
                g_transmit_message.tx_efid = DEV_CAN0_ID;
                g_transmit_message.tx_data[0] = 0xaa;
                g_transmit_message.tx_data[1] = 0xbb;
                g_transmit_message.tx_data[2] = 0xcc;
                g_transmit_message.tx_data[3] = 0xdd;
                g_transmit_message.tx_data[4] = 0xee;
                g_transmit_message.tx_data[5] = 0xff;
                g_transmit_message.tx_data[6] = 0x00;
                g_transmit_message.tx_data[7] = 0x11;
                g_transmit_message.tx_data[8] = 0x22;
                g_transmit_message.tx_data[9] = 0x33;
                g_transmit_message.tx_data[10] = 0x44;
                g_transmit_message.tx_data[11] = 0x55;
                addr = (uint32_t)g_transmit_message.tx_data;
                for(i = 0U; i < 3U; i++){
                    printf("\r\n can0 transmit data:%08x", *(uint32_t *)addr);
                    addr += 4;
                }
            
                /* transmit message */
                can_message_transmit(CAN0, &g_transmit_message);
                /* waiting for the KEY_B key up */
                while(0 == gd_eval_key_state_get(KEY_B));
            }
        }
        communication_check();
    }
}

/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
