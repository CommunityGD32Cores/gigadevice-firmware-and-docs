/*!
    \file  main.c
    \brief RTC calendar 
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"
#include "gd32f403_eval.h"
#include "rtc.h"
#include <stdio.h>

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* COM1 config */
    gd_eval_com_init(EVAL_COM1);

    /* NVIC config */
    nvic_configuration();

    printf( "\r\n This is a RTC demo...... \r\n" );

    if (bkp_read_data(BKP_DATA_0) != 0xA5A5){
        /* backup data register value is not correct or not yet programmed
        (when the first time the program is executed) */
        printf("\r\nThis is a RTC demo!\r\n");
        printf("\r\n\n RTC not yet configured....");

        /* RTC configuration */
        rtc_configuration();

        printf("\r\n RTC configured....");

        /* adjust time by values entred by the user on the hyperterminal */
        time_adjust();

        bkp_write_data(BKP_DATA_0, 0xA5A5);
    }else{
        /* check if the power on reset flag is set */
        if (rcu_flag_get(RCU_FLAG_PORRST) != RESET){
            printf("\r\n\n Power On Reset occurred....");
        }else if (rcu_flag_get(RCU_FLAG_SWRST) != RESET){
            /* check if the pin reset flag is set */
            printf("\r\n\n External Reset occurred....");
        }

        /* allow access to BKP domain */
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();

        printf("\r\n No need to configure RTC....");
        /* wait for RTC registers synchronization */
        rtc_register_sync_wait();

        /* enable the RTC second */
        rtc_interrupt_enable(RTC_INT_SECOND);

        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
    }

#ifdef RTCCLOCKOUTPUT_ENABLE
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* disable the tamper pin */
    bkp_tamper_detection_disable();

    /* enable RTC clock output on tamper Pin */
    bkp_rtc_calibration_output_enable();
#endif

    /* clear reset flags */
    rcu_all_reset_flag_clear();

    /* display time in infinite loop */
    time_show();

    while (1){
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
