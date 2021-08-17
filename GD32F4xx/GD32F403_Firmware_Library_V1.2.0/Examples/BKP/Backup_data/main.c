/*!
    \file  main.c
    \brief backup data register
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"
#include "gd32f403_eval.h"

#define  BKP_DATA_REG_NUM              42

void led_config(void);
void write_backup_register(uint16_t data);
uint32_t check_backup_register(uint16_t data);

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
    /* PMU lock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* BKP clock enable */
    rcu_periph_clock_enable(RCU_BKPI);
    /* enable write access to the registers in backup domain */
    pmu_backup_write_enable();
    /* clear the bit flag of tamper event */
    bkp_flag_clear(BKP_FLAG_TAMPER);
    
    /* check if the POR/PDR reset flag is set */
    if(RESET != rcu_flag_get(RCU_FLAG_PORRST)){
        /* clear the RCU all reset flags */
        rcu_all_reset_flag_clear();
        /* turn on LED4 */
        gd_eval_led_on(LED3);
        
        /* check if backup data registers has been written */
        if(0x00 == check_backup_register(0x1226)){
            /* Backup data registers values are correct */
            /* turn on LED2 */
            gd_eval_led_on(LED1);
        }else{
            /* backup data registers values are not correct or they are not written*/
            /* write data to backup data registers */
            write_backup_register(0x1226);
            /* turn on LED3 */
            gd_eval_led_on(LED2);
        }
    }
    /* turn on LED5 */
    gd_eval_led_on(LED4);;
    while(1){
    }
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
    /* write data to backup registers */
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
    /* check the data of backup registers */
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
