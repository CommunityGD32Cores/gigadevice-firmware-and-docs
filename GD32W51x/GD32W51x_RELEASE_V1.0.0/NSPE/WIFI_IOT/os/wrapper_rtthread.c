/*!
    \file    wrapper_rtthread.c
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

#include <board.h>
// #include <shell.h>
#include <debug_print.h>
#include <delay.h>
#include "pm_sleep.h"

/***************** misc re-implementation *****************/
void rt_hw_console_output(const char *str)
{
    co_printf(str);
}

/***************** heap management implementation *****************/
#ifndef RT_USING_MEMHEAP_AS_HEAP
/* There is a file name conflict in MDK project, thus we import it in code instead of in project */
#if defined (RT_USING_HEAP) && defined (RT_USING_SMALL_MEM)
#include <../os/rt-thread/src/mem.c>
#endif
#else
/* These macros are not exposed by memheap.c, we copy them here */
#define RT_MEMHEAP_MAGIC        0x1ea01ea0
#define RT_MEMHEAP_MINIALLOC    12

#define RT_MEMHEAP_SIZE         RT_ALIGN(sizeof(struct rt_memheap_item), RT_ALIGN_SIZE)
#define MEMITEM_SIZE(item)      ((rt_uint32_t)item->next - (rt_uint32_t)item - RT_MEMHEAP_SIZE)
#endif

void *sys_malloc(size_t size)
{
    return rt_malloc(size);
}

void *sys_calloc(size_t count, size_t size)
{
    return rt_calloc(count, size);
}

void *sys_realloc(void *mem, size_t size)
{
    return rt_realloc(mem, size);
}

void sys_mfree(void *ptr)
{
    rt_free(ptr);
}

int32_t sys_free_heap_size(void)
{
#ifndef RT_USING_MEMHEAP_AS_HEAP
    rt_uint32_t total;
    rt_uint32_t used;

    rt_memory_info(&total, &used, NULL);
    return total - used;
#else
    rt_uint32_t available_size = 0;
    struct rt_object *object;
    struct rt_list_node *node;
    struct rt_memheap *heap;
    struct rt_object_information *information;

    information = rt_object_get_information(RT_Object_Class_MemHeap);
    RT_ASSERT(information != RT_NULL);
    for (node  = information->object_list.next;
         node != &(information->object_list);
         node  = node->next) {
        object = rt_list_entry(node, struct rt_object, list);
        heap   = (struct rt_memheap *)object;

        available_size += heap->available_size;
    }

    return available_size;
#endif
}

int32_t sys_min_free_heap_size(void)
{
    rt_uint32_t total = 0;
    rt_uint32_t max_used = 0;

#ifndef RT_USING_MEMHEAP_AS_HEAP
    rt_memory_info(&total, NULL, &max_used);
#else
    struct rt_object *object;
    struct rt_list_node *node;
    struct rt_memheap *heap;
    struct rt_object_information *information;

    information = rt_object_get_information(RT_Object_Class_MemHeap);
    RT_ASSERT(information != RT_NULL);
    for (node  = information->object_list.next;
         node != &(information->object_list);
         node  = node->next) {
        object = rt_list_entry(node, struct rt_object, list);
        heap   = (struct rt_memheap *)object;

        total += heap->pool_size;
        max_used += heap->max_used_size;
    }
#endif
    return total - max_used;
}

static void heap_dump(void)
{
#ifndef RT_USING_MEMHEAP_AS_HEAP
    int position;
    struct heap_mem *mem;
    int size;

    for (mem = (struct heap_mem *)heap_ptr; mem != heap_end; mem = (struct heap_mem *)&heap_ptr[mem->next]) {
        if (mem->used)
            continue;

        position = (rt_uint32_t)mem - (rt_uint32_t)heap_ptr;

        rt_kprintf("[0x%08x - ", mem);

        size = mem->next - position - SIZEOF_STRUCT_MEM;
        rt_kprintf("%d", size);

        if (mem->magic != HEAP_MAGIC)
            rt_kprintf("]: ***\r\n");
        else
            rt_kprintf("]\r\n");
    }
#else
    struct rt_object *object;
    struct rt_list_node *node;
    struct rt_memheap *heap;
    struct rt_object_information *information;
    rt_uint32_t free_size;
    struct rt_memheap_item *header_ptr;

    information = rt_object_get_information(RT_Object_Class_MemHeap);
    RT_ASSERT(information != RT_NULL);
    for (node  = information->object_list.next;
         node != &(information->object_list);
         node  = node->next) {
        object = rt_list_entry(node, struct rt_object, list);
        heap   = (struct rt_memheap *)object;

        header_ptr = heap->free_list->next_free;
        while (header_ptr != heap->free_list) {
            rt_kprintf("[0x%08x - ", header_ptr);

            /* get current freed memory block size */
            free_size = MEMITEM_SIZE(header_ptr);
            rt_kprintf("%d", free_size);

            if (header_ptr->magic != RT_MEMHEAP_MAGIC)
                rt_kprintf("]: ***\r\n");
            else
                rt_kprintf("]\r\n");
            header_ptr = header_ptr->next_free;
        }
    }
#endif
}

uint16_t sys_heap_block_size(void)
{
#ifndef RT_USING_MEMHEAP_AS_HEAP
    return SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED;
#else
    return RT_MEMHEAP_SIZE + RT_MEMHEAP_MINIALLOC;
#endif
}

void dump_mem_block_list(void)
{
    heap_dump();
}

/***************** memory manipulation *****************/
void sys_memcpy(void *des, const void *src, uint32_t n)
{
    rt_memcpy(des, src, n);
}

void sys_memmove(void *des, const void *src, uint32_t n)
{
    rt_memmove(des, src, n);
}

void sys_memset(void *s, uint8_t c, uint32_t count)
{
    rt_memset(s, c, count);
}

int32_t sys_memcmp(const void *buf1, const void *buf2, uint32_t count)
{
    return rt_memcmp(buf1, buf2, count);
}

/***************** OS API wrappers *****************/

void *sys_task_create(void *task, const uint8_t *name, uint32_t *stack_base, uint32_t stack_size,
                    uint32_t queue_size, uint32_t priority, task_func_t func, void *ctx)
{
    rt_err_t ret;
    task_wrapper_t *task_wrapper = NULL;
    rt_thread_t task_handle;

    if (queue_size) {
        task_wrapper = (task_wrapper_t *)sys_malloc(sizeof(task_wrapper_t));
        if (task_wrapper == NULL) {
            DEBUGPRINT("sys_task_create: malloc wrapper failed\r\n");
            return NULL;
        }

        task_wrapper->task_queue = rt_mq_create("", sizeof(void *), queue_size, RT_IPC_FLAG_FIFO);
        if (task_wrapper->task_queue == NULL) {
            DEBUGPRINT("sys_task_create: create task queue failed\r\n");
            sys_mfree(task_wrapper);
            return NULL;
        }
    }

    /* protect task creation and task wrapper pointer storing against inconsistency of them if preempted in the middle */
    rt_enter_critical();
    if (task != NULL && stack_base != NULL) {
        ret = rt_thread_init(task, (const char *)name, func, ctx, stack_base, stack_size * sizeof(uint32_t), priority, 10);
        if (ret != RT_EOK) {
            DEBUGPRINT("sys_task_create init task failed\r\n");
            rt_exit_critical();
            goto exit;
        }
        task_handle = task;
    } else {
        task_handle = rt_thread_create((const char *)name, func, ctx, stack_size * sizeof(uint32_t), priority, 10);
        if (task_handle == NULL) {
            DEBUGPRINT("sys_task_create creat task failed\r\n");
            rt_exit_critical();
            goto exit;
        }
    }
    rt_thread_startup(task_handle);

    if (queue_size > 0) {
        task_wrapper->task_handle = task_handle;
        task_wrapper->max_msg_num = queue_size;
        task_wrapper->cur_msg_num = 0;
    }
    task_handle->user_data = (uint32_t)task_wrapper;
    rt_exit_critical();

    return task_handle;

exit:
    if (queue_size > 0) {
        rt_mq_delete(task_wrapper->task_queue);
        sys_mfree(task_wrapper);
    }

    return NULL;
}

void sys_task_delete(void *task)
{
    os_task_t *task_handle = (os_task_t *)task;
    task_wrapper_t *task_wrapper;

    if (task == NULL) {
        task_handle = rt_thread_self();
    }
    task_wrapper = (task_wrapper_t *)task_handle->user_data;

    /* if task is deleted by another task, delete task first, then free task_wrapper. Otherwise, when
        task_wrapper is freed but task is still alive, task_wrapper pointer got by task is invalid.
    */
    if (task && task != rt_thread_self()) {
        if (rt_object_is_systemobject((rt_object_t)task))
            rt_thread_detach(task);
        else
            rt_thread_delete(task);
    }

    if (task_wrapper != NULL) {
        rt_mq_delete(task_wrapper->task_queue);
        sys_mfree(task_wrapper);
    }

    /* rt-thread doesn't support/need deleting task of itself, just let it exit from task function */
}

int32_t sys_task_wait(uint32_t timeout_ms, void *msg_ptr)
{
    os_task_t *task_handle;
    task_wrapper_t *task_wrapper;
    int32_t result;
    SYS_SR_ALLOC();

    task_handle = rt_thread_self();

    task_wrapper = (task_wrapper_t *)task_handle->user_data;
    if (task_wrapper == NULL) {
        DEBUGPRINT("sys_task_wait: can't find task wrapper\r\n");
        return OS_ERROR;
    }

    result = sys_queue_fetch(&task_wrapper->task_queue, msg_ptr, timeout_ms, 1);
    if (result == OS_OK) {
        SYS_CRITICAL_ENTER();
        task_wrapper->cur_msg_num--;
        SYS_CRITICAL_EXIT();
    }

    return result;
}

int32_t sys_task_post(void *receiver_task, void *msg_ptr, uint8_t from_isr)
{
    os_task_t *task_handle = (os_task_t *)receiver_task;
    task_wrapper_t *task_wrapper;
    int32_t ret;
    SYS_SR_ALLOC();

    task_wrapper = (task_wrapper_t *)task_handle->user_data;
    if (task_wrapper == NULL) {
        DEBUGPRINT("sys_task_post: can't find task wrapper\r\n");
        return OS_ERROR;
    }

    ret = sys_queue_post(&task_wrapper->task_queue, msg_ptr);
    if (ret == OS_OK) {
        SYS_CRITICAL_ENTER();
        task_wrapper->cur_msg_num++;
        SYS_CRITICAL_EXIT();
    }

    return ret;
}

void sys_task_msg_flush(void *task)
{
    os_task_t *task_handle = (os_task_t *)task;
    task_wrapper_t *task_wrapper;
    SYS_SR_ALLOC();

    if (task == NULL) {
        task_handle = rt_thread_self();
    }

    task_wrapper = (task_wrapper_t *)task_handle->user_data;
    if (task_wrapper != NULL) {
        rt_mq_control(task_wrapper->task_queue, RT_IPC_CMD_RESET, NULL);
        SYS_CRITICAL_ENTER();
        task_wrapper->cur_msg_num = 0;
        SYS_CRITICAL_EXIT();
    }
}

int32_t sys_task_msg_num(void *task, uint8_t from_isr)
{
    os_task_t *task_handle = (os_task_t *)task;
    task_wrapper_t *task_wrapper;

    if (task == NULL) {
        task_handle = rt_thread_self();
    }

    task_wrapper = (task_wrapper_t *)task_handle->user_data;
    if (task_wrapper == NULL) {
        DEBUGPRINT("sys_task_msg_num: can't find task wrapper\r\n");
        return OS_ERROR;
    }

    return task_wrapper->cur_msg_num;
}

os_task_t *sys_idle_task_handle_get(void)
{
    return rt_thread_idle_gethandler();
}

os_task_t *sys_timer_task_handle_get(void)
{
    /* It's not exposed by rt-thread */
    return NULL;
}

uint32_t sys_stack_free_get(void *task)
{
    os_task_t *task_handle = task;
    rt_uint8_t *ptr;

    if (task == NULL)
        return 0;

    ptr = (rt_uint8_t *)task_handle->stack_addr;
    while (*ptr == '#')
        ptr++;
    return ((rt_uint32_t) ptr - (rt_uint32_t) task_handle->stack_addr)/sizeof(rt_uint32_t);
}

static int object_name_maxlen(struct rt_list_node *list)
{
    struct rt_list_node *node;
    struct rt_object *object = NULL;
    int max_length = 0, length;

    rt_enter_critical();
    for (node = list->next; node != list; node = node->next) {
        object = rt_list_entry(node, struct rt_object, list);

        length = rt_strlen(object->name);
        if (length > max_length) max_length = length;
    }
    rt_exit_critical();

    if (max_length > RT_NAME_MAX || max_length == 0) max_length = RT_NAME_MAX;

    return max_length;
}

rt_inline void object_split(int len)
{
    while (len--) rt_kprintf("-");
}

static long _list_thread(struct rt_list_node *list)
{
    int maxlen;
    rt_uint8_t *ptr;
    struct rt_thread *thread;
    struct rt_list_node *node;

    maxlen = object_name_maxlen(list);

    rt_kprintf("%-*.s pri  status      sp     stack size max used left tick  error\r\n", maxlen, "thread"); object_split(maxlen);
    rt_kprintf(" ---  ------- ---------- ----------  ------  ---------- ---\r\n");
    for (node = list->next; node != list; node = node->next) {
        rt_uint8_t stat;
        thread = rt_list_entry(node, struct rt_thread, list);
        rt_kprintf("%-*.*s %3d ", maxlen, RT_NAME_MAX, thread->name, thread->current_priority);

        stat = (thread->stat & RT_THREAD_STAT_MASK);
        if (stat == RT_THREAD_READY)        rt_kprintf(" ready  ");
        else if (stat == RT_THREAD_SUSPEND) rt_kprintf(" suspend");
        else if (stat == RT_THREAD_INIT)    rt_kprintf(" init   ");
        else if (stat == RT_THREAD_CLOSE)   rt_kprintf(" close  ");

        ptr = (rt_uint8_t *)thread->stack_addr;
        while (*ptr == '#') ptr++;

        rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\r\n",
                   thread->stack_size + ((rt_uint32_t)thread->stack_addr - (rt_uint32_t)thread->sp),
                   thread->stack_size,
                   (thread->stack_size - ((rt_uint32_t) ptr - (rt_uint32_t) thread->stack_addr)) * 100
                        / thread->stack_size,
                   thread->remaining_tick,
                   thread->error);
    }

    return OS_OK;
}

void sys_task_list(char *pwrite_buf)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Thread);
    _list_thread(&info->object_list);
}

int32_t sys_sema_init(os_sema_t *sema, int32_t init_val)
{
    *sema = rt_sem_create("", init_val, RT_IPC_FLAG_FIFO);
    if (*sema == NULL) {
        DEBUGPRINT("sys_sema_init, sema = NULL\r\n");
        return OS_ERROR;
    }
    return OS_OK;
}

void sys_sema_free(os_sema_t *sema)
{
    if (*sema == NULL) {
        DEBUGPRINT("sys_sema_free, sema = NULL\r\n");
        return;
    }

    rt_sem_delete(*sema);
    *sema = NULL;
}

void sys_sema_up(os_sema_t *sema)
{
    rt_sem_release(*sema);
}

void sys_sema_up_from_isr(os_sema_t *sema)
{
    rt_sem_release(*sema);
}

/*void sys_post_sema(os_sema_t *sema)
{
    xSemaphoreGive(*sema);
}
*/

int32_t sys_sema_down(os_sema_t *sema, uint32_t timeout_ms)
{
    uint32_t timeout_tick;
    rt_err_t result;

    if (timeout_ms == 0) {
        timeout_tick = (uint32_t)RT_WAITING_FOREVER;
    } else {
        timeout_tick = timeout_ms / OS_MS_PER_TICK;
        if (timeout_tick == 0) {
            timeout_tick = 1;
        }
    }

    result = rt_sem_take(*sema, timeout_tick);
    if (result == RT_EOK)
        return OS_OK;

    if (result == -RT_ETIMEOUT) {
        //DEBUGPRINT("sys_sema_down, timeout\r\n");
    } else {
        DEBUGPRINT("sys_sema_down, error\r\n");
    }

    return ERROR;
}

void sys_mutex_init(os_mutex_t *mutex)
{
    *mutex = rt_mutex_create("", RT_IPC_FLAG_FIFO);
    DEBUG_ASSERT(*mutex != NULL);
}

void sys_mutex_free(os_mutex_t *mutex)
{
    if (*mutex == NULL) {
        DEBUGPRINT("sys_mutex_free, mutex = NULL\r\n");
        return;
    }

    rt_mutex_delete(*mutex);
    *mutex = NULL;
}

void sys_mutex_get(os_mutex_t *mutex)
{
    while (rt_mutex_take(*mutex, RT_WAITING_FOREVER) != RT_EOK) {
        DEBUGPRINT("[%s] get mutex 0x%08x failed, retry\r\n", rt_thread_self()->name, *mutex);
    }
}

void sys_mutex_put(os_mutex_t *mutex)
{
    rt_mutex_release(*mutex);
}

int32_t sys_queue_init(os_queue_t *queue, int32_t queue_size, uint32_t item_size)
{
    if ((*queue = rt_mq_create("", item_size, queue_size, RT_IPC_FLAG_FIFO)) == NULL) {
        DEBUGPRINT("sys_queue_init failed\r\n");
        return OS_ERROR;
    }

    return OS_OK;
}

void sys_queue_free(os_queue_t *queue)
{
    rt_mq_delete(*queue);

    *queue = NULL;
}

int32_t sys_queue_post(os_queue_t *queue, void *msg)
{
    if (rt_mq_send(*queue, msg, (*queue)->msg_size) != RT_EOK) {
        DEBUGPRINT("sys_queue_post failed\r\n");
        return OS_ERROR;
    }

    return OS_OK;
}

int32_t sys_queue_fetch(os_queue_t *queue, void *msg, uint32_t timeout_ms, uint8_t is_blocking)
{
    uint32_t timeout_tick;

    if (!is_blocking) {
        timeout_tick = 0;
    } else if (is_blocking && timeout_ms == 0) {
        timeout_tick = (uint32_t)RT_WAITING_FOREVER;
    } else {
        timeout_tick = timeout_ms / OS_MS_PER_TICK;
        if (timeout_tick == 0) {
            timeout_tick = 1;
        }
    }

    if (rt_mq_recv(*queue, msg, (*queue)->msg_size, timeout_tick) != RT_EOK) {
        return OS_TIMEOUT;
    }

    return OS_OK;
}

uint32_t sys_current_time_get(void)
{
    return (uint32_t)(rt_tick_get() * OS_MS_PER_TICK);
}

uint32_t sys_time_get(void *p)
{
    return sys_current_time_get();
}

void sys_ms_sleep(uint32_t ms)
{
    uint32_t tick = ms / OS_MS_PER_TICK;

    if (tick == 0) {
        tick = 1;
    }

    rt_thread_delay(tick);
}

void sys_us_delay(uint32_t nus)
{
    rt_enter_critical();
    //rt_hw_us_delay(nus);
    systick_udelay(nus);
    rt_exit_critical();
}

void sys_yield(void)
{
    rt_thread_yield();
}

void sys_sched_lock(void)
{
    rt_enter_critical();
}

void sys_sched_unlock(void)
{
    rt_exit_critical();
}

static int32_t _arc4_random(void)
{
    uint32_t res = sys_current_time_get();
    uint32_t seed = seed_rand32();

    seed = ((seed & 0x007F00FF) << 7) ^
        ((seed & 0x0F80FF00) >> 8) ^
        (res << 13) ^ (res >> 9);

    return (int32_t)seed;
}

int32_t sys_random_bytes_get(void *dst, uint32_t size)
{
    uint32_t ranbuf;
    uint32_t *lp;
    int32_t i, count;
    count = size / sizeof(uint32_t);
    lp = (uint32_t *)dst;

    for (i = 0; i < count; i++) {
        lp[i] = _arc4_random();
        size -= sizeof(uint32_t);
    }

    if(size > 0) {
        ranbuf = _arc4_random();
        sys_memcpy(&lp[i], &ranbuf, size);
    }

    return OS_OK;
}

static void _sys_timer_callback(void *p_tmr)
{
    timer_wrapper_t *timer_wrapper;

    timer_wrapper = rt_container_of(*(os_timer_t *)p_tmr, timer_wrapper_t, os_timer);
    timer_wrapper->timer_func(p_tmr, timer_wrapper->p_arg);
}

void sys_timer_init(os_timer_t *timer, const uint8_t *name, uint32_t delay, uint8_t periodic, timer_func_t func, void *arg)
{
    timer_wrapper_t *timer_wrapper;

    timer_wrapper = (timer_wrapper_t *)sys_malloc(sizeof(timer_wrapper_t));
    if (timer_wrapper == NULL) {
        DEBUGPRINT("sys_timer_init, malloc wrapper failed\r\n");
        return;
    }

    timer_wrapper->p_arg = arg;
    timer_wrapper->timer_func = func;

    rt_timer_init(&timer_wrapper->os_timer, (const char *)name, _sys_timer_callback, timer, delay / OS_MS_PER_TICK,
                  RT_TIMER_FLAG_SOFT_TIMER | (periodic ? RT_TIMER_FLAG_PERIODIC : RT_TIMER_FLAG_ONE_SHOT));

    *timer = &timer_wrapper->os_timer;
}

void sys_timer_delete(os_timer_t *timer)
{
    timer_wrapper_t *timer_wrapper;
    os_timer_t p_timer;

    if (*timer == NULL) {
        DEBUGPRINT("sys_timer_delete: timer is NULL\r\n");
        return;
    }

    p_timer = *timer;
    *timer = NULL;

    if (rt_timer_detach(p_timer) != RT_EOK) {
        DEBUGPRINT("sys_timer_delete failed\r\n");
        return;
    }

       timer_wrapper = rt_container_of(p_timer, timer_wrapper_t, os_timer);
    sys_mfree(timer_wrapper);
}

void sys_timer_start(os_timer_t *timer, uint8_t from_isr)
{
    rt_err_t result;

    result = rt_timer_start(*timer);
    if (result != RT_EOK) {
        DEBUGPRINT("sys_timer_start failed\r\n");
    }
}


void sys_timer_start_ext(os_timer_t *timer, uint32_t delay, uint8_t from_isr)
{
    rt_tick_t timer_ticks;
    rt_err_t result = 0;
    register rt_base_t level;

    if (delay <= OS_MS_PER_TICK) {
        timer_ticks = 1;
    } else {
        timer_ticks = delay / OS_MS_PER_TICK;
    }

    level = rt_hw_interrupt_disable();

    rt_timer_control(*timer, RT_TIMER_CTRL_SET_TIME, &timer_ticks);

    rt_hw_interrupt_enable(level);

    result = rt_timer_start(*timer);
    if (result != RT_EOK) {
        DEBUGPRINT("sys_timer_start_ext start timer failed\r\n");
    }
}

uint8_t sys_timer_stop(os_timer_t *timer, uint8_t from_isr)
{
    rt_err_t result;

    result = rt_timer_stop(*timer);

    if (result != RT_EOK) {
        DEBUGPRINT("sys_timer_stop failed\r\n");
        return 0;
    }

    return 1;
}

uint8_t sys_timer_pending(os_timer_t *timer)
{
    return !!((*timer)->parent.flag & RT_TIMER_FLAG_ACTIVATED);
}

void sys_os_misc_init(void)
{

}

/* 16KB is reserved for LwIP memory pool, tail 8KB is reserved for PS packet buffer */
#define TOTAL_HEAP_SIZE     ( ( size_t ) ( 80 * 1024) )
static uint8_t HeapRegion0[TOTAL_HEAP_SIZE];
#ifdef EXTEND_MEMORY_ADD
#define TOTAL_HEAP1_SIZE    ( ( size_t ) ( 64 * 1024) )
static uint8_t HeapRegion1[TOTAL_HEAP1_SIZE];
static struct rt_memheap _heap;
#endif

extern void systick_config(void);

void sys_os_init(void)
{
    //rt_hw_interrupt_init();
    //rt_hw_board_init();
    //rt_hw_driver_init();
    systick_config();
    rt_system_heap_init(HeapRegion0, HeapRegion0 + TOTAL_HEAP_SIZE);
#ifdef EXTEND_MEMORY_ADD
    rt_memheap_init(&_heap, "heap1", HeapRegion1, TOTAL_HEAP_SIZE);
#endif
    rt_system_scheduler_init();
    rt_system_timer_init();
    rt_system_timer_thread_init();
#ifdef RT_USING_FINSH
    finsh_system_init();
    finsh_set_device(CONSOLE_DEVICE);
#endif
    //rt_application_init();
#ifdef RT_USING_PM
    rt_system_sleep_init();
#endif
    rt_thread_idle_init();
    rt_show_version();
}

void sys_os_start(void)
{
    rt_system_scheduler_start();
}
