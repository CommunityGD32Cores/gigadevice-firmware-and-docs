/*!
    \file    main.c
    \brief   LiteOS demo
    
    \version 2018-03-26, V1.0.0, demo for GD32E103
    \version 2020-09-30, V1.1.0, demo for GD32E103
    \version 2020-12-31, V1.2.0, demo for GD32E103
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

#include "gd32e103v_eval.h"
#include "systick.h"

#include "los_task.h"
#include "los_sys.h"
#include "los_tick.h"
#include "los_config.h"
#include "los_printf.h"

#include "los_bsp_adapter.h"
#include "los_bsp_led.h"
#include "los_bsp_key.h"
#include "los_bsp_uart.h"

uint32_t g_uwInitTaskID;
uint32_t g_uwLedTaskID;
uint32_t g_uwKeyTaskID;

/*!
    \brief      key task function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LOS_Key_Tsk(void)
{
    UINT32 uwEvent;
    
    PRINT_RELEASE("Key Task start \r\n");
    while(1){
        /* waiting User key is pressed, timeout value is 5s  */
        uwEvent = LOS_EventRead(&g_Key_Event, DEV_KEY_EVENT, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, 5000);
        if(DEV_KEY_EVENT == uwEvent){
            PRINT_RELEASE("the User key has been pressed \r\n");
            LOS_EvbLedControl(LOS_LED3, LED_ON);
            LOS_TaskDelay(1000);
            LOS_EvbLedControl(LOS_LED3, LED_OFF);
            LOS_TaskDelay(1000);
        }else{
            PRINT_RELEASE("press the User key, LED will be respond \r\n");
        }
    }
}

/*!
    \brief      creat key task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LOS_Key_Tsk_Entry(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)LOS_Key_Tsk;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTaskInitParam.pcName = "InspectTsk";
    stTaskInitParam.usTaskPrio = 7;
    uwRet = LOS_TaskCreate(&g_uwKeyTaskID, &stTaskInitParam);
    
    if (uwRet != LOS_OK)
    {
        return;
    }
    
    uwRet = LOS_EventInit(&g_Key_Event);
    if(uwRet != LOS_OK)
    {
        while(1);
    } 
    
    return;
}

/*!
    \brief      LED task function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LOS_Led_Tsk(void)
{
    while(1){
        PRINT_RELEASE("LED Task running \r\n");
        /* LED blink once per 1s */
        LOS_EvbLedControl(LOS_LED2, LED_ON);
        LOS_TaskDelay(1000);
        LOS_EvbLedControl(LOS_LED2, LED_OFF);
        LOS_TaskDelay(1000);
    }
}

/*!
    \brief      creat LED task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LOS_Led_Tsk_Entry(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)LOS_Led_Tsk;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTaskInitParam.pcName = "InspectTsk";
    stTaskInitParam.usTaskPrio = 8;
    uwRet = LOS_TaskCreate(&g_uwLedTaskID, &stTaskInitParam);
    
    if (uwRet != LOS_OK)
    {
        return;
    }
    
    return;
}

/*!
    \brief      initialize task function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LOS_Init_Tsk(void)
{
    PRINT_RELEASE("\r\n /============== Gigadevice LiteOS Demo ==============/ \r\n");
    LOS_Led_Tsk_Entry();
    LOS_Key_Tsk_Entry();
    LOS_TaskDelete(g_uwInitTaskID);
}

/*!
    \brief      creat init task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LOS_InitTask_Entry(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    /* creat init task */
    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)LOS_Init_Tsk;
    stTaskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName = "InspectTsk";
    stTaskInitParam.usTaskPrio = 9;
    uwRet = LOS_TaskCreate(&g_uwInitTaskID, &stTaskInitParam);
    
    if (uwRet != LOS_OK)
    {
        return;
    }
    
    return;
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    UINT32 uwRet;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE3_SUB1);

    uwRet = LOS_KernelInit();

    /*init LiteOS kernel */
    if (uwRet != LOS_OK) {
        return LOS_NOK;
    }

    /* enable LiteOS system tick interrupt */
    LOS_EnableTick();

    /*
        notice: add your code here
        here you can create task for your function 
        do some hw init that need after systemtick init
    */
    /* init the device on the dev baord */
    LOS_EvbSetup();

    LOS_InitTask_Entry();

    /* kernel start to run */
    LOS_Start();
    for (;;);
}
