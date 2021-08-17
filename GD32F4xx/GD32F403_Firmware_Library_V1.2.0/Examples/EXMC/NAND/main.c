/*!
    \file  main.c
    \brief NAND test demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"
#include "exmc_nandflash.h"

#define BUFFER_SIZE                 (0x100U)
#define NAND_HY_MAKERID             (0xADU)
#define NAND_HY_DEVICEID            (0xF1U)

nand_id_struct nand_id;
uint8_t txbuffer[BUFFER_SIZE], rxbuffer[BUFFER_SIZE];
__IO uint32_t writereadstatus = 0, status= 0;
uint32_t j = 0;
uint32_t k = 0;
uint32_t writereadaddr ;
uint16_t zone, block, page, pageoffset;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* LED initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED4);
    /* config the USART */
    gd_eval_com_init(EVAL_COM1);
    /* config the EXMC access mode */
    exmc_nandflash_init();
    /* read NAND ID */
    nand_read_id(&nand_id);

    printf("Read NAND ID");
    /* print NAND ID */
    printf("\r\nNand flash ID:0x%X 0x%X 0x%X 0x%X\r\n",nand_id.maker_id,nand_id.device_id,
                                               nand_id.third_id,nand_id.fourth_id);

    if((NAND_HY_MAKERID == nand_id.maker_id) && (NAND_HY_DEVICEID == nand_id.device_id))
    {
        /* set the read and write the address */
        zone = 0;
        block = 10;
        page = 0;
        pageoffset = 1100;
        writereadaddr = ((zone * NAND_ZONE_SIZE + block) * NAND_BLOCK_SIZE + page) 
                        * NAND_PAGE_SIZE + pageoffset;

        /* whether address cross-border */
        if((writereadaddr + BUFFER_SIZE ) > NAND_MAX_ADDRESS)
        {
            printf("\r\nAddress cross-border");

            /* failure, light on LED4 */
            gd_eval_led_on(LED4);
            while(1);
        }

        /* fill writebuffer with 0x00.. */
        fill_buffer_nand(txbuffer, BUFFER_SIZE , 0x00);

        /* write data to nand flash */
        status = nand_write(writereadaddr, txbuffer, BUFFER_SIZE);
        if(NAND_OK == status)
        {
            printf("\r\nWrite data successfully!");
        }
        else
        {
            printf("\r\nWrite data failure!");

            /* failure, light on LED4 */
            gd_eval_led_on(LED4);
            while(1);
        }

        /* read data from nand flash */
        status = nand_read(writereadaddr, rxbuffer, BUFFER_SIZE);
        if(NAND_OK == status)
        {
            printf("\r\nRead data successfully!");
        }
        else
        {
            printf("\r\nRead data failure!\n\r");

            /* failure, light on LED4 */
            gd_eval_led_on(LED4);
            while(1);
        }

        /* Read and write data comparison for equality */
        writereadstatus = 0;
        for(j = 0; j < BUFFER_SIZE; j++)
        {
            if(txbuffer[j] != rxbuffer[j])
            {
                writereadstatus++;
                break;
            }
        }

        printf("\r\nThe result to access the nand flash:");
        if (writereadstatus == 0)
        { 
            printf("\r\nAccess NAND flash successfully!");

            gd_eval_led_on(LED1);
        }
        else
        { 
            printf("\r\nAccess NAND flash failure!");
            
            /* failure, light on LED4 */
            gd_eval_led_on(LED4);
            while(1);
        }
        printf("\r\nPrintf data to be read:\r\n");
        for(k = 0; k < BUFFER_SIZE; k ++)
        {
            printf("0x%02X ",rxbuffer[k]);
        }
    }
    else
    {
        printf("\n\rRead NAND ID failure!\n\r");
        
        /* failure, light on LED4 */
        gd_eval_led_on(LED4);
        while(1);
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
