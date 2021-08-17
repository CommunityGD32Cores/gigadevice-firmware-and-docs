/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "los_memstat.inc"

#include "los_task.ph"

#include "los_config.h"

typedef struct {
    UINT32 uwMemUsed;
} TSK_MEM_USED_INFO;

LITE_OS_SEC_BSS_MINOR TSK_MEM_USED_INFO g_TskMemUsedInfo[LOSCFG_BASE_CORE_TSK_LIMIT + 1];

LITE_OS_SEC_TEXT_MINOR VOID osTaskMemUsedInc(UINT32 uwUsedSize)
{
    UINT32 taskId;

    if (NULL == g_stLosTask.pstRunTask)
    {
        return;
    }

    if (OS_INT_ACTIVE)
    {
        return;
    }

    taskId = (UINT32) g_stLosTask.pstRunTask->uwTaskID;

    if (taskId > LOSCFG_BASE_CORE_TSK_LIMIT)
    {
        return;
    }

    g_TskMemUsedInfo[taskId].uwMemUsed += uwUsedSize;
}

LITE_OS_SEC_TEXT_MINOR VOID osTaskMemUsedDec(UINT32 uwUsedSize)
{
    UINT32 taskId;

    taskId = (UINT32) g_stLosTask.pstRunTask->uwTaskID;

    if (taskId > LOSCFG_BASE_CORE_TSK_LIMIT)
    {
        return;
    }

    if (OS_INT_ACTIVE)
    {
        return;
    }

    g_TskMemUsedInfo[taskId].uwMemUsed -= uwUsedSize;
}

LITE_OS_SEC_TEXT_MINOR UINT32 osTaskMemUsage(UINT32 uwTaskId)
{
    if ((UINT32)uwTaskId > LOSCFG_BASE_CORE_TSK_LIMIT)
    {
        return LOS_NOK;
    }

    return g_TskMemUsedInfo[(UINT32)uwTaskId].uwMemUsed;
}
