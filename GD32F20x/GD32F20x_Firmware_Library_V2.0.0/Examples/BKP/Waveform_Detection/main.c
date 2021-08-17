/*!
    \file  main.c
    \brief Waveform_Detection demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "gd32f207i_eval.h"

#define BKP_DATA_REG_NUM              42
  
void led_config(void);
void rtc_config(void);
void nvic_config(void);
void write_backup_register(uint16_t data);
uint32_t check_backup_register(uint16_t data);
uint32_t is_backup_register_clear(void);
    
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{     
    /* led configuration and turn on all led */
    led_config();
    /* NVIC configuration */
    nvic_config();
    /* PMU clock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* BKP clock enable */
    rcu_periph_clock_enable(RCU_BKPI);
    
    /* enable write access to the registers in backup domain */
    pmu_backup_write_enable();
    
    /* configure the RTC clock */
    rtc_config();

    /* tamper detection disable  */
    bkp_tamper_detection_disable(TAMPER_1);

    /* disable the tamper interrupt */
    bkp_tamper_interrupt_disable(TAMPER_1);
    
    /* disable the tamper interrupt */
    bkp_tamper_interrupt_disable(TAMPER_1);
    /* clear the bit flag of tamper event */
    bkp_flag_clear(BKP_FLAG_TAMPER1_WAVEDETECT);
    /* enable the tamper_1/waveform detect interrupt */
    bkp_tamper_interrupt_enable(TAMPER_1);

    /* enable the waveform detection function */
    bkp_waveform_detect_enable(BKP_WAVEFORM_DETECT_1, ENABLE);

    /* write data to backup DATAx registers */
    write_backup_register(0x0715);
  
    /* check if the written data are correct */
    if(0x00 == check_backup_register(0x0715)){
        /* turn on LED1 */
        gd_eval_led_on(LED1);
    }else{
        /* turn on LED2 */
        gd_eval_led_on(LED2);
    }

    while (1){
    }
}

/*!
    \brief      configure led
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/*!
    \brief      configure RTC clock source and prescaler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_config(void)
{
    /* enable the LXTAL */
    rcu_osci_on(RCU_LXTAL);
    
    /* enable the LXTAL high driving capability */
    rcu_lxtal_drive_capability_config(RCU_LXTAL_HIGHDRI);

    /* wait until LXTAL is stable */
    while(RESET == rcu_osci_stab_wait(RCU_LXTAL)){
    }

    /* select LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* enable the RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);
    
    /* wait RTC registers synchronized */
    rtc_register_sync_wait();
    
    /* set the RTC time base to 1s */
    rtc_prescaler_set(32767);

    /* wait RTC last write operation finished */
    rtc_lwoff_wait();
}

/*!
    \brief      write data to backup DATAx registers
    \param[in]  data: the data to be written to backup data registers
      \arg        0x0000-0xFFFF
    \param[out] none
    \retval     none
*/
void write_backup_register(uint16_t data)
{
    uint32_t temp = 0;
    
    /* write data to backup data registers */
    for (temp = 0; temp < BKP_DATA_REG_NUM; temp++){
        if(temp < 10){
            BKP_DATA0_9(temp) = data + (temp * 0x50);
        }else{
            BKP_DATA10_41(temp) = data + (temp * 0x50);
        }
    }
}

/*!
    \brief      check if the backup DATAx registers values are correct or not
    \param[in]  data: the data to be written to backup data registers
      \arg        0x0000-0xFFFF
    \param[out] none
    \retval     the number of data register
*/
uint32_t check_backup_register(uint16_t data)
{
    uint32_t temp = 0;
    
    /* check the data of backup data registers */
    for(temp = 0; temp < BKP_DATA_REG_NUM; temp++){
        if(temp < 10){
            /* get data from data register 0-9 */
            if(data + (temp * 0x50) != BKP_DATA_GET(BKP_DATA0_9(temp))){
                return temp+1;
            }
        }else{
            /* get data from data register 10-41 */
            if(data + (temp * 0x50) != BKP_DATA_GET(BKP_DATA10_41(temp))){
                return temp+1;
            }
        }
    }
    return 0;
}

/*!
    \brief      check if the backup data registers are clear or not
    \param[in]  none
    \param[out] none
    \retval     the number of data register
*/
uint32_t is_backup_register_clear(void)
{
    uint32_t temp = 0;
    
    for(temp = 0; temp < BKP_DATA_REG_NUM; temp++){
        if(temp < 10){
            /* check if the data of data register 0-9 is 0x0000 */
            if(0x0000 != BKP_DATA_GET(BKP_DATA0_9(temp))){
                return temp+1;
            }
        }else{
            /* check if the data of data register 10-41 is 0x0000 */
            if(0x0000 != BKP_DATA_GET(BKP_DATA10_41(temp))){
                return temp+1;
            }
        }
    }
    return 0;
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(TAMPER_IRQn,0,0);
}

