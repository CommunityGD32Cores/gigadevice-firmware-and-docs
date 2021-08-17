#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "los_bsp_uart.h"
#include "gd32e103v_eval.h"

static char _buffer[128];
/******************************************************************************
    here include some special hearder file you need
******************************************************************************/



/*****************************************************************************
 Function    : LOS_EvbUartInit
 Description : Init uart device
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbUartInit(void)
{
    gd_eval_com_init(EVAL_COM0);
    /* Output a message on Hyperterminal using printf function */
    printf("\n\r LOS_EvbUartInit function initialize successfully \n\r");
}

/*****************************************************************************
 Function    : LOS_EvbUartWriteByte
 Description : Uart write one byte
 Input       : const char c
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbUartWriteByte(const char c)
{
    //add you code here.
    /* e.g. write a character to the USART */
    usart_data_transmit(EVAL_COM0, (uint8_t)c);
    /* Loop until transmit data register is empty */
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    
    return;
}

/*****************************************************************************
 Function    : LOS_EvbUartReadByte
 Description : Uart reaad one byte
 Input       : char* c
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbUartReadByte(char* c)
{
    //add you code here.
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE));
    *c = (usart_data_receive(EVAL_COM0));
}

/*****************************************************************************
 Function    : LosUartPrintf
 Description : Los printf function
 Input       : char* fmt, ...
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbUartPrintf(char* fmt, ...)
{
    //add you code here.
    int i;
    va_list ap;
    va_start(ap, fmt);
    vsprintf(_buffer, fmt, ap);
    va_end(ap);
    
    for (i = 0; _buffer[i] != '\0'; i++){
        LOS_EvbUartWriteByte(_buffer[i]);
    }
}

/*****************************************************************************
 Function    : LOS_EvbUartWriteStr
 Description : Uart Write String function
 Input       : const char* str
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbUartWriteStr(const char* str)
{
    //add you code here.
    while(*str){
        LOS_EvbUartWriteByte(*str++);
    }
}

#ifndef LOS_KERNEL_TEST_KEIL_SWSIMU
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口USART */
    LOS_EvbUartWriteByte((char)ch);

    return (ch);
}
#endif
