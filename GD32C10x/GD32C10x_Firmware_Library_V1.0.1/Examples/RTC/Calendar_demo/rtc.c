/*!
    \file    rtc.c
    \brief   RTC check and config,time_show and time_adjust function
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#include "rtc.h"
#include "gd32c10x_lcd_eval.h"

uint16_t x1 = 80;
uint16_t x2 = 88;
uint16_t y1 = 100;
uint16_t y2 = 120;
char_format_struct char_format;

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(RTC_IRQn,1,0);
}

/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);
    
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* enable the RTC second and alarm interrupt*/
    rtc_interrupt_enable(RTC_INT_SECOND);
    rtc_interrupt_enable(RTC_INT_ALARM);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    rtc_prescaler_set(32767);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
}

/*!
    \brief      calculate how many days have passed in this year by date
    \param[in]  year, month, day : constitute the date
    \param[out] none
    \retval     the days number
*/
uint32_t calc_days_num_by_date(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t i;
    uint32_t sum_days = 0;
    uint32_t days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if((month < 1) || (month > 12)){
        return 0;
    }

    days_of_month[1] = IS_LEAP_YEAR(year) ? 29 : 28;

    for(i = 0; i < (month - 1); i++){
        sum_days += days_of_month[i];
    }

    sum_days += day;

    return sum_days;
}

/*!
    \brief      return the time entered by user, using Hyperterminal
    \param[in]  none
    \param[out] none
    \retval     current time of RTC counter value
*/
uint32_t time_regulate(void)
{
    uint32_t sum_days = 0;
    uint32_t max_days_a_month = 0;
    uint32_t year = 0xFF, month  = 0xFF, day    = 0xFF;
    uint32_t hour = 0xFF, minute = 0xFF, second = 0xFF;
    uint32_t days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    printf("\r\n==============Time Settings=====================================");
    printf("\r\nPlease Set Years (valid range: %d-%d):", MIN_YEAR, MAX_YEAR);
    while (year == 0xFF){
        year = usart_scanf(9999);

        if((year < MIN_YEAR) || (year > MAX_YEAR)){
            year = 0xFF;
            printf("\n\rPlease enter valid number between %d and %d\n", MIN_YEAR, MAX_YEAR);
        }
    }
    days_of_month[1] = IS_LEAP_YEAR(year) ? 29 : 28;
    printf("  %d", year);

    printf("\r\nPlease Set Months (valid range: 01-12):");
    while (month == 0xFF){
        month = usart_scanf(12);

        if(0 == month){
            month = 0xFF;
            printf("\n\rPlease enter valid number between 1 and 12\n");
        }
    }
    printf("  %d", month);

    printf("\r\nPlease Set Days (valid range: 01-31):");
    while (day == 0xFF){
        day = usart_scanf(31);

        max_days_a_month = days_of_month[month - 1];

        if((day < 1) || (day > max_days_a_month)){
            day = 0xFF;
            printf("\n\rPlease enter valid number between 1 and %d\n", max_days_a_month);
        }
    }
    printf("  %d", day);
    
    printf("\r\nPlease Set Hours (valid range: 00-23):");
    while (hour == 0xFF){
        hour = usart_scanf(23);
    }
    printf("  %d", hour);
    
    printf("\r\nPlease Set Minutes (valid range: 00-59):");
    while (minute == 0xFF){
        minute = usart_scanf(59);
    }
    printf("  %d", minute);
    
    printf("\r\nPlease Set Seconds (valid range: 00-59):");
    while (second == 0xFF){
        second = usart_scanf(59);
    }
    printf("  %d", second);

    printf("\r\n\nTime is set successfully as : %d.%.2d.%.2d  %.2d:%.2d:%.2d", year, month, day, hour, minute, second);

    sum_days = calc_days_num_by_date(year, month, day);

    /* return the value which will be stored in RTC counter register */
    /* year information will be stored in the high 7 bits, total second information will be stored in the low 25 bits */
    return (((year - MIN_YEAR) << 25) + (((sum_days - 1) * 24 * 3600) + (hour * 3600 + minute * 60 + second)));
}

/*!
    \brief      adjust time 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_adjust(void)
{
    uint32_t temp = 0;
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    temp = time_regulate();
    /* change the current time */
    rtc_counter_set(temp);
    rtc_lwoff_wait();
}

/*!
    \brief      print calendar on lcd
    \param[in]  rtc_counter: value read from RTC counter register
    \param[out] none
    \retval     none
*/
void print_calendar_on_lcd(uint32_t rtc_counter)
{
    uint32_t i = 0, day_num = 0, temp = 0;
    uint32_t year = 0, month = 0, day = 0;
    uint32_t hour = 0, minute = 0, second = 0;
    uint8_t date_string[10];
    uint8_t time_string[8];
    uint32_t days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* compute years */
    year = (rtc_counter >> 25) + MIN_YEAR;
    rtc_counter &= 0x01FFFFFF;
    
    /* compute months */
    day_num  = rtc_counter / (3600 * 24);
    days_of_month[1] = IS_LEAP_YEAR(year) ? 29 : 28;
    for(i = 0; i < 12; i++){
        if(day_num >= days_of_month[i]){
            day_num -= days_of_month[i];
            continue;
        }else{
            break;
        }
    }
    month = ((i >= 12) ? 12 : (i + 1));
    /* compute days */
    day = day_num + 1;

    temp = rtc_counter % (3600 * 24);
    /* compute  hours */
    hour = temp / 3600;
    /* compute minutes */
    minute = (temp % 3600) / 60;
    /* compute seconds */
    second = (temp % 3600) % 60;

    date_string[0] = year / 1000 + DIGIT_TO_CHAR_OFFSET;
    date_string[1] = (year % 1000) / 100 + DIGIT_TO_CHAR_OFFSET;
    date_string[2] = ((year % 100) / 10) + DIGIT_TO_CHAR_OFFSET;
    date_string[3] = (year % 10) + DIGIT_TO_CHAR_OFFSET;
    date_string[4] = '.';
    date_string[5] = month / 10 + DIGIT_TO_CHAR_OFFSET;
    date_string[6] = month % 10 + DIGIT_TO_CHAR_OFFSET;
    date_string[7] = '.';
    date_string[8] = day / 10 + DIGIT_TO_CHAR_OFFSET;
    date_string[9] = day % 10 + DIGIT_TO_CHAR_OFFSET;

    time_string[0] = hour / 10 + DIGIT_TO_CHAR_OFFSET;
    time_string[1] = hour % 10 + DIGIT_TO_CHAR_OFFSET;
    time_string[2] = ':';
    time_string[3] = minute / 10 + DIGIT_TO_CHAR_OFFSET;
    time_string[4] = minute % 10 + DIGIT_TO_CHAR_OFFSET;
    time_string[5] = ':';
    time_string[6] = second / 10 + DIGIT_TO_CHAR_OFFSET;
    time_string[7] = second % 10 + DIGIT_TO_CHAR_OFFSET;
    
    /* configure char format */
    char_format.char_color = BLUE;
    char_format.bk_color   = WHITE;
    char_format.direction  = CHAR_DIRECTION_VERTICAL;
    char_format.font       = CHAR_FONT_8_16;
    
    /* draw date on LCD screen */
    for(i = 0; i < 10; i++){
        lcd_char_display((x1+8*i), y1, date_string[i], char_format);
    }

    /* draw time on LCD screen */
    for(i = 0; i < 8; i++){
        lcd_char_display((x2+8*i), y2, time_string[i], char_format);
    }
}

/*!
    \brief      get numeric values from the hyperterminal
    \param[in]  max_value: input value can not beyond this max value
    \param[out] none
    \retval     input value in BCD mode
*/
uint32_t usart_scanf(uint32_t max_value)
{
    uint32_t err_flag = 0;
    uint32_t index = 0, loop = 0, input_value = 0;
    uint8_t  tmp[4] = {0, 0, 0, 0};
    uint32_t multiple[4] = {1, 10, 100, 1000};

    for(index = 0; index < 4; index++){
        /* loop until RBNE = 1 */
        while(usart_flag_get(USART1, USART_FLAG_RBNE) == RESET);
        tmp[index] = (usart_data_receive(USART1));

        if((tmp[index] < DIGIT_TO_CHAR_OFFSET) || (tmp[index] > (DIGIT_TO_CHAR_OFFSET + 9))){
            err_flag = 1;
        }

        /* only year information need 4 digits, other information just need 2 digits */
        if((9999 != max_value) && (1 == index)){
            index++;
            break;
        }
    }

    if(1 == err_flag){
        printf("\n\rPlease enter digit between 0 and 9 rather than other characters\n");
        return 0xFF;
    }

    /* transform to numeric value */
    for(loop = 0; loop < index; loop++){
        input_value += ((tmp[loop] - DIGIT_TO_CHAR_OFFSET) * multiple[index - loop - 1]);
    }

    /* check */
    if (input_value > max_value){
        printf("\n\rPlease enter valid number between 0 and %d\n", max_value);
        return 0xFF;
    }
    return input_value;
}
