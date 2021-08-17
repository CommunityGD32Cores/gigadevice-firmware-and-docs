/*!
    \file  exmc_sram.c
    \brief EXMC SRAM(ISSI IS61LV51216) driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "gd32f30x.h"
#include "exmc_sram.h"

#define Bank0_SRAM2_ADDR            ((uint32_t)0x68000000)
#define BANK_SRAM_ADDR              Bank0_SRAM2_ADDR

/*!
    \brief      SRAM peripheral initialize
    \param[in]  none 
    \param[out] none
    \retval     none
*/
void exmc_sram_init(void)
{
    exmc_norsram_parameter_struct nor_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_init_struct;

    /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);

    /* EXMC enable */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure EXMC_D[0~15]*/
    /* PD14(EXMC_D0), PD15(EXMC_D1),PD0(EXMC_D2), PD1(EXMC_D3), PD8(EXMC_D13), PD9(EXMC_D14), PD10(EXMC_D15) */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1| GPIO_PIN_8 | GPIO_PIN_9 |
                                                         GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);

    /* PE7(EXMC_D4), PE8(EXMC_D5), PE9(EXMC_D6), PE10(EXMC_D7), PE11(EXMC_D8), PE12(EXMC_D9), 
       PE13(EXMC_D10), PE14(EXMC_D11), PE15(EXMC_D12) */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | 
                                                         GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | 
                                                         GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* configure EXMC_A[0~18]*/
    /* PF0(EXMC_A0), PF1(EXMC_A1), PF2(EXMC_A2), PF3(EXMC_A3), PF4(EXMC_A4), PF5(EXMC_A5), 
       PF12(EXMC_A6), PF13(EXMC_A7), PF14(EXMC_A8), PF15(EXMC_A9) */
    gpio_init(GPIOF, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                                                         GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 | 
                                                         GPIO_PIN_14 | GPIO_PIN_15);

    /* PG0(EXMC_A10), PG1(EXMC_A11), PG2(EXMC_A12), PG3(EXMC_A13), PG4(EXMC_A14), PG5(EXMC_A15) */
    gpio_init(GPIOG, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                                                         GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);       
    /* PD11(EXMC_A16), PD12(EXMC_A17), PD13(EXMC_A18) */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);


    /* configure NOE and NWE */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);


    /* configure NBL0-1 */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

    /* configure EXMC NE2 */
    gpio_init(GPIOG, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* configure timing parameter */
    nor_timing_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;
    nor_timing_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    nor_timing_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_DISABLE;
    nor_timing_init_struct.bus_latency = 1;
    nor_timing_init_struct.asyn_data_setuptime = 7;
    nor_timing_init_struct.asyn_address_holdtime = 2;
    nor_timing_init_struct.asyn_address_setuptime = 5;

    /* configure EXMC bus parameters */
    nor_init_struct.norsram_region = EXMC_BANK0_NORSRAM_REGION2;
    nor_init_struct.write_mode = EXMC_ASYN_WRITE;
    nor_init_struct.extended_mode = DISABLE;
    nor_init_struct.asyn_wait = DISABLE;
    nor_init_struct.nwait_signal = DISABLE;
    nor_init_struct.memory_write = ENABLE;
    nor_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    nor_init_struct.wrap_burst_mode = DISABLE;
    nor_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    nor_init_struct.burst_mode = DISABLE;
    nor_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    nor_init_struct.memory_type = EXMC_MEMORY_TYPE_SRAM;
    nor_init_struct.address_data_mux = DISABLE;
    nor_init_struct.read_write_timing = &nor_timing_init_struct;
    nor_init_struct.write_timing = &nor_timing_init_struct;
    exmc_norsram_init(&nor_init_struct);

    /* enable the EXMC bank0 NORSRAM */
    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION2);
}

/*!
    \brief      write a Half-word buffer(data is 16 bits) to the EXMC SRAM memory
    \param[in]  pbuffer : pointer to buffer
    \param[in]  writeaddr : SRAM memory internal address from which the data will be written
    \param[in]  num_halfword_write : number of half-words to write
    \param[out] none
    \retval     none
*/
void exmc_sram_writebuffer_16(uint16_t* pbuffer, uint32_t writeaddr, uint32_t num_halfword_write)
{
    /* while there is data to write */
    for(;num_halfword_write != 0;num_halfword_write--){
        /* transfer data to the memory */
        *(uint16_t *) (BANK_SRAM_ADDR + writeaddr) = *pbuffer++;

        /* increment the address */
        writeaddr += 2;
    }   
}

/*!
    \brief      read a block of 16-bit data from the EXMC SRAM memory
    \param[in]  pbuffer : pointer to the buffer that receives the data read from the SRAM memory
    \param[in]  readaddr : SRAM memory internal address to read from
    \param[in]  num_halfword_read : number of half-words to read
    \param[out] none
    \retval     none
*/
void exmc_sram_readbuffer_16(uint16_t* pbuffer, uint32_t readaddr, uint32_t num_halfword_read)
{
    /* while there is data to read */
    for(; num_halfword_read != 0; num_halfword_read--) {
        /* read a half-word from the memory */
        *pbuffer++ = *(uint16_t*) (BANK_SRAM_ADDR + readaddr);
    
        /* increment the address */
        readaddr += 2;
    }  
}

/*!
    \brief      write a word buffer(data is 32 bits) to the EXMC SRAM memory
    \param[in]  pbuffer : pointer to buffer
    \param[in]  writeaddr : SRAM memory internal address from which the data will be written
    \param[in]  num_word_write : number of words to write
    \param[out] none
    \retval     none
*/
void exmc_sram_writebuffer_32(uint32_t* pbuffer, uint32_t writeaddr, uint32_t num_word_write)
{
    /* while there is data to write */
    for(; num_word_write != 0; num_word_write--){
        /* transfer data to the memory */
        *(uint32_t *)(BANK_SRAM_ADDR + writeaddr) = *pbuffer++;

        /* increment the address */  
        writeaddr += 4;
    }
}

/*!
    \brief      read a block of 32-bit data from the EXMC SRAM memory
    \param[in]  pbuffer : pointer to the buffer that receives the data read from the SRAM memory
    \param[in]  readaddr : SRAM memory internal address to read from
    \param[in]  num_word_read : number of words to read
    \param[out] none
    \retval     none
*/
void exmc_sram_readbuffer_32(uint32_t* pbuffer, uint32_t readaddr, uint32_t num_word_read)
{
    /* while there is data to read */
    for(; num_word_read != 0; num_word_read--){
        /* read a word from the memory */
        *pbuffer++ = *(uint32_t*) (BANK_SRAM_ADDR + readaddr);
    
        /* increment the address */
        readaddr += 4;
    }
}

/*!
    \brief      write a Byte buffer(data is 8 bits ) to the EXMC SRAM memory
    \param[in]  pbuffer : pointer to buffer
    \param[in]  writeaddr : SRAM memory internal address from which the data will be written
    \param[in]  num_byte_write : number of bytes to write
    \param[out] none
    \retval     none
*/
void exmc_sram_writebuffer_8(uint8_t* pbuffer, uint32_t writeaddr, uint32_t num_byte_write)
{
    /* while there is data to write */
    for(; num_byte_write != 0; num_byte_write--){
        /* transfer data to the memory */
        *(uint8_t *) (BANK_SRAM_ADDR + writeaddr) = *pbuffer++;

        /* increment the address*/
        writeaddr += 1;
    }
}

/*!
    \brief      read a block of 8-bit data from the EXMC SRAM memory
    \param[in]  pbuffer : pointer to the buffer that receives the data read from the SRAM memory
    \param[in]  readaddr : SRAM memory internal address to read from
    \param[in]  num_byte_read : number of bytes to write
    \param[out] none
    \retval     none
*/
void exmc_sram_readbuffer_8(uint8_t* pbuffer, uint32_t readaddr, uint32_t num_byte_read)
{
    /* while there is data to read */
    for(; num_byte_read != 0; num_byte_read--){
        /* read a byte from the memory */
        *pbuffer++ = *(uint8_t*) (BANK_SRAM_ADDR + readaddr);

        /* increment the address */
        readaddr += 1;
    }
}

/*!
    \brief      fill the 16-bit buffer with specified value
    \param[in]  pbuffer: pointer on the buffer to fill
    \param[in]  buffersize: size of the buffer to fill
    \param[in]  offset: first value to fill on the buffer
    \param[out] none
    \retval     none
*/
void fill_buffer_16(uint16_t *pbuffer, uint16_t buffer_lenght, uint16_t offset)
{
    uint16_t index = 0;

    for(index = 0; index < buffer_lenght; index++ ){
        pbuffer[index] = index + offset;
    }
}
