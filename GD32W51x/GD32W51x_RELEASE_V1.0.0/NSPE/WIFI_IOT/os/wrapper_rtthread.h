/*!
    \file    wrapper_rtthread.h
    \brief   RTThread wrapper for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#ifndef __WRAPPER_RTTHREAD_H
#define __WRAPPER_RTTHREAD_H

/*============================ INCLUDES ======================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rtconfig.h>
#include <rthw.h>
#include <rtthread.h>


/*============================ MACROS ========================================*/
#if RT_VERSION < 3
#warning "Target rt-thread version 3.0.0 or above, other versions are not fully tested"
#endif

/* Enable them if use OS's provided system ISR implementations */
#define SYS_HARDFAULT_HANDLER
#define SYS_SVC_HANDLER
#define SYS_PENDSV_HANDLER
#define SYS_SYSTICK_HANDLER

/* Priority: 0 ~ 32 (RT_THREAD_PRIORITY_MAX).
 * The priority of idle task is RT_THREAD_PRIORITY_MAX - 1. */
#define TASK_PRIO_APP_BASE            16
#define TASK_PRIO_WIFI_BASE           16
#define TASK_PRIO_IDLE                (RT_THREAD_PRIORITY_MAX - 1)

#define OS_TICK_RATE_HZ               RT_TICK_PER_SECOND
#define OS_MS_PER_TICK                (1000 / RT_TICK_PER_SECOND)

/*============================ MACRO FUNCTIONS ===============================*/
/* Declare a variable to hold current interrupt status to restore it later */
#define SYS_SR_ALLOC()                register rt_base_t __sr_temp
/* Disable interrupts (nested) */
#define SYS_CRITICAL_ENTER()          do {__sr_temp = rt_hw_interrupt_disable();}while(0)
/* Enable interrupts (nested) */
#define SYS_CRITICAL_EXIT()           do {rt_hw_interrupt_enable(__sr_temp);}while(0)


/* OS IRQ service hook called just after the ISR starts */
#define sys_int_enter()               rt_interrupt_enter()
/* OS IRQ service hook called before the ISR exits */
#define sys_int_exit()                rt_interrupt_leave()

#define TASK_PRIO_HIGHER(n)           (-n)
#define TASK_PRIO_LOWER(n)            (n)

/*============================ TYPES =========================================*/
/*
 * All wrapped new types are pointers to hold dynamically allocated
 * objects except for wrapped task type which we will use for static objects,
 * so keep it as a struct type.
 * In principle, we should nullify all pointers after they are
 * freed or deleted to make them available for sys_xxx_valid macros.
 */
typedef rt_sem_t os_sema_t;
typedef rt_mutex_t os_mutex_t;
typedef struct rt_thread os_task_t;
typedef rt_timer_t os_timer_t;
typedef rt_mq_t os_queue_t;

typedef void (*task_func_t)(void *argv);
typedef void (*timer_func_t)(void *p_tmr, void *p_arg);

typedef struct timer_wrapper {
    struct rt_timer os_timer;
    void *p_arg;
    timer_func_t timer_func;
} timer_wrapper_t;

typedef struct task_wrapper {
    rt_thread_t task_handle;
    os_queue_t task_queue;
    int cur_msg_num;
    int max_msg_num;
} task_wrapper_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#endif //#ifndef __WRAPPER_RTTHREAD_H
