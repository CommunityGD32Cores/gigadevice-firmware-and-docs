#ifndef _LOS_BSP_KEY_H
#define _LOS_BSP_KEY_H

#include "los_event.h"

#define DEV_KEY_EVENT   0x00000001u

#define LOS_KEY_PRESS   0
#define LOS_KEY1    1

extern EVENT_CB_S g_Key_Event;

void LOS_EvbKeyInit(void);
unsigned int LOS_EvbGetKeyVal(int KeyNum);

#endif

