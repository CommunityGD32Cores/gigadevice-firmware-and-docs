/*!
    \file  gd32f207i_sdram_eval.c
    \brief exmc sdram(MICRON 48LC16M16A2) driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "stdio.h"
#include "gd32f207i_sdram_eval.h"

/* Define mode register content */
/* Burst Length */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0003)

/* Burst Type */
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)

/* CAS Latency */
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)

/* Write Mode */
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)

#define SDRAM_TIMEOUT                            ((uint32_t)0x0000FFFF)
#define SDRAM_DEVICE0_ADDR                       ((uint32_t)0xC0000000)
#define SDRAM_DEVICE1_ADDR                       ((uint32_t)0xD0000000)

//#define USE_SYSTICK_DELAY                        /* select coarse(software delay) or fine(systick delay) delay when sdram initialization */

#ifdef USE_SYSTICK_DELAY
#include "systick.h"
#define DELAY_MS(x)                              delay_1ms(x)
#else
#define DELAY_MS(x)                              delay_1ms_coarse(x)
extern uint32_t SystemCoreClock;
/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds 
    \param[out] none
    \retval     none
*/
static void delay_1ms_coarse(uint32_t count)
{
    uint32_t delay;
    
    delay = (count * (SystemCoreClock / 1000)) / 3;
    
    while(delay != 0){
        delay--;
    }
}
#endif /* USE_SYSTICK_DELAY */

/*!
    \brief      sdram peripheral initialize
    \param[in]  sdram_device: specifie the SDRAM device 
    \param[out] none
    \retval     none
*/
void exmc_synchronous_dynamic_ram_init(uint32_t sdram_device)
{
    exmc_sdram_parameter_struct        sdram_init_struct;
    exmc_sdram_timing_parameter_struct  sdram_timing_init_struct;
    exmc_sdram_command_parameter_struct     sdram_command_init_struct;

    uint32_t command_content = 0, bank_select;
    uint32_t timeout = SDRAM_TIMEOUT;

#ifdef USE_SYSTICK_DELAY
    /* systick clock configure */
    systick_config();
#endif /* USE_SYSTICK_DELAY */    
    /* enable EXMC clock*/
    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_AF);
    
    /* Common GPIO configuration */
    /* EXMC gpio pin PC0: EXMC_NWE remap */
    gpio_pin_remap1_config(GPIO_PCF5, GPIO_PCF5_EXMC_SDNWE_PC0_REMAP, ENABLE);
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    /* D0(PD14),D1(PD15),D2(PD0),D3(PD1),D13(PD8),D14(PD9),D15(PD10) pin configuration */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8 | GPIO_PIN_9 |
                                                         GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);
    
    /* GPIOE configuration */
    /* NBL0(PE0),NBL1(PE1),D4(PE7),D5(PE8),D6(PE9),D7(PE10),D8(PE11),D9(PE12),D10(PE13),D11(PE14),D12(PE15) pin configuration */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8 |
                                                         GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                                                         GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* GPIOF configuration */
    /* A0(PF0),A1(PF1),A2(PF2),A3(PF3),A4(PF4),A5(PF5),A6(PF12),A7(PF13),A8(PF14),A9(PF15),SDNRAS(PF11) pin configuration */
    gpio_init(GPIOF, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                                                         GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_11 | GPIO_PIN_12 |
                                                         GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* GPIOG configuration */
    /* A10(PG0),A11(PG1),A12(PG2),BA0(PG4),BA1(PG5),SDCLK(PG8),SDNCAS(PG15) pin configuration */
    gpio_init(GPIOG, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | 
                                                         GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15);
    
    /* GPIOH configuration */
    /* SDCKE0(PH2), SDNE0(PH3) pin configuration */
    gpio_init(GPIOH, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3);
    
    /* specify which SDRAM to read and write */
    if(EXMC_SDRAM_DEVICE0 == sdram_device){
        bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    }else{
        bank_select = EXMC_SDRAM_DEVICE1_SELECT;
    }

    /* EXMC SDRAM device initialization sequence --------------------------------*/
    /* Step 1 : configure SDRAM timing registers --------------------------------*/
    /* LMRD: 2 clock cycles */
    sdram_timing_init_struct.load_mode_register_delay = 2;
    /* XSRD: min = 67ns */
    sdram_timing_init_struct.exit_selfrefresh_delay = 2; 
    /* RASD: min=42ns , max=120k (ns) */
    sdram_timing_init_struct.row_address_select_delay = 2;
    /* ARFD: min=60ns */
    sdram_timing_init_struct.auto_refresh_delay = 2;
    /* WRD:  min=1 Clock cycles +6ns */
    sdram_timing_init_struct.write_recovery_delay = 1;
    /* RPD:  min=18ns */
    sdram_timing_init_struct.row_precharge_delay = 1;
    /* RCD:  min=18ns */
    sdram_timing_init_struct.row_to_column_delay = 1;

    /* step 2 : configure SDRAM control registers ---------------------------------*/
    sdram_init_struct.sdram_device = sdram_device;
    sdram_init_struct.column_address_width = EXMC_SDRAM_COW_ADDRESS_9;
    sdram_init_struct.row_address_width = EXMC_SDRAM_ROW_ADDRESS_13;
    sdram_init_struct.data_width = EXMC_SDRAM_DATABUS_WIDTH_16B;
    sdram_init_struct.internal_bank_number = EXMC_SDRAM_4_INTER_BANK;
    sdram_init_struct.cas_latency = EXMC_CAS_LATENCY_3_SDCLK; 
    sdram_init_struct.write_protection = DISABLE;
    sdram_init_struct.sdclock_config = EXMC_SDCLK_PERIODS_2_HCLK;  
    sdram_init_struct.brust_read_switch = ENABLE;
    sdram_init_struct.pipeline_read_delay = EXMC_PIPELINE_DELAY_1_HCLK;
    sdram_init_struct.timing  = &sdram_timing_init_struct;
    /* EXMC SDRAM bank initialization */
    exmc_sdram_init(&sdram_init_struct);

    /* step 3 : configure CKE high command---------------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_CLOCK_ENABLE;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */ 
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 4 : insert 10ms delay----------------------------------------------*/
    DELAY_MS(10);

    /* step 5 : configure precharge all command----------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_PRECHARGE_ALL;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 6 : configure Auto-Refresh command-----------------------------------*/
    sdram_command_init_struct.command = EXMC_SDRAM_AUTO_REFRESH;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_8_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */ 
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 7 : configure load mode register command-----------------------------*/
    /* program mode register */
    command_content = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1        |
                                SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                                SDRAM_MODEREG_CAS_LATENCY_3           |
                                SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                                SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    sdram_command_init_struct.command = EXMC_SDRAM_LOAD_MODE_REGISTER;
    sdram_command_init_struct.bank_select = bank_select;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = command_content;

    /* wait until the SDRAM controller is ready */ 
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);

    /* step 8 : set the auto-refresh rate counter--------------------------------*/
    /* 64ms, 8192-cycle refresh, 64ms/8192=7.81us */
    /* SDCLK_Freq = SYS_Freq/2 */
    /* (7.81 us * SDCLK_Freq) - 20 */
    exmc_sdram_refresh_count_set(448);

    /* wait until the SDRAM controller is ready */ 
    timeout = SDRAM_TIMEOUT; 
    while((exmc_flag_get(sdram_device, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0)){
        timeout--;
    }
}

/*!
    \brief      fill the buffer with specified value
    \param[in]  pbuffer: pointer on the buffer to fill
    \param[in]  buffersize: size of the buffer to fill
    \param[in]  value: value to fill on the buffer
    \param[out] none
    \retval     none
*/
void fill_buffer(uint8_t *pbuffer, uint16_t buffer_lengh, uint16_t offset)
{
    uint16_t index = 0;

    /* put in global buffer same values */
    for (index = 0; index < buffer_lengh; index++ ){
        pbuffer[index] = index + offset;
    }
}

/*!
    \brief      write a byte buffer(data is 8 bits) to the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  writeaddr: SDRAM memory internal address from which the data will be written
    \param[in]  numbyte_write: number of bytes to write
    \param[out] none
    \retval     none
*/
void sdram_writebuffer_8(uint32_t sdram_device, uint8_t* pbuffer, uint32_t writeaddr, uint32_t numbyte_write)
{
    uint32_t temp_addr;
    
    /* Select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0){
        temp_addr = SDRAM_DEVICE0_ADDR;
    }else{
        temp_addr = SDRAM_DEVICE1_ADDR;
    }
    
     /* While there is data to write */
    for(; numbyte_write != 0; numbyte_write--) {
        /* Transfer data to the memory */
        *(uint8_t *)(temp_addr + writeaddr) = *pbuffer++;

        /* Increment the address*/  
        writeaddr += 1;
    }
}

/*!
    \brief      read a block of 8-bit data from the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  readaddr: SDRAM memory internal address to read from
    \param[in]  numbyte_read: number of bytes to read
    \param[out] none
    \retval     none
*/
void sdram_readbuffer_8(uint32_t sdram_device, uint8_t* pbuffer, uint32_t readaddr, uint32_t numbyte_read)
{
    uint32_t temp_addr;
    
    /* select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0){
        temp_addr = SDRAM_DEVICE0_ADDR;
    }else{
        temp_addr = SDRAM_DEVICE1_ADDR;
    }
    
    /* while there is data to read */
    for(; numbyte_read != 0; numbyte_read--){
        /* read a byte from the memory */
        *pbuffer++ = *(uint8_t*)(temp_addr + readaddr);
    
        /* increment the address */
        readaddr += 1;
    }
}

/*!
    \brief      write a Half-word buffer(data is 16 bits) to the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  writeaddr: SDRAM memory internal address from which the data will be written
    \param[in]  numhalfbyte_write: number of half-words to write
    \param[out] none
    \retval     none
*/
void sdram_writebuffer_16(uint32_t sdram_device, uint16_t* pbuffer, uint32_t writeaddr, uint32_t numhalfbyte_write)
{
    uint32_t temp_addr;
    
    /* Select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0){
        temp_addr = SDRAM_DEVICE0_ADDR;
    }else{
        temp_addr = SDRAM_DEVICE1_ADDR;
    }
    
     /* While there is data to write */
    for(; numhalfbyte_write != 0; numhalfbyte_write--) {
        /* Transfer data to the memory */
        *(uint16_t *)(temp_addr + writeaddr) = *pbuffer++;

        /* Increment the address*/  
        writeaddr += 2;
    }
}

/*!
    \brief      read a block of 16-bit data from the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  readaddr: SDRAM memory internal address to read from
    \param[in]  numhalfbyte_read: number of half-word to read
    \param[out] none
    \retval     none
*/
void sdram_readbuffer_16(uint32_t sdram_device, uint16_t* pbuffer, uint32_t readaddr, uint32_t numhalfbyte_read)
{
    uint32_t temp_addr;
    
    /* select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0){
        temp_addr = SDRAM_DEVICE0_ADDR;
    }else{
        temp_addr = SDRAM_DEVICE1_ADDR;
    }
    
    /* while there is data to read */
    for(; numhalfbyte_read != 0; numhalfbyte_read--){
        /* read a half-word from the memory */
        *pbuffer++ = *(uint16_t*)(temp_addr + readaddr);
    
        /* increment the address */
        readaddr += 2;
    }
}

/*!
    \brief      write a word buffer(data is 32 bits) to the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  writeaddr: SDRAM memory internal address from which the data will be written
    \param[in]  numword_write: number of words to write
    \param[out] none
    \retval     none
*/
void sdram_writebuffer_32(uint32_t sdram_device, uint32_t* pbuffer, uint32_t writeaddr, uint32_t numword_write)
{
    uint32_t temp_addr;
    
    /* Select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0){
        temp_addr = SDRAM_DEVICE0_ADDR;
    }else{
        temp_addr = SDRAM_DEVICE1_ADDR;
    }
    
     /* While there is data to write */
    for(; numword_write != 0; numword_write--) {
        /* Transfer data to the memory */
        *(uint32_t *)(temp_addr + writeaddr) = *pbuffer++;

        /* Increment the address*/  
        writeaddr += 4;
    }
}

/*!
    \brief      read a block of 32-bit data from the EXMC SDRAM memory
    \param[in]  sdram_device: specify which a SDRAM memory block is written
    \param[in]  pbuffer: pointer to buffer
    \param[in]  readaddr: SDRAM memory internal address to read from
    \param[in]  numword_read: number of word to read
    \param[out] none
    \retval     none
*/
void sdram_readbuffer_32(uint32_t sdram_device, uint32_t* pbuffer, uint32_t readaddr, uint32_t numword_read)
{
    uint32_t temp_addr;
    
    /* select the base address according to EXMC_Bank */
    if(sdram_device == EXMC_SDRAM_DEVICE0){
        temp_addr = SDRAM_DEVICE0_ADDR;
    }else{
        temp_addr = SDRAM_DEVICE1_ADDR;
    }
    
    /* while there is data to read */
    for(; numword_read != 0; numword_read--){
        /* read a word from the memory */
        *pbuffer++ = *(uint32_t*)(temp_addr + readaddr);
    
        /* increment the address */
        readaddr += 4;
    }
}
