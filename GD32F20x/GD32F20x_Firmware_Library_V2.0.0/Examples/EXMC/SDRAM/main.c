/*!
    \file  main.c
    \brief EXMC SDRAM demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "gd32f207i_eval.h"
#include "gd32f207i_sdram_eval.h"

/* SDRAM */
#define BUFFER_SIZE                ((uint32_t)0x0400)
#define WRITE_READ_ADDR            ((uint32_t)0x0000)
uint32_t writereadstatus = 0;
uint8_t txbuffer[BUFFER_SIZE];
uint8_t rxbuffer[BUFFER_SIZE];

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
    gd_eval_led_init(LED3);
    
    /* systick clock configure */
    systick_config();
    
    /* config the USART */
    gd_eval_com_init(EVAL_COM1);
    
    /* config the EXMC access mode */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);

    printf("\r\nSDRAM initialized!");
    delay_1ms(1000);
    
    /* fill txbuffer */
    fill_buffer(txbuffer, BUFFER_SIZE, 0x0000);
    memset(rxbuffer, 0, sizeof(rxbuffer)/sizeof(rxbuffer[0]));
    
    printf("\r\nSDRAM write data completed!");
    delay_1ms(1000);
    
    /* write data to SDRAM */
    sdram_writebuffer_8(EXMC_SDRAM_DEVICE0,txbuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    
    printf("\r\nSDRAM read data completed!");
    delay_1ms(1000);
    
    /* read data from SDRAM */
    sdram_readbuffer_8(EXMC_SDRAM_DEVICE0, rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    
    printf("\r\nCheck the data!");
    delay_1ms(1000);
    
    /* compare two buffers */
    for(i = 0; i < BUFFER_SIZE; i++){
        if (rxbuffer[i] != txbuffer[i]){
            writereadstatus ++;
            break;
        }
    }

    if(writereadstatus){
        printf("\r\nSDRAM test failed!");

        /* failure, light on LED3 */
        gd_eval_led_on(LED3);
    }else{
        printf("\r\nSDRAM test successed!");
        delay_1ms(1000);
        printf("\r\nThe data is:\r\n");
        delay_1ms(1000);
        for(i=0; i < BUFFER_SIZE; i++){
            printf("%02x  ", rxbuffer[i]);
            if(((i+1) % 16) == 0){
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
