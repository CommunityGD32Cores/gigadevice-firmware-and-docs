/*!
    \file    pm_sleep.c
    \brief   Rtthread pm sleep function for GD32W51x WiFi SDK

    \version 2021-08-03, V1.0.0, firmware for GD32W51x
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
#include "system_gd32w51x.h"
#include "wrapper_os.h"
#include "debug_print.h"
#include "bsp_wlan.h"
#include "wlan_intf.h"
#include "wifi_netlink.h"
#include "wakelock.h"
#include <drivers/pm.h>
#include "pm_sleep.h"

#ifdef RT_USING_PM

#define CPU_CLOCK_HZ                    ( SystemCoreClock )
#define SYSTICK_CLOCK_HZ                CPU_CLOCK_HZ
#define RT_NVIC_SYSTICK_CLK_BIT         ( 1UL << 2UL )
#define TICK_RATE_HZ                    ( ( uint32_t ) 1000 )
/* The systick is a 24-bit counter. */
#define MAX_24_BIT_NUMBER               ( 0xffffffUL )
/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define MISSED_COUNTS_FACTOR            ( 45UL )

/**
 * @brief Constants required to manipulate the NVIC.
 */
#define NVIC_SYSTICK_CTRL                ( ( volatile uint32_t * ) 0xe000e010 )
#define NVIC_SYSTICK_LOAD                ( ( volatile uint32_t * ) 0xe000e014 )
#define NVIC_SYSTICK_CURRENT_VALUE       ( ( volatile uint32_t * ) 0xe000e018 )
#define NVIC_INT_CTRL                    ( ( volatile uint32_t * ) 0xe000ed04 )
#define NVIC_SYSPRI2                     ( ( volatile uint32_t * ) 0xe000ed20 )
#define NVIC_SYSTICK_CLK                 ( 0x00000004 )
#define NVIC_SYSTICK_INT                 ( 0x00000002 )
#define NVIC_SYSTICK_ENABLE              ( 0x00000001 )
#define NVIC_PENDSVSET                   ( 0x10000000 )
#define MIN_INTERRUPT_PRIORITY           ( 255UL )
#define NVIC_PENDSV_PRI                  ( MIN_INTERRUPT_PRIORITY << 16UL )
#define NVIC_SYSTICK_PRI                 ( MIN_INTERRUPT_PRIORITY << 24UL )
#define NVIC_SYSTICK_COUNT_FLAG_BIT      ( 1UL << 16UL )
#define NVIC_PENDSVCLEAR_BIT             ( 1UL << 27UL )
#define NVIC_PEND_SYSTICK_CLEAR_BIT      ( 1UL << 25UL )

#define DEEP_SLEEP_MIN_TIME_MS           1000
#define DEEP_SLEEP_MAX_TIME_MS           10000

uint32_t TimerCountsForOneTick;
uint32_t MaximumSuppressedTicks;
uint32_t StoppedTimerCompensation;

int sleep_flag = 0;
uint16_t sleep_time;
rt_tick_t timeout_tick;
rt_uint32_t ReloadValue;

struct time_rtc time_before_sleep;

static int rtthread_ready_to_sleep(void)
{
    return sys_wakelock_status_get() == 0;
}

static void sleep(struct rt_pm *pm, uint8_t mode)
{
    uint32_t systick_cur_val;

    if (!sleep_flag)
        return;

    if (mode != PM_SLEEP_MODE_DEEP)
        return;

    systick_cur_val = *NVIC_SYSTICK_CURRENT_VALUE;
    deep_sleep_enter(sleep_time);
    *NVIC_SYSTICK_CURRENT_VALUE = systick_cur_val;
}

static void run(struct rt_pm *pm, uint8_t mode)
{
    return;
}

static void pm_timer_start(struct rt_pm *pm, rt_uint32_t timeout)
{
    /* Make sure the SysTick reload value does not overflow the counter. */
    if( timeout > MaximumSuppressedTicks )
    {
        timeout = MaximumSuppressedTicks;
    }

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
    is accounted for as best it can be, but using the tickless mode will
    inevitably result in some tiny drift of the time maintained by the
    kernel with respect to calendar time. */
    *NVIC_SYSTICK_CTRL &= ~NVIC_SYSTICK_ENABLE;

    /* Calculate the reload value required to wait timeout_tick
    tick periods.  -1 is used because this code will execute part way
    through one of the tick periods. */
    ReloadValue = *NVIC_SYSTICK_CURRENT_VALUE + ( TimerCountsForOneTick * ( timeout - 1UL ) );
    if( ReloadValue > StoppedTimerCompensation )
    {
        ReloadValue -= StoppedTimerCompensation;
    }

    if(!rtthread_ready_to_sleep()) {
        /* Restart from whatever is left in the count register to complete this tick period. */
        *NVIC_SYSTICK_LOAD = *NVIC_SYSTICK_CURRENT_VALUE;

        /* Restart SysTick. */
        *NVIC_SYSTICK_CTRL |= NVIC_SYSTICK_ENABLE;

        /* Reset the reload register to the value required for normal tick periods. */
        *NVIC_SYSTICK_LOAD = TimerCountsForOneTick - 1UL;
    } else {
        sleep_flag = 1;
        /* Set the new reload value. */
        *NVIC_SYSTICK_LOAD = ReloadValue;

        /* Clear the SysTick count flag and set the count value back to zero. */
        *NVIC_SYSTICK_CURRENT_VALUE = 0UL;

        /* Restart SysTick. */
        *NVIC_SYSTICK_CTRL |= NVIC_SYSTICK_ENABLE;

        timeout_tick = timeout;

        if (pm->sleep_mode == PM_SLEEP_MODE_DEEP) {
            if (timeout < MaximumSuppressedTicks) {
                sleep_time = DEEP_SLEEP_MIN_TIME_MS;
            } else {
                sleep_time = DEEP_SLEEP_MAX_TIME_MS;
            }

            rtc_32k_time_get(&time_before_sleep, 0);
            // DEBUGPRINT("time_before_sleep sec %d msec %d\r\n\n", time_before_sleep.tv_sec, time_before_sleep.tv_msec);
        }
    }
}

static void pm_timer_stop(struct rt_pm *pm)
{
    return;
}

static rt_tick_t pm_timer_get_tick(struct rt_pm *pm)
{
    rt_tick_t passed_time, CompleteTickPeriods, CompletedSysTickDecrements;
    struct time_rtc time_after_sleep;

    if (!sleep_flag)
        return 0;
    sleep_flag = 0;

    if (pm->sleep_mode == PM_SLEEP_MODE_DEEP) {
        rtc_32k_time_get(&time_after_sleep, 1);

        /* CPU wake up by interrupt and run in irc16M clock, so we need enable interrupt to change clock
        by func system_clock_config_nspe() in NSPE\WIFI_IOT\bsp\bsp_wlan.c */
        __asm volatile ("cpsie i" : : : "memory");
        __asm volatile ("dsb" ::: "memory");
        __asm volatile ("isb");

        // DEBUGPRINT("time_after_sleep sec %d msec %d\r\n", time_after_sleep.tv_sec, time_after_sleep.tv_msec);
        if (time_after_sleep.tv_sec >= time_before_sleep.tv_sec) {
            passed_time = (time_after_sleep.tv_sec * 1000 + time_after_sleep.tv_msec) -
                    (time_before_sleep.tv_sec * 1000 + time_before_sleep.tv_msec);
        } else {
            passed_time = ((time_after_sleep.tv_sec + 60) * 1000 + time_after_sleep.tv_msec) -
                    (time_before_sleep.tv_sec * 1000 + time_before_sleep.tv_msec);
        }
        // DEBUGPRINT("wakeup: sleep time = %d passed_time = %d\r\n", sleep_time, passed_time);
        rt_tick_set(rt_tick_get() + passed_time);
        rt_timer_check();
    }

    if (pm->sleep_mode == PM_SLEEP_MODE_IDLE)
    {
        __asm volatile ("dsb" ::: "memory");
        __asm volatile ("wfi");
        __asm volatile ("isb");
    }

    if (pm->sleep_mode == PM_SLEEP_MODE_DEEP) {
        timeout_tick = 1 + ((*NVIC_SYSTICK_CURRENT_VALUE) / (CPU_CLOCK_HZ / OS_TICK_RATE_HZ));
    }

    /* Disable interrupts again because the clock is about to be stopped
    and interrupts that execute while the clock is stopped will increase
    any slippage between the time maintained by the RTOS and calendar
    time. */
    __asm volatile ("cpsid i" : : : "memory");
    __asm volatile ("dsb" ::: "memory");
    __asm volatile ("isb");

    *NVIC_SYSTICK_CTRL = ( RT_NVIC_SYSTICK_CLK_BIT | NVIC_SYSTICK_INT );

    if( ( *NVIC_SYSTICK_CTRL & NVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
    {
        uint32_t CalculatedLoadValue;

        CalculatedLoadValue = ( TimerCountsForOneTick - 1UL ) - ( ReloadValue - (*NVIC_SYSTICK_CURRENT_VALUE) );
        if( ( CalculatedLoadValue < StoppedTimerCompensation ) || ( CalculatedLoadValue > TimerCountsForOneTick ) )
        {
            CalculatedLoadValue = ( TimerCountsForOneTick - 1UL );
        }

        *NVIC_SYSTICK_LOAD = CalculatedLoadValue;

        CompleteTickPeriods = timeout_tick - 1UL;
    } else {
        CompletedSysTickDecrements = ( timeout_tick * TimerCountsForOneTick ) - (*NVIC_SYSTICK_CURRENT_VALUE);

        CompleteTickPeriods = CompletedSysTickDecrements / TimerCountsForOneTick;

        *NVIC_SYSTICK_LOAD = ( ( CompleteTickPeriods + 1UL ) * TimerCountsForOneTick ) - CompletedSysTickDecrements;
    }

    *NVIC_SYSTICK_CURRENT_VALUE = 0UL;
    *NVIC_SYSTICK_CTRL |= NVIC_SYSTICK_ENABLE;
    *NVIC_SYSTICK_LOAD = TimerCountsForOneTick - 1UL;

    return CompleteTickPeriods;
}

void rt_system_sleep_init(void)
{
    static const struct rt_pm_ops pm_ops =
    {
        sleep,
        run,
        pm_timer_start,
        pm_timer_stop,
        pm_timer_get_tick
    };
    rt_uint8_t timer_mask = 0;

    timer_mask = (1UL << PM_SLEEP_MODE_DEEP) | (1UL << PM_SLEEP_MODE_IDLE);

    rt_system_pm_init(&pm_ops, timer_mask, NULL);

    TimerCountsForOneTick = ( SYSTICK_CLOCK_HZ / TICK_RATE_HZ );
    MaximumSuppressedTicks = MAX_24_BIT_NUMBER / TimerCountsForOneTick;
    StoppedTimerCompensation = MISSED_COUNTS_FACTOR / ( CPU_CLOCK_HZ / SYSTICK_CLOCK_HZ );

    /* Stop and reset the SysTick. */
    *( NVIC_SYSTICK_CTRL ) = 0UL;
    *( NVIC_SYSTICK_CURRENT_VALUE ) = 0UL;

    /* Configure SysTick to interrupt at the requested rate. */
    *( NVIC_SYSTICK_LOAD ) = ( CPU_CLOCK_HZ / TICK_RATE_HZ ) - 1UL;
    *( NVIC_SYSTICK_CTRL ) = NVIC_SYSTICK_CLK | NVIC_SYSTICK_INT | NVIC_SYSTICK_ENABLE;
}
#endif