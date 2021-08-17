#include "los_bsp_key.h"
#include "gd32e103v_eval.h"

#include "los_printf.h"

/******************************************************************************
    here include some special hearder file you need
******************************************************************************/
EVENT_CB_S g_Key_Event; 



/*****************************************************************************
 Function    : LOS_EvbKeyInit
 Description : Init GIOP Key 
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbKeyInit(void)
{
    //add you code here.
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);
}

/*****************************************************************************
 Function    : LOS_EvbGetKeyVal
 Description : Get GIOP Key value
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
unsigned int LOS_EvbGetKeyVal(int KeyNum)
{
    unsigned int KeyVal = 0xFF;
    
    //add you code here.
    switch(KeyNum){
        case LOS_KEY1:
        {
            KeyVal = gd_eval_key_state_get(KEY_USER);
            break;
        }
        default:
        {
            break;
        }
    }


    return KeyVal;
}

/*****************************************************************************
 Function    : EXTIxxx_IRQHandler
 Description : EXT xxx IRQ Handler detail,change the function name 
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void EXTI10_15_IRQHandler(void)
{
    uint32_t uwRet;
    
    //add you code here.
    if(RESET != exti_interrupt_flag_get(USER_KEY_EXTI_LINE)){
        exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);
        uwRet = LOS_EventWrite(&g_Key_Event, DEV_KEY_EVENT);
        if(uwRet != LOS_OK)
        {
            PRINT_ERR("event write failed .\r\n");
        }
    }
    return;
}
