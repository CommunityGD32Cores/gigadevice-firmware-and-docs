/*!
    \file    smartcard.c
    \brief   smartcard driver

    \version 2020-07-23, V3.0.0, firmware for GD32F1x0
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

#include "smartcard.h"

sc_ATR sc_A2R;
uint8_t ATR_length = 0U;
uint8_t sc_ATR_table[40] = {0};
static __IO uint8_t scdata = 0U;
static uint32_t F_table[16] = {372, 372, 558, 744, 1116, 1488, 1860, 0,0, 512, \
                               768, 1024, 1536, 2048, 0, 0};
static uint32_t D_table[16] = {0, 1, 2, 4, 8, 16, 32, 0, 12, 20, 0, 0, 0, 0, 0, 0};

/* private function */
static void sc_deinit(void);
static uint8_t sc_decode_answer2reset(uint8_t *card);
static void sc_answer_req(sc_state_enum *scstate, uint8_t *card, uint8_t length);
static void sc_send_data(sc_APDU_commands *sc_APDU, sc_APDU_response *sc_response_status);
static ErrStatus usart_byte_receive(uint8_t *data, uint32_t timeout);

/*!
    \brief      handles all smartcard states and serves to send and receive all
                communication data between smartcard and reader
    \param[in]  scstate: pointer to an sc_state_enum enumeration
    \param[in]  sc_APDU: pointer to an sc_APDU_commands structure that will be initialized
    \param[out] sc_response: pointer to a sc_APDU_response structure which will be initialized
    \retval     none
*/
void sc_handler(sc_state_enum *scstate, sc_APDU_commands *sc_APDU, sc_APDU_response *sc_response)
{
    uint32_t i = 0U;

    switch(*scstate){
    case SC_POWER_ON:
        if(SC_GET_A2R == sc_APDU->header.ins){
            /* smartcard initialization */
            sc_init();

            /* reset data from sc buffer */
            for(i = 0U; i < 40U; i++){
                sc_ATR_table[i] = 0U;
            }

            /* reset sc_A2R structure */
            sc_A2R.ts = 0U;
            sc_A2R.t0 = 0U;
            for (i = 0U; i < SETUP_LENGTH; i++){
                sc_A2R.t[i] = 0U;
            }

            for(i = 0U; i < HIST_LENGTH; i++){
                sc_A2R.h[i] = 0U;
            }

            sc_A2R.tlength = 0U;
            sc_A2R.hlength = 0U;

            /* next state */
            *scstate = SC_RESET_LOW;
        }
        break;

    case SC_RESET_LOW:
        if(SC_GET_A2R == sc_APDU->header.ins){
            /* if card is detected then POWER ON, card reset and wait for an answer) */
            if(1U == sc_detect()){
                while((SC_POWER_OFF != (*scstate)) && (SC_ACTIVE != (*scstate))){
                    /* check for answer to reset */
                    sc_answer_req(scstate, (uint8_t *)&sc_ATR_table[0], 40U);
                }
            }else{
                (*scstate) = SC_POWER_OFF;
            }
        }
        break;

    case SC_ACTIVE:
        if(SC_GET_A2R == sc_APDU->header.ins){
            if(T0_PROTOCOL == sc_decode_answer2reset(&sc_ATR_table[0])){
                (*scstate) = SC_ACTIVE_ON_T0;
            }else{
                (*scstate) = SC_POWER_OFF; 
            }
        }
        break;

    case SC_ACTIVE_ON_T0:
        sc_send_data(sc_APDU, sc_response);
        break;

    case SC_POWER_OFF:
        /* disable smartcard interface */
        sc_deinit(); 
        break;

    default:
        (*scstate) = SC_POWER_OFF;
    }
}

/*!
    \brief      configures the communication baudrate
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sc_PTS_config(void)
{
    uint32_t work_baudrate = 0U, apbclock = 0U;
    uint8_t PPS_confirm_status = 1U;
    uint8_t PTS_cmd[4] = {0x00U, 0x00U, 0x00U, 0x00U};
    uint8_t PTS_answer[4] = {0x00U, 0x00U, 0x00U, 0x00U};

    /* reconfigure the USART baud-rate */
    apbclock = rcu_clock_freq_get(CK_USART);

    apbclock /= ((USART_GP(SC_USART)& (uint16_t)0x00FFU) * 2U);

    /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
       in case of a framing error FE */
    usart_dma_receive_config(SC_USART, USART_DENR_ENABLE);

    if(0x10U == (sc_A2R.t0 & (uint8_t)0x10)){
        if(0x11U != sc_A2R.t[0]){
            /* PPSS identifies the PPS request or response and is equal to 0xFF */
            PTS_cmd[0] = 0xFFU;

            /* PPS0 indicates by the bits b5, b6, b7 equal to 1 the presence of the optional
            bytes PPSI1, PPS2, PPS3 respectively */
            PTS_cmd[1] = 0x10U;

            /* PPS1 allows the interface device to propose value of F and D to the card */
            PTS_cmd[2] = sc_A2R.t[0];

            /* PCK check character */
            PTS_cmd[3] = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)sc_A2R.t[0];

            /* send command */
            for(uint8_t i = 0U; i < 4U; i++){
                scdata = PTS_cmd[i];
                usart_data_transmit(SC_USART, scdata);
                while(RESET == usart_flag_get(SC_USART, USART_FLAG_TC));
            }

            /* Disable the DMA Receive (Reset DMAR bit only) */  
            usart_dma_receive_config(SC_USART, USART_DENR_DISABLE);

            /* start the receive process: to receive the command answer from the card */
            for(uint8_t i = 0U; i < 4U; i++){
                usart_byte_receive((uint8_t *)&PTS_answer[i], SC_RECEIVE_TIMEOUT);
            }

            /* check the received command answer */
            if((0xFFU != PTS_answer[0]) || (0x10U != PTS_answer[1]) || (PTS_answer[2] != sc_A2R.t[0]) ||
            (PTS_answer[3] != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)sc_A2R.t[0]))){
                /* PPSS, PPS0, PPS1 and PCK exchange unsuccessful */
                PPS_confirm_status = 0x00U;
            }

            /* PPS exchange successful */
            if(0x01U == PPS_confirm_status){
                work_baudrate = apbclock * D_table[(sc_A2R.t[0] & (uint8_t)0x0F)];
                work_baudrate /= F_table[((sc_A2R.t[0] >> 4U) & (uint8_t)0x0FU)];

                usart_disable(SC_USART);
                usart_baudrate_set(SC_USART, work_baudrate);
                usart_parity_config(SC_USART, USART_PM_EVEN);
                usart_word_length_set(SC_USART, USART_WL_9BIT);
                usart_stop_bit_set(SC_USART, USART_STB_1_5BIT);
                usart_transmit_config(SC_USART, USART_TRANSMIT_ENABLE);
                usart_receive_config(SC_USART, USART_RECEIVE_ENABLE);
                usart_enable(SC_USART);
            }
        }
    }
}

/*!
    \brief      initializes all peripheral used for smart card
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sc_init(void)
{
    /* enable smartcard interface clock */
    SC_3_5V_CLK_ENABLE();
    SC_RESET_CLK_ENABLE();
    SC_CMDVCC_CLK_ENABLE();
    SC_DETECT_CLK_ENABLE();

    /* configure smartcard 3/5V pin */
    gpio_mode_set(SC_3_5V_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_3_5V_PIN);
    gpio_output_options_set(SC_3_5V_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SC_3_5V_PIN);

    /* configure smartcard RESET pin */
    gpio_mode_set(SC_RESET_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_RESET_PIN);
    gpio_output_options_set(SC_RESET_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SC_RESET_PIN);

    /* configure smartcard CMDVCC pin */
    gpio_mode_set(SC_CMDVCC_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_CMDVCC_PIN);
    gpio_output_options_set(SC_CMDVCC_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SC_CMDVCC_PIN);

    /* configure smartcard DETECT pin */
    gpio_mode_set(SC_DETECT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SC_DETECT_PIN);
    syscfg_exti_line_config(SC_DETECT_EXTI_PORT_SOURCE, SC_DETECT_EXTI_PIN_SOURCE);
    exti_init(SC_DETECT_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(SC_DETECT_EXTI_LINE);

    /* configure the NVIC for smartcard DETECT pin */
    nvic_irq_enable(SC_DETECT_EXTI_IRQn, 2U, 0U);

    /***********************************************************
     USART configured as follow:
        - Word Length = 9 Bits
        - 1.5 Stop Bit
        - Even parity
        - BaudRate = 10753 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Tx and Rx enabled
        - USART Clock enabled
    *************************************************************/
    /* enable clock  */
    SC_USART_GPIO_CLK_ENABLE();
    SC_USART_CLK_ENABLE();

    /* configure USARTx_CK pin */
    gpio_af_set(SC_USART_CK_GPIO_PORT, SC_USART_CK_AF, SC_USART_CK_PIN);
    gpio_mode_set(SC_USART_CK_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SC_USART_CK_PIN);
    gpio_output_options_set(SC_USART_CK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SC_USART_CK_PIN);

    /* configure USARTx_Tx pin */
    gpio_af_set(SC_USART_TX_GPIO_PORT, SC_USART_TX_AF, SC_USART_TX_PIN);
    gpio_mode_set(SC_USART_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SC_USART_TX_PIN);
    gpio_output_options_set(SC_USART_TX_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, SC_USART_TX_PIN);

    /* configure the NVIC for SC_USART IRQ */
    nvic_irq_enable(SC_USART_IRQn, 0U, 0U);

    /* configure the initial parameter */
    usart_deinit(SC_USART);

    /* USART clock set to 4 MHz */

    usart_prescaler_config(SC_USART, (rcu_clock_freq_get(CK_USART)/1000000U)/(4*2));

    /* USART Guard Time set to 16 Bit */
    usart_guard_time_config(SC_USART, 16U);

    /* configure smartcard clock  */
    usart_synchronous_clock_config(SC_USART, USART_CLEN_EN, USART_CPH_1CK, USART_CPL_LOW);
    usart_clock_enable(SC_USART);

    usart_baudrate_set(SC_USART, 10753U);
    usart_parity_config(SC_USART, USART_PM_EVEN);
    usart_word_length_set(SC_USART, USART_WL_9BIT);
    usart_stop_bit_set(SC_USART, USART_STB_1_5BIT);
    usart_transmit_config(SC_USART, USART_TRANSMIT_ENABLE);
    usart_receive_config(SC_USART, USART_RECEIVE_ENABLE);
    usart_hardware_flow_rts_config(SC_USART, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(SC_USART, USART_RTS_DISABLE);

    /* enable the SC_USART Parity error interrupt */
    usart_interrupt_enable(SC_USART, USART_INT_PERR);

    /* enable the SC_USART Framing¡¢ORERR¡¢NERR error interrupt */
    usart_interrupt_enable(SC_USART, USART_INT_ERR);

    /* enable the NACK transmission */
    usart_smartcard_mode_nack_enable(SC_USART);

    /* enable the smartcard interface */
    usart_smartcard_mode_enable(SC_USART);

    /* configure the auto retry */
    usart_smartcard_autoretry_config(SC_USART, 0x06);

    /* enable SC_USART */
    usart_enable(SC_USART);

    /***********************************************************
    SmartCard interface configure:
        - select voltage;
        - disable power
        - set reset pin
    *************************************************************/
    /* select 5V */
    sc_voltage_config(SC_VOLTAGE_5V);

    /* disable CMDVCC */
    sc_power_cmd(DISABLE);

    /* set RSTIN high */
    sc_reset(RESET);
}

/*!
    \brief      detect whether the smartcard is present or not
    \param[in]  none
    \param[out] none
    \retval     insert state: 0(smartcard inserted), 1(smartcard not inserted)
*/
uint8_t sc_detect(void)
{
    return 1;
}

/*!
    \brief      enable or disable the power to the smartcard communication 
                data between smartcard and reader
    \param[in]  new_state: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void sc_power_cmd(EventStatus new_state)
{
    if(DISABLE != new_state){
        gpio_bit_set(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
    }else{
        gpio_bit_reset(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
    }
}

/*!
    \brief      set or clear the smartcard reset pin
    \param[in]  pin_state: RESET or SET
    \param[out] none
    \retval     none
*/
void sc_reset(bit_status pin_state)
{
    if(RESET == pin_state){
        /* configure reset pin to low level */
        gpio_bit_reset(SC_RESET_GPIO_PORT, SC_RESET_PIN);
    }else{
        /* configure reset pin to high level */
        gpio_bit_set(SC_RESET_GPIO_PORT, SC_RESET_PIN);
    }
}

/*!
    \brief      configure the card power voltage
    \param[in]  sc_voltage: specifies the card power voltage
      \arg      SC_VOLTAGE_5V: 5V card
      \arg      SC_VOLTAGE_3V: 3V card
    \param[out] none
    \retval     none
*/
void sc_voltage_config(uint32_t sc_voltage)
{
    if(SC_VOLTAGE_5V == sc_voltage){
        /* select smartcard 5V */  
        gpio_bit_set(SC_3_5V_GPIO_PORT, SC_3_5V_PIN);
    }else{
        /* select smartcard 3V */
        gpio_bit_reset(SC_3_5V_GPIO_PORT, SC_3_5V_PIN);
    }
}

/*!
    \brief      get the right value from the D_Table index
    \param[in]  index : index to read from the table 
    \param[out] timeout: time out value
    \retval     value read from the table
*/
uint32_t sc_get_DTable_value(uint8_t index)
{
    return D_table[index];
}

/*!
    \brief      resends the byte that failed to be received(ICCard) correctly
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sc_parity_error_handler(void)
{
    usart_data_transmit(SC_USART, scdata);

    while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET){
    }
}

/*!
    \brief      deinitialize all resources used by the smartcard interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void sc_deinit(void)
{
    /* disable CMDVCC */
    sc_power_cmd(DISABLE);

    /* deinitialize SC_USART */
    usart_deinit(SC_USART);

    /* disable SC_USART clock */
    SC_USART_CLK_DISABLE();
}

/*!
    \brief      decode the answer to reset received from card
    \param[in]  card: pointer to the buffer containing the card ATR information
    \param[out] none
    \retval     none
*/
static uint8_t sc_decode_answer2reset(uint8_t *card)
{
    uint32_t i = 0U, flag = 0U, buf = 0U, protocol = 0U;

    /* initial character */
    sc_A2R.ts = card[0];

    /* format character */
    sc_A2R.t0 = card[1];

    sc_A2R.hlength = sc_A2R.t0 & (uint8_t)0x0FU;

    if(0x80U == (sc_A2R.t0 & (uint8_t)0x80U)){
        flag = 1U;
    }

    for(i = 0U; i < 4U; i++){
        sc_A2R.tlength = sc_A2R.tlength + (((sc_A2R.t0 & (uint8_t)0xF0U) >> (4U + i)) & (uint8_t)0x1U);
    }

    for(i = 0U; i < sc_A2R.tlength; i++) {
        sc_A2R.t[i] = card[i + 2];
    }

    if(0x0U == (sc_A2R.t0 & (uint8_t)0x80U)){
        protocol = 0U;
    }else{
        protocol = sc_A2R.t[sc_A2R.tlength - 1U] & (uint8_t)0x0FU;
    }

    while(flag){
        if (0x80U == (sc_A2R.t[sc_A2R.tlength - 1U] & (uint8_t)0x80U)){
            flag = 1U;
        }else{
            flag = 0U;
        }

        buf = sc_A2R.tlength;
        sc_A2R.tlength = 0U;

        for (i = 0U; i < 4U; i++){
            sc_A2R.tlength = sc_A2R.tlength + (((sc_A2R.t[buf - 1U] & (uint8_t)0xF0) >> (4U + i)) & (uint8_t)0x1U);
        }

        for (i = 0U;i < sc_A2R.tlength; i++){
            sc_A2R.t[buf + i] = card[i + 2U + buf];
        }
        sc_A2R.tlength += (uint8_t)buf;
    }

    for (i = 0U; i < sc_A2R.hlength; i++){
        sc_A2R.h[i] = card[i + 2U + sc_A2R.tlength];
    }

  return (uint8_t)protocol;
}

/*!
    \brief      request the reset answer from card
    \param[in]  scstate: smartcard state
    \param[out] card: pointer to a buffer which will contain the card ATR information
    \param[in]  length: maximum ATR length
    \retval     none
*/
static void sc_answer_req(sc_state_enum *scstate, uint8_t *card, uint8_t length)
{
    uint8_t data = 0U;
    uint16_t i = 0U;
    ATR_length = 0U;

    switch(*scstate){
    case SC_RESET_LOW:
        sc_reset(RESET);
        /* check response with reset low */
        for(i = 0U; i < length; i++){
            if(SUCCESS == usart_byte_receive(&data, SC_RECEIVE_TIMEOUT)){
                card[i] = data;
                ATR_length++;
            }
        }

        if(0x0U != card[0]){
            (*scstate) = SC_ACTIVE;
        }else{
            (*scstate) = SC_RESET_HIGH;
        }
        break;

    case SC_RESET_HIGH:
        sc_reset(SET);
        /* check response with reset high */
        for(i = 0U; i < length; i++){
            if(SUCCESS == usart_byte_receive(&data, SC_RECEIVE_TIMEOUT)){
                card[i] = data;
                ATR_length++;
            }
        }

        if(card[0]){
            (*scstate) = SC_ACTIVE;
        }else{
            (*scstate) = SC_POWER_OFF;
        }
        break;

    case SC_ACTIVE:
        break;

    case SC_POWER_OFF:
        /* if no answer data is received, close the connection */
        sc_reset(SET);
        sc_power_cmd(DISABLE);
        break;

    default:
        (*scstate) = SC_RESET_LOW;
        break;
    }
}

/*!
    \brief      manages the smartcard transport layer: send APDU commands and receives
                the APDU response.
    \param[in]  sc_APDU: pointer to a sc_APDU_commands structure which will be initialized
    \param[out] sc_response_status: pointer to a sc_APDU_response structure which will be initialized
    \retval     none
*/
static void sc_send_data(sc_APDU_commands *sc_APDU, sc_APDU_response *sc_response_status)
{
    uint32_t i = 0U;
    uint8_t locData = 0U;

    uint8_t command[5] = {0};

    /* reset response buffer */
    for(i = 0; i < LC_MAX; i++){
        sc_response_status->data[i] = 0U;
    }

    sc_response_status->sw1 = 0U;
    sc_response_status->sw2 = 0U;

    /* send header */
    command[0] = sc_APDU->header.cla;
    command[1] = sc_APDU->header.ins;
    command[2] = sc_APDU->header.p1;
    command[3] = sc_APDU->header.p2;

    /* enable the DMA receive (set DMAR bit only) to enable 
       interrupt generation in case of a framing error FE */
    usart_dma_receive_config(SC_USART, USART_DENR_ENABLE);

    /* send body length to/from SC */
    if(sc_APDU->body.lc){
        command[4] = sc_APDU->body.lc;
        for(i = 0U; i < 5U; i++){
            scdata = command[i];
            usart_data_transmit(SC_USART, scdata);
            while(RESET == usart_flag_get(SC_USART, USART_FLAG_TC));
        }
    }else if(sc_APDU->body.le){
        command[4] = sc_APDU->body.le;
        for(i = 0; i < 5; i++){
            scdata = command[i];
            usart_data_transmit(SC_USART, scdata);
            while(RESET == usart_flag_get(SC_USART, USART_FLAG_TC));
        }
    }

    /* flush the smartcard data register */
    usart_data_receive(SC_USART);

    /* wait procedure byte from card */
    if(SUCCESS == usart_byte_receive((uint8_t *)&locData, 0xFFFFFU)){
        if((0x60 == (locData & (uint8_t)0xF0)) || (0x90 == (locData & (uint8_t)0xF0))){
            /* sw1 received */
            sc_response_status->sw1 = locData;

            if(SUCCESS == usart_byte_receive((uint8_t *)&locData, SC_RECEIVE_TIMEOUT)){
                /* sw2 received */
                sc_response_status->sw2 = locData;
            }
        }else if(((locData & (uint8_t)0xFEU) == (((uint8_t)~(sc_APDU->header.ins)) & (uint8_t)0xFEU)) || \
                ((locData & (uint8_t)0xFEU) == (sc_APDU->header.ins & (uint8_t)0xFEU))){
            sc_response_status->data[0] = locData; /* ACK received */
        }
    }

    /* if no status bytes received */
    if(0x0U == sc_response_status->sw1){
        /* send body data to smartcard */
        if(sc_APDU->body.lc){
            /* send body data */
            for(i = 0U; i < sc_APDU->body.lc; i++){
                scdata = sc_APDU->body.data[i];
                usart_data_transmit(SC_USART, scdata);
                while(RESET == usart_flag_get(SC_USART, USART_FLAG_TC));
            }

            /* flush the smartcard data register */
            usart_data_receive(SC_USART);

            /* Disable the DMA Receive (Reset DMAR bit only) */  
            usart_dma_receive_config(SC_USART, USART_DENR_DISABLE);
        }else if(sc_APDU->body.le){
            /* receive body data from smartcard */
            for(i = 0U; i < sc_APDU->body.le; i++){
                usart_byte_receive((uint8_t *)&sc_response_status->data[i], SC_RECEIVE_TIMEOUT);
            }
        }

        i = 0U;
        while(i < 10U)
        {
            /* wait sw1 */
            if(SUCCESS == usart_byte_receive(&locData, SC_RECEIVE_TIMEOUT)){
                sc_response_status->sw1 = locData;
                i = 11U;
            }else{
                i++;
            }
        }

        i = 0U;
        while(i < 10U)
        {
            /* wait sw2 */
            if(SUCCESS == usart_byte_receive(&locData, SC_RECEIVE_TIMEOUT)){
                sc_response_status->sw2 = locData;
                i = 11U;
            }else{
                i++;
            }
        }
    }
}

/*!
    \brief      Receives a new data while the time out not elapsed.
    \param[in]  data: data of received
    \param[out] timeout: time out value
    \retval     ErrorStatus value:
                - SUCCESS: new data has been received
                - ERROR: time out was elapsed and no further data is received
*/
static ErrStatus usart_byte_receive(uint8_t *data, uint32_t timeout)
{
    uint32_t counter = 0U;

    while((RESET == usart_flag_get(SC_USART, USART_FLAG_RBNE)) && (counter != timeout)){
        counter++;
    }

    if(counter != timeout){
        *data = (uint8_t)usart_data_receive(SC_USART);
        return SUCCESS;
    }else{
        return ERROR;
    }
}
