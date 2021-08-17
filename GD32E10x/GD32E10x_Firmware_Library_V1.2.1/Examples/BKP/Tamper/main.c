/*!
    \file    main.c
    \brief   tamper demo
    
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
    \version 2020-09-30, V1.1.0, firmware for GD32E10x
    \version 2020-12-31, V1.2.0, firmware for GD32E10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#include "gd32e10x.h"
#include "gd32e10x_eval.h"

#define DATA                          (0x1226)
#define BKP_DATA_REG_MIN              (BKP_DATA_0)
#define BKP_DATA_REG_MAX              (BKP_DATA_41)

void led_config(void);
void nvic_config(void);
void rcu_config(void);
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
    /* configure leds */
    led_config();
    /* NVIC configuration */
    nvic_config();
    /* enable the peripheral clock */
    rcu_config();
    
    /* enable write access to the registers in backup domain */
    pmu_backup_write_enable();
    /* tamper pin active level set */
    bkp_tamper_active_level_set(TAMPER_PIN_ACTIVE_LOW);
    /* tamper detection disable  */
    bkp_tamper_detection_disable();
    /* disable the tamper interrupt */
    bkp_interrupt_disable();
    /* clear the bit flag of tamper event */
    bkp_flag_clear();
    /* configure the tamper pin active on low level, and enable the tamper pin */
    bkp_interrupt_enable();
    /* tamper detection enable  */
    bkp_tamper_detection_enable();
    /* write data to backup DATAx registers */
    write_backup_register(DATA);

    /* check if the written data are correct */
    if(0x00 == check_backup_register(DATA)){
        /* turn on LED2 */
        gd_eval_led_on(LED2);
    }else{
        /* turn on LED3 */
        gd_eval_led_on(LED3);
    }
    
    while(1){
    }
}

/*!
    \brief      configure leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(TAMPER_IRQn, 0, 0);
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* PMU clock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* BKP clock enable */
    rcu_periph_clock_enable(RCU_BKPI);
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
    bkp_data_register_enum temp = BKP_DATA_0;

    /* construct different data and write data to backup registers */
    for(temp = BKP_DATA_REG_MIN; temp <= BKP_DATA_REG_MAX; temp++){
        bkp_data_write(temp, (data + (temp * 0x50)));
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
    bkp_data_register_enum temp = BKP_DATA_0;

    /* check the data of backup registers */
    for(temp = BKP_DATA_REG_MIN; temp <= BKP_DATA_REG_MAX; temp++){
        if(bkp_data_read(temp) != (data + (temp * 0x50))){
            return temp;
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
    bkp_data_register_enum temp = BKP_DATA_0;

    /* check whether the data of backup registers are equal to zero or not */
    for(temp = BKP_DATA_REG_MIN; temp <= BKP_DATA_REG_MAX; temp++){
        if(0x0000 != bkp_data_read(temp)){
            return temp;
        }
    }

    return 0;
}
