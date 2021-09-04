/*!
    \file  enet_link_check.c
    \brief plug code of network cable state auto check 

    \version 2018-7-11, V1.0.0, plug for 
             GD32F107C/GD32F207C/GD32F207I/GD32F307C/GD32F450Z EVAL board
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "enet_link_check.h"
#include <stdio.h>

/* 
    [Readme]:
    (#)enet_link_check.c/.h provides a way to get the network cable link status and 
    call callback function which defining by user when the link status has changed.
    (#)It is compatible with the ENET demo in GD32F107C/GD32F207C/GD32F207I/GD32F307C/
    GD32F450Z EVAL board. User only needs add this file to project and include head file
    in where user need to call corresponding functions. 
    
    ===============================================================================
                            ##### How to use it #####
    ===============================================================================
    [Example]:
    (+)1 step:
        add it to your project.
    (+)2 step:
        include "enet_link_check.h" in where you need to call it.
    (+)3 step:
        call enet_link_check_init(arg1, arg2) after enet module has initialized(due to 
        link check need communication with phy chip to get link status).
        (++)arg1: callback function, it will be auto call when link status changes.
            void user_defined_callback(enet_link_state_enum state)
            {
                if(ENET_LINK_STATE_UP == state){
                    
                    //the link status changes to link up
                    //do something
                }else{
                    
                    //the link status changes to link down
                    //do something
                }
            }
        (++)arg2: mechanism of getting link status, for DP83848 chip. It has 3 method,
            alternatives include the poll by pin, poll by phy register flag, exti interrupt.
    (+)4 step:
        loop call enet_link_check_poll(), except to get status by interrupt mode(ENET_LINK_CHECK_INT)
    
    ===============================================================================
                ##### How to modify it according to program yourself #####
    ===============================================================================
    (#)according to your phy chip and circuit design, you need modify "enet_link_check.h"
    to make sure code is correct for board yourself.
*/

static link_check_cb _p_link_callback;
static enet_link_state_enum _g_last_link_state = ENET_LINK_STATE_DOWN;
static enet_link_check_enum _g_link_type = ENET_LINK_CHECK_RESET;

static void _link_gpio_init(void);
static void _link_exti_init(void);
static enet_link_state_enum _link_state_reg_read(void);
static void _link_pin_recovery(void);
static void _link_pin_enable(void);
static uint8_t _link_pin_poll(void);
static uint8_t _link_reg_poll(void);

/*!
    \brief      network cable link status check initialize 
    \param[in]  p_func: the pointer of callback function, it will be call when link status changes
    \param[in]  type: mechanism of getting link status
        \arg        ENET_LINK_CHECK_INT: get status by EXTI interrupt(the int pin level of phy chip 
                                         will toggle, if link status changes)
        \arg        ENET_LINK_CHECK_PIN_POLL: get status by get int pin level of phy chip periodically
        \arg        ENET_LINK_CHECK_REG_POLL: get status by get flag of phy register periodically
    \param[out] none
    \retval     none
*/ 
void enet_link_check_init(link_check_cb p_func, enet_link_check_enum type)
{
    _p_link_callback = p_func;
    _g_link_type = type;
    
    switch(type){
        case ENET_LINK_CHECK_INT:
            _link_pin_enable();
            _link_pin_recovery();
            _link_gpio_init();
            _link_exti_init();
            break;
        
        case ENET_LINK_CHECK_PIN_POLL:
            _link_pin_enable();
            _link_pin_recovery();
            _link_gpio_init();
            break;
        
        case ENET_LINK_CHECK_REG_POLL:
            _g_last_link_state = _link_state_reg_read();
            break;
        case ENET_LINK_CHECK_RESET:
            printf("enet link check type invalid \r\n");
            while(1);
    }
}

/*!
    \brief      poll to check the link status
    \param[in]  none
    \param[out] none
    \retval     uint8_t: 0:link status has not changed / 1:link status has changed
*/ 
uint8_t enet_link_check_poll(void)
{
    uint8_t reval = 0;
    
    switch( _g_link_type){
        case ENET_LINK_CHECK_INT:
            printf("int type should't use poll function \r\n");
            break;
        
        case ENET_LINK_CHECK_PIN_POLL:
            reval = _link_pin_poll();
            break;
        
        case ENET_LINK_CHECK_REG_POLL:
            reval = _link_reg_poll();
            break;
        case ENET_LINK_CHECK_RESET:
            printf("link check is not initialized yet \r\n");
            break;
    }
    
    return reval;
}

/*!
    \brief      link check interrupt handler 
    \param[in]  none
    \param[out] none
    \retval     none
*/ 
void enet_link_check_irq(void)
{
    enet_link_state_enum link_state;
    
    if(RESET != exti_interrupt_flag_get(LINK_CHECK_EXTI_LINE)){
        exti_interrupt_flag_clear(LINK_CHECK_EXTI_LINE);
        _link_pin_recovery();
        link_state = _link_state_reg_read();
        if(NULL != _p_link_callback){
            _p_link_callback(link_state);
        } 
    }
}

/*!
    \brief      this function handles EXTI used as link check exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LINK_CHECK_IRQ_HANDLE(void)
{
    enet_link_check_irq();
}

static void _link_gpio_init(void)
{
    rcu_periph_clock_enable(LINK_CHECK_GPIO_CLK);
    LINK_CHECK_GPIO_CONFIG;
}

static void _link_exti_init(void)
{                           
    rcu_periph_clock_enable(LINK_CHECK_EXTI_CLK);

    /* enable and set EXTI interrupt to the lowest priority */
    nvic_irq_enable(LINK_CHECK_EXTI_IRQn, 0U, 0U);

    /* connect EXTI line to key GPIO pin */
    LINK_CHECK_EXTI_SOURCE_CONFIG(LINK_CHECK_EXTI_PORT_SOURCE, LINK_CHECK_EXTI_PIN_SOURCE);

    /* configure key EXTI line */
#if (RESET == ENET_LINK_CHANGE_LEVEL)
    exti_init(LINK_CHECK_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
#else
    exti_init(LINK_CHECK_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
#endif /* RESET == ENET_LINK_CHANGE_LEVEL */
    
    exti_interrupt_flag_clear(LINK_CHECK_EXTI_LINE);
}

static enet_link_state_enum _link_state_reg_read(void)
{
    uint16_t phy_value = 0U; 
    
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
    phy_value &= PHY_LINKED_STATUS;
    if(0 == phy_value){
        return ENET_LINK_STATE_DOWN;
    }else{
        return ENET_LINK_STATE_UP;
    }
}

static void _link_pin_recovery(void)
{
    uint16_t phy_value; 
    
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_MISR, &phy_value);
}

static void _link_pin_enable(void)
{
    uint16_t phy_value; 
    
    phy_value = PHY_INT_AND_OUTPUT_ENABLE;
    enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_MICR, &phy_value);
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_MICR, &phy_value);
    if(PHY_INT_AND_OUTPUT_ENABLE != phy_value){
        while(1);
    }
    
    phy_value = PHY_LINK_INT_ENABLE;
    enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_MISR, &phy_value);
}

static uint8_t _link_pin_poll(void)
{
    uint8_t reval = 0;
    enet_link_state_enum link_state;
    if(ENET_LINK_CHANGE_LEVEL == gpio_input_bit_get(LINK_CHECK_GPIO_PORT, \
                                                      LINK_CHECK_GPIO_PIN)){
        _link_pin_recovery();
        reval = 1;
        link_state = _link_state_reg_read();
        if(NULL != _p_link_callback){
            _p_link_callback(link_state);
        }
    }
    
    return reval;
}

static uint8_t _link_reg_poll(void)
{
    enet_link_state_enum link_state;
    uint8_t reval = 0;
    
    link_state = _link_state_reg_read();
    if(link_state != _g_last_link_state){
        reval = 1;
        _g_last_link_state = link_state;
        
        if(NULL != _p_link_callback){
            _p_link_callback(link_state);
        }
    }
    
    return reval;    
}
