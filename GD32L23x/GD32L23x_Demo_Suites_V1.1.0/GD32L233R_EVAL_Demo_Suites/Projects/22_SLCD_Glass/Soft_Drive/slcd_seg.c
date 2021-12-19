/*!
    \file    slcd_seg.c
    \brief   source of the slcd segment driver

    \version 2021-08-15, V1.0.0, demo for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "slcd_seg.h"
#include "gd32l23x.h"

/* digit SLCD DATA buffer */
uint8_t digit[4];

/* table of the digit code for SLCD */
__I uint32_t numbertable[10] = {
    /* 0     1     2     3     4 */
    0xDD, 0x88, 0x79, 0xE9, 0xAC,
    /* 5     6     7     8     9 */
    0xE5, 0xF5, 0x89, 0xFD, 0xED
};

static void digit_to_code(uint8_t c);
static void slcd_gpio_config(void);
static void slcd_seg_digit_write(uint8_t ch, uint8_t position, slcd_display_enum type);

/*!
    \brief      convert digit to SLCD code
    \param[in]  the digit to write
    \param[out] none
    \retval     none
*/
static void digit_to_code(uint8_t c)
{
    uint8_t ch = 0;

    /* the *c is a number */
    if(c < 10) {
        ch = numbertable[c];
    }

    digit[0] = (uint8_t)((ch) & 0x03);
    digit[1] = (uint8_t)((ch >> 2) & 0x03);
    digit[2] = (uint8_t)((ch >> 4) & 0x03);
    digit[3] = (uint8_t)((ch >> 6) & 0x03);
}

/*!
    \brief      initialize the GPIO port of SLCD peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void slcd_gpio_config(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    /* SLCD GPIO */
    /* configure GPIOA */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    /* configure GPIOB */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ,
                            GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* configure GPIOC */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    gpio_af_set(GPIOC, GPIO_AF_3, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

/*!
    \brief      configure SLCD peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void slcd_seg_configuration(void)
{
    volatile uint16_t i;

    /* configure the SLCD GPIO pins */
    slcd_gpio_config();

    /* enable the clock of SLCD */
    rcu_periph_clock_enable(RCU_SLCD);
    /* wait 2 RTC clock to write SLCD register */
    for(i = 0; i < 1000; i++);

    slcd_deinit();
    /* config the prescaler and the divider of SLCD clock */
    slcd_clock_config(SLCD_PRESCALER_2, SLCD_DIVIDER_16);
    /* SLCD bias voltage select */
    slcd_bias_voltage_select(SLCD_BIAS_1_4);
    /* SLCD duty cycle select */
    slcd_duty_select(SLCD_DUTY_1_4);
    /* SLCD voltage source select */
    slcd_voltage_source_select(SLCD_VOLTAGE_INTERNAL);
    /* SLCD pulse on duration config */
    slcd_pulse_on_duration_config(SLCD_PULSEON_DURATION_7);
    /* SLCD dead time duration config */
    slcd_dead_time_config(SLCD_DEADTIME_PERIOD_7);
    slcd_contrast_ratio_config(SLCD_CONTRAST_LEVEL_7);
    /* enable the permanent high drive */
    slcd_high_drive_config(ENABLE);

    slcd_enhance_mode_enable();
    /* wait for SLCD CFG register synchronization */
    while(!slcd_flag_get(SLCD_FLAG_SYN));
    /* enable SLCD interface */
    slcd_enable();
    /* wait for SLCD controller on flag */
    while(!slcd_flag_get(SLCD_FLAG_ON));
}

/*!
    \brief      write one digit to the SLCD DATA register
    \param[in]  ch: the digit to write
    \param[in]  position: position in the SLCD of the digit to write
    \param[out] none
    \retval     none
*/
void slcd_seg_digit_display(uint8_t ch, uint8_t position)
{
    /* wait the last SLCD DATA update request finished */
    while(slcd_flag_get(SLCD_FLAG_UPR));

    /* SLCD write a char */
    slcd_seg_digit_write(ch, position, INTEGER);

    /* request SLCD DATA update */
    slcd_data_update_request();
}

/*!
    \brief      write a integer(6 digits) to SLCD DATA register
    \param[in]  num: number to send to SLCD(0-999999)
    \param[out] none
    \retval     none
*/
void slcd_seg_number_display(uint32_t num)
{
    uint8_t i = 0x00, length, ch[6];

    ch[5] = num / 100000;
    ch[4] = (num % 100000) / 10000;
    ch[3] = (num % 10000) / 1000;
    ch[2] = (num % 1000) / 100;
    ch[1] = (num % 100) / 10;
    ch[0] = num % 10;

    if(ch[5]) {
        length = 6;
    } else if(ch[4]) {
        length = 5;
    } else if(ch[3]) {
        length = 4;
    } else if(ch[2]) {
        length = 3;
    } else if(ch[1]) {
        length = 2;
    } else {
        length = 1;
    }

    slcd_seg_clear_all();
    /* wait the last SLCD DATA update request finished */
    while(slcd_flag_get(SLCD_FLAG_UPR));

    /* send the string character one by one to SLCD */
    while(i < length) {
        /* display one digit on SLCD */
        slcd_seg_digit_write(ch[i], 6 - i, INTEGER);
        /* increment the digit counter */
        i++;
    }

    /* request SLCD DATA update */
    slcd_data_update_request();
}

/*!
    \brief      write a float number(6 digits which has 2 decimal) to SLCD DATA register
    \param[in]  num: number to send to SLCD
    \param[out] none
    \retval     none
*/
void slcd_seg_floatnumber_display(float num)
{
    uint8_t i = 0x00, length, ch[6];
    uint32_t temp;

    temp = (uint32_t)(num * 100);
    ch[5] = temp / 100000;
    ch[4] = (temp % 100000) / 10000;
    ch[3] = (temp % 10000) / 1000;
    ch[2] = (temp % 1000) / 100;
    ch[1] = (temp % 100) / 10;
    ch[0] = temp % 10;

    if(ch[5]) {
        length = 6;
    } else if(ch[4]) {
        length = 5;
    } else if(ch[3]) {
        length = 4;
    } else {
        length = 3;
    }

    slcd_seg_clear_all();
    /* wait the last SLCD DATA update request finished */
    while(slcd_flag_get(SLCD_FLAG_UPR));

    /* send the string character one by one to SLCD */
    while(i < length) {
        /* display one digit on SLCD */
        slcd_seg_digit_write(ch[i], 6 - i, FLOAT);
        /* increment the digit counter */
        i++;
    }

    /* request SLCD DATA update */
    slcd_data_update_request();
}

/*!
    \brief      write a digit to SLCD DATA register
    \param[in]  ch: the digit to write
    \param[in]  position: position in the SLCD of the digit to write,which can be 1..6
    \param[in]  type: the type of the data
    \param[out] none
    \retval     none
*/
static void slcd_seg_digit_write(uint8_t ch, uint8_t position, slcd_display_enum type)
{
    /* convert ASCii to SLCD digit or char */
    digit_to_code(ch);

    switch(position) {
    case 6:
        /* clear the corresponding segments (COM0..COM3, SEG22..23) */
        SLCD_DATA0 &= (uint32_t)(0xFF3FFFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFF3FFFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFF3FFFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFF3FFFFFU);

        /* write the corresponding segments (COM0..COM3, SEG22..23) */
        SLCD_DATA0 |= (uint32_t)(digit[0] << 22);
        SLCD_DATA1 |= (uint32_t)(digit[1] << 22);
        SLCD_DATA2 |= (uint32_t)(digit[2] << 22);
        SLCD_DATA3 |= (uint32_t)(digit[3] << 22);
        break;

    case 5:
        /* clear the corresponding segments (COM0..COM3, SEG21..20) */
        SLCD_DATA0 &= (uint32_t)(0xFFCFFFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFCFFFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFCFFFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFCFFFFFU);

        /* write the corresponding segments (COM0..COM3, SEG21..20) */
        SLCD_DATA0 |= (uint32_t)(digit[0] << 20);
        SLCD_DATA1 |= (uint32_t)(digit[1] << 20);
        SLCD_DATA2 |= (uint32_t)(digit[2] << 20);
        SLCD_DATA3 |= (uint32_t)(digit[3] << 20);
        break;

    case 4:
        /* clear the corresponding segments (COM0..COM3, SEG18..19) */
        SLCD_DATA0 &= (uint32_t)(0xFFF3FFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFF3FFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFF3FFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFF3FFFFU);

        /* write the point (COM0, SEG19) */
        if(type == FLOAT) {
            SLCD_DATA0 |= (uint32_t)(0x01 << 19);
        }
        /* write the corresponding segments (COM0..COM3, SEG18..19) */
        SLCD_DATA0 |= (uint32_t)(digit[0] << 18);
        SLCD_DATA1 |= (uint32_t)(digit[1] << 18);
        SLCD_DATA2 |= (uint32_t)(digit[2] << 18);
        SLCD_DATA3 |= (uint32_t)(digit[3] << 18);
        break;

    case 3:
        /* clear the corresponding segments (COM0..COM3, SEG14..15) */
        SLCD_DATA0 &= (uint32_t)(0xFFFF3FFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFFF3FFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFFF3FFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFFF3FFFU);

        /* write the corresponding segments (COM0..COM3, SEG12..13) */
        SLCD_DATA0 |= (uint32_t)(digit[0] << 14);
        SLCD_DATA1 |= (uint32_t)(digit[1] << 14);
        SLCD_DATA2 |= (uint32_t)(digit[2] << 14);
        SLCD_DATA3 |= (uint32_t)(digit[3] << 14);
        break;

    case 2:
        /* clear the corresponding segments (COM0..COM3, SEG12..13) */
        SLCD_DATA0 &= (uint32_t)(0xFFFFCFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFFFCFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFFFCFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFFFCFFFU);

        /* write the corresponding segments (COM0..COM3, SEG12..13) */
        SLCD_DATA0 |= (uint32_t)(digit[0] << 12);
        SLCD_DATA1 |= (uint32_t)(digit[1] << 12);
        SLCD_DATA2 |= (uint32_t)(digit[2] << 12);
        SLCD_DATA3 |= (uint32_t)(digit[3] << 12);
        break;

    case 1:
        /* clear the corresponding segments (COM0..COM3, SEG6/16) */
        SLCD_DATA0 &= (uint32_t)(0xFFFEFFBFU);
        SLCD_DATA1 &= (uint32_t)(0xFFFEFFBFU);
        SLCD_DATA2 &= (uint32_t)(0xFFFEFFBFU);
        SLCD_DATA3 &= (uint32_t)(0xFFFEFFBFU);

        /* write the corresponding segments (COM0..COM3, SEG6/16) */
        SLCD_DATA0 |= ((uint32_t)((digit[0] & 0x01) << 6)) | ((uint32_t)((digit[0] & 0x02) << 15));
        SLCD_DATA1 |= ((uint32_t)((digit[1] & 0x01) << 6)) | ((uint32_t)((digit[1] & 0x02) << 15));
        SLCD_DATA2 |= ((uint32_t)((digit[2] & 0x01) << 6)) | ((uint32_t)((digit[2] & 0x02) << 15));
        SLCD_DATA3 |= ((uint32_t)((digit[3] & 0x01) << 6)) | ((uint32_t)((digit[3] & 0x02) << 15));
        break;
    }
}

/*!
    \brief      clear data in the SLCD DATA register
    \param[in]  position: position in the SLCD of the digit to write,which can be 1..6
    \param[out] none
    \retval     none
*/
void slcd_seg_digit_clear(uint8_t position)
{
    switch(position) {
    case 6:
        /* clear the corresponding segments (COM0..COM3, SEG22..23) */
        SLCD_DATA0 &= (uint32_t)(0xFF3FFFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFF3FFFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFF3FFFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFF3FFFFFU);
        break;

    case 5:
        /* clear the corresponding segments (COM0..COM3, SEG20..21) */
        SLCD_DATA0 &= (uint32_t)(0xFFCFFFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFCFFFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFCFFFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFCFFFFFU);
        break;

    case 4:
        /* clear the corresponding segments (COM0..COM3, SEG18..19) */
        SLCD_DATA0 &= (uint32_t)(0xFFF3FFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFF3FFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFF3FFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFF3FFFFU);
        break;

    case 3:
        /* clear the corresponding segments (COM0..COM3, SEG14..15) */
        SLCD_DATA0 &= (uint32_t)(0xFFFF3FFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFFF3FFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFFF3FFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFFF3FFFU);
        break;

    case 2:
        /* clear the corresponding segments (COM0..COM3, SEG12..13) */
        SLCD_DATA0 &= (uint32_t)(0xFFFFCFFFU);
        SLCD_DATA1 &= (uint32_t)(0xFFFFCFFFU);
        SLCD_DATA2 &= (uint32_t)(0xFFFFCFFFU);
        SLCD_DATA3 &= (uint32_t)(0xFFFFCFFFU);
        break;

    case 1:
        /* clear the corresponding segments (COM0..COM3, SEG6/16) */
        SLCD_DATA0 &= (uint32_t)(0xFFFEFFBFU);
        SLCD_DATA1 &= (uint32_t)(0xFFFEFFBFU);
        SLCD_DATA2 &= (uint32_t)(0xFFFEFFBFU);
        SLCD_DATA3 &= (uint32_t)(0xFFFEFFBFU);
        break;
    }
}

/*!
    \brief      clear all the SLCD DATA register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void slcd_seg_clear_all(void)
{
    SLCD_DATA0 = 0x0000;
    SLCD_DATA1 = 0x0000;
    SLCD_DATA2 = 0x0000;
    SLCD_DATA3 = 0x0000;
}
