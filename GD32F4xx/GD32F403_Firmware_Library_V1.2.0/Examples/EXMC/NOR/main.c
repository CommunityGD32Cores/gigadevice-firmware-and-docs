/*!
    \file  main.c
    \brief EXMC NOR Flash demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"
#include "exmc_norflash.h"

/* NOR */
#define BUFFER_SIZE               0x100
#define WRITE_READ_ADDR           0xfff200

nor_id_struct nor_id;
uint32_t writereadstatus = 0;
uint16_t writebuffer[BUFFER_SIZE];
uint16_t readbuffer[BUFFER_SIZE];
nor_status_struct status;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i=0;
    
    /* LED initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED4);
    /* config the USART */
    gd_eval_com_init(EVAL_COM1);
    /* config the EXMC access mode */
    exmc_norflash_init();
    /* read nor flash id and printf */
    nor_read_id(&nor_id);

    printf("\r\nread NOR ID");
    
    /* print NOR ID */
    printf("\r\nNOR flash ID:0x%X 0x%X 0x%X 0x%X\r\n",nor_id.manufacturer_code,nor_id.device_code1,
                                                      nor_id.device_code2,nor_id.device_code3);
    nor_return_to_read_mode();

    /* erase the nor flash block to be written data */
    status = nor_eraseblock(WRITE_READ_ADDR);
    if(NOR_SUCCESS == status){
        printf("\r\nerase nor flash block successfully!");
    }else{
        printf("\r\nerase nor flash block failure!");

        /* failure, light on LED4 */
        gd_eval_led_on(LED4);
        while(1);
    }

    /* whether address cross-border */
    if((WRITE_READ_ADDR + BUFFER_SIZE ) > NOR_MAX_ADDRESS){
        printf("\n\raddress cross-border\n\r");

        /* failure, light on LED4 */
        gd_eval_led_on(LED4);
        while(1);
    }

    /* fill writeBuffer with the specified value */
    nor_fill_buffer(writebuffer, BUFFER_SIZE, 0x0006);

    /* write data to nor flash, WRITE_READ_ADDR: the starting address of the write data */
    status = nor_write_buffer(writebuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    if(NOR_SUCCESS == status){
        printf("\r\nwrite data to nor flash block successfully!");
    }else{
        printf("\r\nwrite data to nor flash block failure!");

        /* failure, light on LED4 */
        gd_eval_led_on(LED4);
        while(1);
    }

    /* read data from nor flash, WRITE_READ_ADDR: the starting address of the read data*/
    nor_readbuffer(readbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    /* read and write data comparison for equality */
    writereadstatus = 0;
    for (i = 0x00; i < BUFFER_SIZE; i++){
        if (readbuffer[i] != writebuffer[i]){
            writereadstatus++;
            break;
        }
    }

    printf("\n\rthe result to access the nor flash:\n\r");
    if (writereadstatus == 0)
    {
         printf("\n\raccess nor flash successfully!\n\r");
         gd_eval_led_on(LED1);
    }
    else
    {
         printf("\n\raccess nor flash failure!\n\r"); 

        /* failure, light on LED4 */
        gd_eval_led_on(LED4);
        while(1);
    }

    printf("\n\rprintf data to be read: \n\r");
    printf("\n\r");
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%d ",readbuffer[i]);
    }

    while (1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM1,USART_FLAG_TBE));
    return ch;
}
