#include "los_bsp_led.h"
#include "gd32e103v_eval.h"

/******************************************************************************
    here include some special hearder file you need
******************************************************************************/



/*****************************************************************************
 Function    : LOS_EvbLedInit
 Description : Init LED device
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbLedInit(void)
{
    //add you code here.
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);
}

/*************************************************************************************************
 *  Function    £ºLOS_EvbLedControl
 *  Discription : Led control function
 *  Input       : (1) index Led's index                                                                  *
 *                (2) cmd   Led on or off  
 *  Output      : None
 *  Return      : None                                                                                *
 *                                                                                 *
**************************************************************************************************/
void LOS_EvbLedControl(int index, int cmd)
{
    switch (index){
        case LOS_LED1:
        {
            if (cmd == LED_ON)
            {
                //add you code here.
                /*led1 on */
                gd_eval_led_on(LED2);
            }
            else
            {
                //add you code here.
                gd_eval_led_off(LED2);
            }
            break;
        }
        case LOS_LED2:
        {
            if (cmd == LED_ON)
            {
                //add you code here.
                gd_eval_led_on(LED3);
            }
            else
            {
                //add you code here.
                gd_eval_led_off(LED3);  
            }
            break;
        }
        case LOS_LED3:
        {
            if (cmd == LED_ON)
            {    
                gd_eval_led_on(LED4);
            }
            else
            {
                //add you code here.  
                gd_eval_led_off(LED4);
            }
            break;
        }
        case LOS_LED4:
        {
            if (cmd == LED_ON)
            {    
                gd_eval_led_on(LED5);
            }
            else
            {
                //add you code here.  
                gd_eval_led_off(LED5);
            }
            break;
        }
        
        default:
        {
            break;
        }
    }
}


