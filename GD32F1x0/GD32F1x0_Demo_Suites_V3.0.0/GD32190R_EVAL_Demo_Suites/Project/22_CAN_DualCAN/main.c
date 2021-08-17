/*!
    \file  main.c
    \brief dual CAN communication in normal mode

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32f1x0.h"
#include <stdio.h>
#include "systick.h"
#include "gd32f190r_eval.h"

/* select CAN baudrate */
/* 1MBps */
#define CAN_BAUDRATE  1000
/* 500KBps */
/* #define CAN_BAUDRATE  500 */
/* 250KBps */
/* #define CAN_BAUDRATE  250 */
/* 125KBps */
/* #define CAN_BAUDRATE  125 */
/* 100KBps */ 
/* #define CAN_BAUDRATE  100 */
/* 50KBps */ 
/* #define CAN_BAUDRATE  50 */
/* 20KBps */ 
/* #define CAN_BAUDRATE  20 */

uint8_t can0_receive_flag, can1_receive_flag;
uint16_t can0_val_tx, can0_val_rx, can1_val_tx, can1_val_rx;
can_trasnmit_message_struct     transmit_message;
can_receive_message_struct      receive_message;
can_parameter_struct            can_init_parameter;
can_filter_parameter_struct     can_filter_parameter;

void gpio_config(void);
void nvic_config(void);
void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter);
void can_write_data(uint32_t can_periph, can_trasnmit_message_struct *tx_message);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t n=0;
    
    can0_receive_flag = DISABLE;
    can1_receive_flag = DISABLE;
    can0_val_tx = 0, can0_val_rx = 0, can1_val_tx = 0, can1_val_rx = 0;
    
    /* configure systick */
    systick_config();
    /* configure GPIO */
    gpio_config();
    /* configure NVIC */
    nvic_config();
    /* configure USART */ 
    gd_eval_com_init(EVAL_COM2);
    
    printf("\r\nGD32F1x0 Dual CAN\r\n");

#if CAN_BAUDRATE == 1000 
    printf("BAUDRATE = 1MBps   ");
#elif CAN_BAUDRATE == 500 
    printf("BAUDRATE = 500kBps   ");
#elif CAN_BAUDRATE == 250 
    printf("BAUDRATE = 250kBps   ");
#elif CAN_BAUDRATE == 125 
    printf("BAUDRATE = 125kBps   ");
#elif  CAN_BAUDRATE == 100 
    printf("BAUDRATE = 100kBps   ");
#elif  CAN_BAUDRATE == 50 
    printf("BAUDRATE = 50kBps   ");
#elif  CAN_BAUDRATE == 20
    printf("BAUDRATE = 20kBps   ");
#endif

    /* configure CAN */
    can_config(can_init_parameter, can_filter_parameter);
    /* enable phy */
    can_phy_enable(CAN0);
    
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);
    can_interrupt_enable(CAN1, CAN_INT_RFNE0);
    /* initialiize transmit_message */
    transmit_message.tx_sfid = 0x05A5;
    transmit_message.tx_efid = 0x00;
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
    transmit_message.tx_dlen = 2;
    
    /* infinite loop */
    while(1){
        can0_val_tx = n;
        transmit_message.tx_data[0] = can0_val_tx>>8;
        transmit_message.tx_data[1] = can0_val_tx;
        /* CAN0 transmit message */
        can_message_transmit(CAN0, &transmit_message);
        /* delay 1s */
        delay_1ms(1000);
        /* CAN0 receive completed */
        if(ENABLE == can0_receive_flag){
            can0_receive_flag = DISABLE;
            printf("CAN0 receive data: %d \r\n\r\n",can0_val_rx);
        }
        /* CAN1 receive completed */
        if(ENABLE == can1_receive_flag){
            can1_receive_flag = DISABLE;
            transmit_message.tx_data[0] = can1_val_tx>>8;
            transmit_message.tx_data[1] = can1_val_tx;
            /* CAN1 transmit message */
            can_message_transmit(CAN1, &transmit_message);
            printf("CAN1 receive data: %d \r\n", can1_val_rx);
        }
        n++;
        if(n==1000){
            n=0;
        }
    }
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIO clocks */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure CAN0 pins */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_5);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_5);

    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_6);

    /* configure CAN1 rx pin */ 
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_12);;
  
    /* configure CAN1 tx pin */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_13);
}

/*!
    \brief      initialize CAN and filter
    \param[in]  can_parameter
      \arg        can_parameter_struct
    \param[in]  can_filter
      \arg        can_filter_parameter_struct
    \param[out] none
    \retval     none
*/
void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    
    /* deinitialize CAN register */
    can_deinit(CAN0);
    can_deinit(CAN1);
    
    /* initialize CAN */
    can_parameter.time_triggered=DISABLE;
    can_parameter.auto_bus_off_recovery=DISABLE;
    can_parameter.auto_wake_up=DISABLE;
    can_parameter.no_auto_retrans=DISABLE;
    can_parameter.rec_fifo_overwrite=DISABLE;
    can_parameter.trans_fifo_order=DISABLE;
    can_parameter.working_mode=CAN_NORMAL_MODE;
    can_parameter.resync_jump_width=CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1=CAN_BT_BS1_3TQ;
    can_parameter.time_segment_2=CAN_BT_BS2_2TQ;
    
    /* 1MBps */
#if CAN_BAUDRATE == 1000
    can_parameter.prescaler =12;
    /* 500KBps */
#elif CAN_BAUDRATE == 500 
    can_parameter.prescaler =24;
    /* 250KBps */
#elif CAN_BAUDRATE == 250 
    can_parameter.prescaler =48;
    /* 125KBps */
#elif CAN_BAUDRATE == 125 
    can_parameter.prescaler =96;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100 
    can_parameter.prescaler =120;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50 
    can_parameter.prescaler =240;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20 
    can_parameter.prescaler =600;
#else
    #error "please select list can baudrate in private defines in main.c "
#endif  
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    can_init(CAN1, &can_parameter);
    
    /* initialize filter */ 
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = (((uint32_t)0x5A5<<21)&0xffff0000)>>16;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0xffff;
    can_filter.filter_mask_low = 0xffff;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable=ENABLE;
    
    can_filter_init(&can_filter);
    
    /* CAN1 filter number */
    can_filter.filter_number=15;
    can_filter_init(&can_filter);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn,0,0);

    /* configure CAN1 NVIC */
    nvic_irq_enable(CAN1_RX0_IRQn,1,1);
}

/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM2, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM2, USART_FLAG_TC));
    return ch;
}
