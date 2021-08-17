/*!
    \file  main.c
    \brief EXMC SRAM demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"
#include "exmc_sram.h"


#define BUFFER_SIZE             18                  /*!< write or read buffer size */
#define WRITE_READ_ADDR         0x0000              /*!< SRAM write or read address */

uint16_t txbuffer[BUFFER_SIZE];
uint16_t rxbuffer[BUFFER_SIZE];
uint16_t writereadstatus = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i = 0;

    /* LED initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED4);
    /* config the USART */
    gd_eval_com_init(EVAL_COM1);
    /* config the EXMC access mode */
    exmc_sram_init();
    /* fill txbuffer */
    fill_buffer_16(txbuffer, BUFFER_SIZE, 0x1215);
    /* write data to SRAM  */
    exmc_sram_writebuffer_16(txbuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    /* read data from SRAM */
    exmc_sram_readbuffer_16(rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    /* compare two buffers */
    for(i = 0;i < BUFFER_SIZE;i++){
        if (rxbuffer[i] != txbuffer[i]){
            writereadstatus ++;
            break;
        }
    }

    if(writereadstatus){
        printf("\r\nSRAM test failed!");

        /* failure, light on LED4 */
        gd_eval_led_on(LED4);
    }else{
        printf("\r\nSRAM test successed!");
        printf("\r\nthe data is:\r\n");
        for(i=0;i < BUFFER_SIZE;i++){
            printf("%6x",rxbuffer[i]);
            if(((i+1)%6) == 0){
                printf("\r\n");
            }
        }
        /* success, light on LED1 */
        gd_eval_led_on(LED1);
    }

    while(1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM1,USART_FLAG_TBE));
    return ch;
}
