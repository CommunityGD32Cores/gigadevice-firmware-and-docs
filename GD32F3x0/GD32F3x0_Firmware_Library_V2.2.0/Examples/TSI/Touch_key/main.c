/*!
    \file    main.c
    \brief   use the TSI to perform continuous acquisitions of three channels

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
    \version 2022-01-06, V2.2.0, firmware for GD32F3x0
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "gd32f3x0.h"
#include "gd32f350r_eval.h"
#include <stdlib.h>
#include <stdio.h>

/* the current cycle number array of the channel pin */
uint16_t samplenum[4] = {0, 0, 0, 0};

/* reference value sample array of TSI group5 */
uint16_t sample_refnum_array1[20] = {0};
uint16_t sample_refnum_array2[20] = {0};
uint16_t sample_refnum_array3[20] = {0};

/* average value of cycles */
uint16_t sample_refnum[3] = {0};

void delay(uint32_t nCount);
void gpio_config(void);
void tsi_config(void);
void led_config(void);
void tsi_transfer_pin(uint32_t TSI_groupx_pin);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int m = 0;

    /* TSI peripheral and GPIOB Periph clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_TSI);

    /* PB11  TSI_CHCFG_G5P0     SAMPCAP
       PB12  TSI_CHCFG_G5P1     CHANNEL
       PB13  TSI_CHCFG_G5P2     CHANNEL
       PB14  TSI_CHCFG_G5P3     CHANNEL */

    /* configure the GPIO ports */
    gpio_config();

    /* configure the TSI peripheral */
    tsi_config();

    /* configure the LED */
    led_config();

    /* reference cycle value acquisition and processing */
    for(m = 0; m < 20; m++) {
        /* get charge transfer complete cycle number of group5 pin1 */
        tsi_transfer_pin(TSI_CHCFG_G5P1);

        /* check the TSI flag:end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF)) {
            /* get charge transfer complete cycle number */
            sample_refnum_array1[m] = tsi_group5_cycle_get();
        }

        /* disable the selected pin as channel pin */
        tsi_channel_pin_disable(TSI_CHCFG_G5P1);

        /* get charge transfer complete cycle number of group5 pin2 */
        tsi_transfer_pin(TSI_CHCFG_G5P2);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF)) {
            sample_refnum_array2[m] = tsi_group5_cycle_get();
        }

        tsi_channel_pin_disable(TSI_CHCFG_G5P2);

        /* get charge transfer complete cycle number of group5 pin3 */
        tsi_transfer_pin(TSI_CHCFG_G5P3);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF)) {
            sample_refnum_array3[m] = tsi_group5_cycle_get();
        }

        tsi_channel_pin_disable(TSI_CHCFG_G5P3);

        /* delay for a period of time while all banks have been acquired */
        delay(0x1000);
    }

    for(m = 1; m < 20; m++) {
        sample_refnum[0] += sample_refnum_array1[m];
        sample_refnum[1] += sample_refnum_array2[m];
        sample_refnum[2] += sample_refnum_array3[m];
    }

    /* average channel cycle value are obtained */
    sample_refnum[0] = sample_refnum[0] / 19;
    sample_refnum[1] = sample_refnum[1] / 19;
    sample_refnum[2] = sample_refnum[2] / 19;

    while(1) {
        /* acquisition pin1 of group5 */
        tsi_transfer_pin(TSI_CHCFG_G5P1);

        /* check the TSI flag--end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF)) {
            /* get charge transfer complete cycle number */
            samplenum[1] = tsi_group5_cycle_get();
        }

        /* channel 1 touch */
        if((sample_refnum[0] - samplenum[1]) > 0x20) {
            /* pin1 of group5 is touched */
            gd_eval_led_on(LED1);
        } else {
            gd_eval_led_off(LED1);
        }
        tsi_channel_pin_disable(TSI_CHCFG_G5P1);

        /* acquisition pin2 of group5 */
        tsi_transfer_pin(TSI_CHCFG_G5P2);

        /* check the TSI flag--end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF)) {
            samplenum[2] = tsi_group5_cycle_get();
        }

        /* light LED2 */
        if((sample_refnum[1] - samplenum[2]) > 0x20) {
            /* TSI_GROUP6_PIN3 is touched */
            gd_eval_led_on(LED2);
        } else {
            gd_eval_led_off(LED2);
        }
        tsi_channel_pin_disable(TSI_CHCFG_G5P2);

        /* acquisition pin3 of group5 */
        tsi_transfer_pin(TSI_CHCFG_G5P3);

        /* check the TSI flag--end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF)) {
            samplenum[3] =  tsi_group5_cycle_get();
        }
        /* light LED3 */
        if((sample_refnum[2] - samplenum[3]) > 0x20) {
            /* pin3 of group5 is touched */
            gd_eval_led_on(LED3);
        } else {
            gd_eval_led_off(LED3);
        }
        tsi_channel_pin_disable(TSI_CHCFG_G5P3);
    }
}

/*!
    \brief      insert a delay time
    \param[in]  nCount: stall Count
    \param[out] none
    \retval     none
*/
void delay(uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* GPIOB11 */
    /* alternate function output open-drain for sampling capacitor IO */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_11);

    /* GPIOB12  GPIOB13 GPIOB14 */
    /* alternate function output push-pull for channel and shield IOs */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_14);

    /* connect pin to peripheral */
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_11);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_13);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_14);
}

/*!
    \brief      configure the TSI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tsi_config(void)
{
    /* TSI configure */
    tsi_init(TSI_CTCDIV_DIV32, TSI_CHARGE_2CTCLK, TSI_TRANSFER_2CTCLK, TSI_MAXNUM2047);
    tsi_sofeware_mode_config();
    tsi_sample_pin_enable(TSI_SAMPCFG_G5P0);
    tsi_group_enable(TSI_GCTL_GE5);

    /* disable hysteresis mode */
    tsi_hysteresis_off(TSI_PHM_G5P0 | TSI_PHM_G5P1 | TSI_PHM_G5P2 | TSI_PHM_G5P3);

    /* enable TSI */
    tsi_enable();
}

/*!
    \brief      configure led
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    /* initialize the LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* close all of LEDs */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
}

/*!
    \brief      acquisition pin y of group x,x=0..5,y=0..3
    \param[in]  tsi_groupx_piny: TSI_CHCFG_GxPy,pin y of group x
    \param[out] none
    \retval     none
*/
void tsi_transfer_pin(uint32_t tsi_groupx_piny)
{
    /* configure the TSI pin channel mode */
    tsi_channel_pin_enable(tsi_groupx_piny);

    /* wait capacitors discharge */
    delay(0xD00);

    /* clear both MNERR and CTCF flags */
    tsi_flag_clear(TSI_FLAG_CTCF | TSI_FLAG_MNERR);

    /* start a new acquisition */
    tsi_software_start();

    /* wait the specified TSI flag state: MNERR or CTCF */
    while(RESET == tsi_flag_get(TSI_FLAG_CTCF | TSI_FLAG_MNERR));
}
