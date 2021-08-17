/*!
    \file  smartcard.c
    \brief This file provides all the Smartcard firmware functions
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/
  
#include "smartcard.h"
#include <stdio.h>

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
SC_ATR SC_A2R;
uint8_t SC_ATR_Table[40];
uint8_t AtrLength = 0;
static __IO uint8_t SCData = 0;

/* F Table */
static uint32_t F_Table[16] = {372, 372, 558, 744, 1116, 1488, 1860, 0, 0, 512, 768,
                               1024, 1536, 2048, 0, 0};

/* D Table */
static uint32_t D_Table[16] = {0, 1, 2, 4, 8, 16, 32, 64, 12, 20, 0, 0, 0, 0, 0, 0};

/* Private function prototypes -----------------------------------------------*/
static void SC_SendData  (SC_ADPU_Commands *SC_ADPU, 
                          SC_ADPU_Response *SC_ResponseStatus);

static void SC_AnswerReq (SC_State *SCState, 
                          uint8_t *card, 
                          uint8_t length);  /* Ask ATR */

static uint8_t SC_decode_Answer2reset(uint8_t *card);  /* Decode ATR */

/* Physical Port Layer -------------------------------------------------------*/
static void SC_Init                 (void);
static void SC_DeInit               (void);
static ErrStatus USART_ByteReceive  (uint8_t *Data, uint32_t TimeOut);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handles all Smartcard states and serves to send and receive all
  *         communication data between Smartcard and reader.
  * @param  SCState: pointer to an SC_State enumeration that will contain the 
  *         Smartcard state.
  * @param  SC_ADPU: pointer to an SC_ADPU_Commands structure that will be initialized.  
  * @param  SC_Response: pointer to a SC_ADPU_Response structure which will be initialized.
  * @retval None
  */
void SC_Handler (SC_State *SCState, 
                 SC_ADPU_Commands *SC_ADPU, 
                 SC_ADPU_Response *SC_Response)
{
    uint32_t i, j = 0;

    switch(*SCState)
    {
        case SC_POWER_ON:
            if (SC_ADPU->Header.INS == SC_GET_A2R)
            {
                /* Smartcard initialization ------------------------------------------*/
                SC_Init();

                /* Reset Data from SC buffer -----------------------------------------*/
                for (i = 0; i < 40; i++)
                {
                    SC_ATR_Table[i] = 0;
                }

                /* Reset SC_A2R Structure --------------------------------------------*/
                SC_A2R.TS = 0;
                SC_A2R.T0 = 0;

                for (i = 0; i < MAX_PROTOCOLLEVEL; i++)
                {
                    for (j = 0; j < MAX_INTERFACEBYTE; j++)
                    {
                        SC_A2R.T[i].InterfaceByte[j].Status = 0;
                        SC_A2R.T[i].InterfaceByte[j].Value = 0;
                    }
                }

                for (i = 0; i < HIST_LENGTH; i++)
                {
                    SC_A2R.H[i] = 0;
                }

                SC_A2R.Tlength = 0;
                SC_A2R.Hlength = 0;

                /* Next State --------------------------------------------------------*/
                *SCState = SC_RESET_LOW;
            }
            break;

        case SC_RESET_LOW:
            if(SC_ADPU->Header.INS == SC_GET_A2R)
            {
                /* If card is detected then Power ON, Card Reset and wait for an answer) */
                if (SC_Detect())
                {
                    while(((*SCState) != SC_POWER_OFF) && ((*SCState) != SC_ACTIVE))
                    {
                        SC_AnswerReq(SCState, &SC_ATR_Table[0], 40); /* Check for answer to reset */
                    }
                }
                else
                {
                    (*SCState) = SC_POWER_OFF;
                }
            }
            break;

        case SC_ACTIVE:
            if (SC_ADPU->Header.INS == SC_GET_A2R)
            {
                if(SC_decode_Answer2reset(&SC_ATR_Table[0]) == T0_PROTOCOL)
                {
                    (*SCState) = SC_ACTIVE_ON_T0;
                }
                else
                {
                    (*SCState) = SC_POWER_OFF; 
                }
            }
            break;

        case SC_ACTIVE_ON_T0:
            /* process commands other than ATR here */
            SC_SendData(SC_ADPU, SC_Response);
            break;

        case SC_POWER_OFF:
            SC_DeInit(); /* Disable Smartcard interface */
            break;

        default: 
            (*SCState) = SC_POWER_OFF;
            break;
    }
}

/**
  * @brief  Enables or disables the power to the Smartcard.
  * @param  NewState: new state of the Smartcard power supply. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SC_PowerCmd(ControlStatus NewState)
{
    if(NewState != DISABLE)
    {
        gpio_bit_reset(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
    }
    else
    {
        gpio_bit_set(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
    }
}

/**
  * @brief  Sets or clears the Smartcard reset pin.
  * @param  ResetState: this parameter specifies the state of the Smartcard 
  *         reset pin. BitVal must be one of the BitAction enum values:
  *                 @arg RESET: to clear the port pin.
  *                 @arg SET: to set the port pin.
  * @retval None
  */
void SC_Reset(FlagStatus ResetState)
{
    gpio_bit_write(SC_RESET_GPIO_PORT, SC_RESET_PIN, ResetState);
}

/**
  * @brief  Resends the byte that failed to be received (by the Smartcard) correctly.
  * @param  None
  * @retval None
  */
void SC_ParityErrorHandler(void)
{
    usart_data_transmit(SC_USART, SCData);

    while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
    {
    }
}

void PPS_BaudRate_Set(uint32_t rate)
{
	
  usart_disable(SC_USART);
  /*Configure the initial baudrate*/
	usart_baudrate_set(SC_USART, rate);
	usart_parity_config(SC_USART, USART_PM_EVEN);
	usart_word_length_set(SC_USART, USART_WL_9BIT);
	usart_stop_bit_set(SC_USART, USART_STB_1_5BIT);
	usart_transmit_config(SC_USART,USART_TRANSMIT_ENABLE);
	usart_receive_config(SC_USART, USART_RECEIVE_ENABLE);
  
  /* Enable SC_USART */
  usart_enable(SC_USART); 
 
}

/**
  * @brief  Configures the IO speed (BaudRate) communication.
  * @param  None
  * @retval None
  */
void SC_PTSConfig(void)
{
    uint32_t workingbaudrate = 0, apbclock = 0;
    uint8_t locData = 0, PPSConfirmStatus = 1;

    /* Reconfigure the USART Baud Rate -------------------------------------------*/
    apbclock = rcu_clock_freq_get(CK_USART);
	  apbclock /= ((USART_GP(SC_USART) & (uint16_t)0x00FF) * 2);
	
    
    /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
       in case of a framing error FE */
	  usart_dma_receive_config(SC_USART, USART_DENR_ENABLE);

    if((SC_A2R.T0 & (uint8_t)0x10) == 0x10)
    {
        if(SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value != 0x11)
        {
            /* PPSS identifies the PPS request or response and is equal to 0xFF */
            SCData = 0xFF;

            /* Send PPSS */
            usart_data_transmit(SC_USART, SCData);

            while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
            {
            }

            /* PPS0 indicates by the bits b5, b6, b7 equal to 1 the presence of the optional
               bytes PPSI1, PPS2, PPS3 respectively */
            SCData = 0x10;

            /* Send PPS0 */
            usart_data_transmit(SC_USART, SCData);
						
            while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
            {
            }

            /* PPS1 allows the interface device to propose value of F and D to the card */
            SCData = SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value;

            /* Send PPS1 */
            usart_data_transmit(SC_USART, SCData);

            while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
            {
            }

            /* PCK check character */ 
            SCData = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value; 

            /* Send PCK */
            usart_data_transmit(SC_USART, SCData);

            while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
            {
            }

            /* Disable the DMA Receive (Reset DMAR bit only) */  
						usart_dma_receive_config(SC_USART, USART_DENR_DISABLE);

            /* PPSS response = PPSS request = 0xFF*/
            if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
            {
                if(locData != 0xFF)
                {
                    /*PPSS exchange unsuccessful */
                    PPSConfirmStatus = 0x00;
                }
            }

            /*PPS0 response */
            if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
            {
                if(locData != 0x10)
                {
                    /*PPS0 exchange unsuccessful */
                    PPSConfirmStatus = 0x00;
                }
            }

            /* PPS1 response */
            if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
            {
                if(locData != SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value)
                {
                    /*PPS1 exchange unsuccessful */
                    PPSConfirmStatus = 0x00;
                }
            }

            if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
            {
                if(locData != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value))
                {
                    /*PCK exchange unsuccessful */
                    PPSConfirmStatus = 0x00;
                }
            }
            else
            {
                /*PPS exchange unsuccessful */
                PPSConfirmStatus = 0x00;
            }

            /* PPS exchange successful */
            if(PPSConfirmStatus == 0x01)
            {
                workingbaudrate = apbclock * D_Table[(SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value & (uint8_t)0x0F)];
                workingbaudrate /= F_Table[((SC_A2R.T[0].InterfaceByte[SC_INTERFACEBYTE_TA].Value >> 4) & (uint8_t)0x0F)];

                PPS_BaudRate_Set(workingbaudrate);
            }
        }
    }
}

/**
  * @brief  Manages the Smartcard transport layer: send APDU commands and receives
  *   the APDU responce.
  * @param  SC_ADPU: pointer to a SC_ADPU_Commands structure which will be initialized.  
  * @param  SC_Response: pointer to a SC_ADPU_Response structure which will be initialized.
  * @retval None
  */
static void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Response *SC_ResponseStatus)
{
    uint32_t i = 0;
    uint8_t locData = 0;
	  //uint8_t counter = 0;

    /* Reset responce buffer ---------------------------------------------------*/
    for(i = 0; i < LC_MAX; i++)
    {
        SC_ResponseStatus->Data[i] = 0;
    }

    SC_ResponseStatus->SW1 = 0;
    SC_ResponseStatus->SW2 = 0;

    /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
       in case of a framing error FE */
		usart_dma_receive_config(SC_USART, USART_DENR_ENABLE);

    /* Send header -------------------------------------------------------------*/
    SCData = SC_ADPU->Header.CLA;
    usart_data_transmit(SC_USART, SCData);
    while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
    {
    }

    SCData = SC_ADPU->Header.INS;
    usart_data_transmit(SC_USART, SCData);
    while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
    {
    }

    SCData = SC_ADPU->Header.P1;
    usart_data_transmit(SC_USART, SCData);
    while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
    {
    }

    SCData = SC_ADPU->Header.P2;
    usart_data_transmit(SC_USART, SCData);
    while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
    {
    }

    /* Send body length to/from SC ---------------------------------------------*/
    if(SC_ADPU->Body.LC)
    {
        SCData = SC_ADPU->Body.LC;
        usart_data_transmit(SC_USART, SCData);
        while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
        {
        }
    }
    else if(SC_ADPU->Body.LE)
    { 
        SCData = SC_ADPU->Body.LE;
        usart_data_transmit(SC_USART, SCData);
        while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
        {
        }
    }
    
    /* Flush the SC_USART DR */
    (void)usart_data_receive(SC_USART);
		 

    /* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
    -------------------------------------------------------- */

    if((USART_ByteReceive(&locData, 0xfffff)) == SUCCESS)
    {
        if(((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
        {
            /* SW1 received */
            SC_ResponseStatus->SW1 = locData;

            if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
            {
                /* SW2 received */
                SC_ResponseStatus->SW2 = locData;
            }
        }
        else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & \
                (uint8_t)0xFE))||((locData & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
        {
            SC_ResponseStatus->Data[0] = locData;/* ACK received */
        }
    }

    /* If no status bytes received ---------------------------------------------*/
    if(SC_ResponseStatus->SW1 == 0x00)
    {
        /* Send body data to SC--------------------------------------------------*/
        if (SC_ADPU->Body.LC)
        {
            for(i = 0; i < SC_ADPU->Body.LC; i++)
            {
                SCData = SC_ADPU->Body.Data[i];
                usart_data_transmit(SC_USART, SCData);
                while(usart_flag_get(SC_USART, USART_FLAG_TC) == RESET)
               {
               }
							   
            }

            /* Flush the SC_USART DR */
            (void)usart_data_receive(SC_USART);

            /* Disable the DMA Receive (Reset DMAR bit only) */  
						usart_dma_receive_config(SC_USART, USART_DENR_DISABLE);
        }
        /* Or receive body data from SC ------------------------------------------*/
        else if (SC_ADPU->Body.LE)
        {
            for(i = 0; i < SC_ADPU->Body.LE; i++)
            {
                if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
                {
                    SC_ResponseStatus->Data[i] = locData;
                }
            }
        }

        /* Wait SW1 --------------------------------------------------------------*/
        i = 0;
        while(i < 10)
        {
            if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
            {
                SC_ResponseStatus->SW1 = locData;
                i = 11;
            }
            else
            {
                i++;
            }
        }

        /* Wait SW2 ------------------------------------------------------------*/   
        i = 0;
        while(i < 10)
        {
            if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
            {
                SC_ResponseStatus->SW2 = locData;
                i = 11;
            }
            else
            {
                i++;
            }
        }
    }
}


/**
  * @brief  Requests the reset answer from card.
  * @param  SCState: pointer to an SC_State enumeration that will contain the Smartcard state.
  * @param  card: pointer to a buffer which will contain the card ATR.
  * @param  length: maximum ATR length
  * @retval None
  */
static void SC_AnswerReq(SC_State *SCState, uint8_t *card, uint8_t length)
{
    uint8_t Data = 0;
    uint32_t i = 0;

    switch(*SCState)
    {
        case SC_RESET_LOW:
						SC_Reset(RESET);
            /* Check response with reset low ---------------------------------------*/
            for (i = 0; i < length; i++)
            {
                if((USART_ByteReceive(&Data, SC_RECEIVE_TIMEOUT)) == SUCCESS)
                {
                    card[i] = Data;
                }
            }

            if(card[0])
            {
                (*SCState) = SC_ACTIVE;
                SC_Reset(SET);
            }
            else
            {
                (*SCState) = SC_RESET_HIGH;
            }
            break;

        case SC_RESET_HIGH:
            /* Check response with reset high --------------------------------------*/
            SC_Reset(SET); /* Reset High */

            for (i = 0; i < length; i++)
            {
                if((USART_ByteReceive(&Data, SC_RECEIVE_TIMEOUT)) == SUCCESS)
                {
                    card[i] = Data;
                    AtrLength++;
                }
            }

            while(AtrLength < length)
            {
                card[AtrLength] = 0xff;
                AtrLength++;
            }

            if(card[0])
            {
                (*SCState) = SC_ACTIVE;
            }
            else
            {
                (*SCState) = SC_POWER_OFF;
            }
            break;

        case SC_ACTIVE:
            break;

        case SC_POWER_OFF:
            /* Close Connection if no answer received ------------------------------*/
            SC_PowerCmd(DISABLE);
            break;

        default:
            (*SCState) = SC_RESET_LOW;
            break;
    }
}

/**
  * @brief  Decodes the Answer to reset received from card.
  * @param  card: pointer to the buffer containing the card ATR.
  * @retval None
  */
static uint8_t SC_decode_Answer2reset(uint8_t *card)
{
    uint32_t i = 0, flag = 0, protocol = 0;
    uint8_t index = 0, level = 0;

    /******************************TS/T0 Decode************************************/
    SC_A2R.TS = card[index++];  /* Initial character */
    SC_A2R.T0 = card[index++];  /* Format character */

    /*************************Historical Table Length Decode***********************/
    SC_A2R.Hlength = SC_A2R.T0 & (uint8_t)0x0F;

    /******************************Protocol Level(1) Decode************************/  
    /* Check TD(1) if present */
    if ((SC_A2R.T0 & (uint8_t)0x80) == 0x80)
    {
        flag = 1;
    }

    /* Each bits in the T0 high nibble(b8 to b5) equal to 1 indicates the presence
       of a further interface byte */
    for (i = 0; i < 4; i++)
    {
        if (((SC_A2R.T0 & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1)
        {
            SC_A2R.T[level].InterfaceByte[i].Status = 1;
            SC_A2R.T[level].InterfaceByte[i].Value = card[index++];
            SC_A2R.Tlength++;
        }
    }

    /*****************************T Decode*****************************************/ 
    if (SC_A2R.T[level].InterfaceByte[3].Status == 1)
    {
        /* Only the protocol(parameter T) present in TD(1) is detected
           if two or more values of parameter T are present in TD(1), TD(2)..., so the 
           firmware should be updated to support them */
        protocol = SC_A2R.T[level].InterfaceByte[SC_INTERFACEBYTE_TD].Value  & (uint8_t)0x0F;
    }
    else
    {
        protocol = 0;
    }

    /* Protocol Level Increment */

    /******************************Protocol Level(n>1) Decode**********************/ 
    while (flag)
    {
        if ((SC_A2R.T[level].InterfaceByte[SC_INTERFACEBYTE_TD].Value & (uint8_t)0x80) == 0x80)
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }

        /* Each bits in the high nibble(b8 to b5) for the TD(i) equal to 1 indicates
          the presence of a further interface byte */
        for (i = 0; i < 4; i++)
        {
            if (((SC_A2R.T[level].InterfaceByte[SC_INTERFACEBYTE_TD].Value & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1)
            {
                SC_A2R.T[level+1].InterfaceByte[i].Status = 1;
                SC_A2R.T[level+1].InterfaceByte[i].Value = card[index++];
                SC_A2R.Tlength++;
            }
        }

        level++; 
    }

    for (i = 0; i < SC_A2R.Hlength; i++)
    {
        SC_A2R.H[i] = card[i + 2 + SC_A2R.Tlength];
    }

    /*************************************TCK Decode*******************************/
    SC_A2R.TCK = card[SC_A2R.Hlength + 2 + SC_A2R.Tlength];

    return (uint8_t)protocol;
}

/**
  * @brief  Initializes all peripheral used for Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_Init(void)
{
	  rcu_periph_clock_enable(SC_USART_TX_GPIO_CLK);
	  rcu_periph_clock_enable(SC_USART_CK_GPIO_CLK);

    /* Enable SC_USART clock */
	  rcu_periph_clock_enable(SC_USART_CLK);

    /* Connect PXx to SC_USART_TX */
	  gpio_af_set(SC_USART_TX_GPIO_PORT, SC_USART_TX_AF, SC_USART_TX_SOURCE);

    /* Connect PXx to SC_USART_CK */ 
	  gpio_af_set(SC_USART_CK_GPIO_PORT, SC_USART_CK_AF, SC_USART_CK_SOURCE);

    /* Configure USART Tx and Rx as alternate function push-pull */		
		gpio_output_options_set(SC_USART_CK_GPIO_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,SC_USART_CK_PIN);
		gpio_mode_set(SC_USART_CK_GPIO_PORT,GPIO_MODE_AF,GPIO_PUPD_PULLUP,SC_USART_CK_PIN);

    /* Configure USART Tx pin as alternate function open-drain */
    gpio_output_options_set(SC_USART_TX_GPIO_PORT,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,SC_USART_TX_PIN);
		gpio_mode_set(SC_USART_TX_GPIO_PORT,GPIO_MODE_AF,GPIO_PUPD_PULLUP,SC_USART_TX_PIN);
 		
		//nvic_priority_group_set(uint32_t nvic_prigroup);
		 nvic_irq_enable(SC_USART_IRQn, 0, 0);

    /* SC_USART configuration ----------------------------------------------------*/
    /* SC_USART configured as follow:
        - Word Length = 9 Bits
        - 1.5 Stop Bit
        - Even parity
        - BaudRate = 8064 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Tx and Rx enabled
        - USART Clock enabled
    */

    /* USART Clock set to 4 MHz (PCLK2 (72 MHz) / 18) */
    usart_prescaler_config(SC_USART, 0x09);
    /* USART Guard Time set to 16 Bit */
		usart_guard_time_config(SC_USART, 16);
    
	  usart_synchronous_clock_config(SC_USART, USART_CLEN_EN, USART_CPH_1CK, USART_CPL_LOW);
    /*Configure the initial baudrate*/
		
	  usart_baudrate_set(SC_USART, 10753);
	  usart_parity_config(SC_USART, USART_PM_EVEN);
	  usart_word_length_set(SC_USART, USART_WL_9BIT);
	  usart_stop_bit_set(SC_USART, USART_STB_1_5BIT);
		usart_transmit_config(SC_USART,USART_TRANSMIT_ENABLE);
		usart_receive_config(SC_USART, USART_RECEIVE_ENABLE);

    /* Enable the SC_USART Parity Error Interrupt */
    usart_interrupt_enable(SC_USART, USART_INT_PERR);
    /* Enable the SC_USART Framing Error Interrupt */
    usart_interrupt_enable(SC_USART, USART_INT_ERR);
    
		usart_clock_enable(SC_USART);
    /* Enable the NACK Transmission */
		usart_smartcard_mode_nack_enable(SC_USART);

    /* Enable the Smartcard Interface */
		usart_smartcard_mode_enable(SC_USART);

    /*Configure the auto retry*/
		usart_smartcard_autoretry_config(SC_USART, 0x06);

    /* Enable SC_USART */
    usart_enable(SC_USART);
}

/**
  * @brief  Deinitializes all resources used by the Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_DeInit(void)
{
    /* Disable CMDVCC */
    SC_PowerCmd(DISABLE);

    /* Deinitializes the SC_USART */
    usart_deinit(SC_USART);

    /* Disable SC_USART clock */
    rcu_periph_clock_disable(SC_USART_CLK);
}

/**
  * @brief  Configures the card power voltage.
  * @param  SC_Voltage: specifies the card power voltage.
  *         This parameter can be one of the following values:
  *              @arg SC_VOLTAGE_5V: 5V cards.
  *              @arg SC_VOLTAGE_3V: 3V cards.
  * @retval None
  */
void SC_VoltageConfig(uint32_t SC_Voltage)
{
    if(SC_Voltage == SC_VOLTAGE_5V)
    {
        /* Select Smartcard 5V */  
        gpio_bit_set(SC_3_5V_GPIO_PORT, SC_3_5V_PIN);
    }
    else
    {
        /* Select Smartcard 3V */      
        gpio_bit_reset(SC_3_5V_GPIO_PORT, SC_3_5V_PIN);
    }
}

/**
  * @brief  Detects whether the Smartcard is present or not.
  * @param  None. 
  * @retval 0 - Smartcard inserted
  *         1 - Smartcard not inserted
  */
uint8_t SC_Detect(void)
{
    return 1;
}

/**
  * @brief  Receives a new data while the time out not elapsed.
  * @param  None
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: New data has been received
  *          - ERROR: time out was elapsed and no further data is received
  */
static ErrStatus USART_ByteReceive(uint8_t *Data, uint32_t TimeOut)
{
    uint32_t Counter = 0;

    while((usart_flag_get(SC_USART, USART_FLAG_RBNE) == RESET) && (Counter != TimeOut))
    {
        Counter++;
    }

    if(Counter != TimeOut)
    {
        *Data = (uint8_t)usart_data_receive(SC_USART);

        return SUCCESS;
    }
    else 
    {
        return ERROR;
    }
}

/**
  * @brief  Get the Right Value from the D_Table Index 
  * @param  idx : Index to Read from the Table 
  * @retval Value read from the Table
  */
uint32_t SC_GetDTableValue(uint8_t idx)
{
    return D_Table[idx];
}


PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    usart_data_transmit( USART1 , (uint8_t) ch );
    
    /* Loop until transmit data register is empty */
    while (usart_flag_get( USART1 , USART_FLAG_TBE) == RESET)
    {
    }

    return ch;
}
/************************ (C) COPYRIGHT 2014 GIGADEVICE *****END OF FILE****/
