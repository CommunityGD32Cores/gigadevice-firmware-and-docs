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

/**@defgroup los_queue Queue
 * @ingroup kernel
 */

#ifndef _LOS_QUEUE_H
#define _LOS_QUEUE_H

#include "los_base.h"
#include "los_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/**
  * @ingroup los_queue
  * Queue error code: The maximum number of queue resources is configured to 0.
  *
  * Value: 0x02000600
  *
  * Solution: Configure the maximum number of queue resources to be greater than 0. If queue modules are not used, set the configuration item for the tailoring of the maximum number of queue resources to NO.
  */
#define LOS_ERRNO_QUEUE_MAXNUM_ZERO                     LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x00)

/**
  * @ingroup los_queue
  * Queue error code: The queue block memory fails to be initialized.
  *
  * Value: 0x02000601
  *
  * Solution: Allocate the queue block bigger memory partition, or decrease the maximum number of queue resources.
  */
#define LOS_ERRNO_QUEUE_NO_MEMORY                       LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x01)

/**
  * @ingroup los_queue
  * Queue error code: The memory for queue creation fails to be requested.
  *
  * Value: 0x02000602
  *
  * Solution: Allocate more memory for queue creation, or decrease the queue length and the number of nodes in the queue to be created.
  */
#define LOS_ERRNO_QUEUE_CREATE_NO_MEMORY                LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x02)

/**
  * @ingroup los_queue
  * Queue error code: The size of the biggest message in the created queue is too big.
  *
  * Value: 0x02000603
  *
  * Solution: Change the size of the biggest message in the created queue.
  */
#define LOS_ERRNO_QUEUE_SIZE_TOO_BIG                    LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x03)

/**
  * @ingroup los_queue
  * Queue error code: The upper limit of the number of created queues is exceeded.
  *
  * Value: 0x02000604
  *
  * Solution: Increase the configured number of resources for queues.
  */
#define LOS_ERRNO_QUEUE_CB_UNAVAILABLE                  LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x04)

/**
  * @ingroup los_queue
  * Queue error code: Invalid queue.
  *
  * Value: 0x02000605
  *
  * Solution: Ensure that the passed-in queue ID is valid.
  */
#define LOS_ERRNO_QUEUE_NOT_FOUND                       LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x05)

/**
  * @ingroup los_queue
  * Queue error code: The task is forbidden to be blocked on a queue when the task is locked.
  *
  * Value: 0x02000606
  *
  * Solution: Unlock the task before using a queue.
  */
#define LOS_ERRNO_QUEUE_PEND_IN_LOCK                    LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x06)

/**
  * @ingroup los_queue
  * Queue error code: The time set for waiting to processing the queue expires.
  *
  * Value: 0x02000607
  *
  * Solution: Check whether the expiry time setting is appropriate.
  */
#define LOS_ERRNO_QUEUE_TIMEOUT                         LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x07)

/**
  * @ingroup los_queue
  * Queue error code: The queue that blocks a task cannot be deleted.
  *
  * Value: 0x02000608
  *
  * Solution: Enable the task to obtain resources rather than be blocked on the queue.
  */
#define LOS_ERRNO_QUEUE_IN_TSKUSE                       LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x08)

/**
  * @ingroup los_queue
  * Queue error code: The queue cannot be written during an interrupt when the time for waiting to processing the queue expires.
  *
  * Value: 0x02000609
  *
  * Solution: Set the expiry time to the never-waiting mode, or use asynchronous queues.
  */
#define LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT              LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x09)

/**
  * @ingroup los_queue
  * Queue error code: The queue is not created.
  *
  * Value: 0x0200060a
  *
  * Solution: Check whether the passed-in queue handle value is valid.
  */
#define LOS_ERRNO_QUEUE_NOT_CREATE                      LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0a)

/**
  * @ingroup los_queue
  * Queue error code: Queue reading and writing are not synchronous.
  *
  * Value: 0x0200060b
  *
  * Solution: Synchronize queue reading with queue writing.
  */
#define LOS_ERRNO_QUEUE_IN_TSKWRITE                     LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0b)

/**
  * @ingroup los_queue
  * Queue error code: Parameters passed in during queue creation are null pointers.
  *
  * Value: 0x0200060c
  *
  * Solution: Ensure the passed-in parameters are not null pointers.
  */
#define LOS_ERRNO_QUEUE_CREAT_PTR_NULL                  LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0c)

/**
  * @ingroup los_queue
  * Queue error code: The queue length or message node size passed in during queue creation is 0.
  *
  * Value: 0x0200060d
  *
  * Solution: Pass in correct queue length and message node size.
  */
#define LOS_ERRNO_QUEUE_PARA_ISZERO                     LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0d)

/**
  * @ingroup los_queue
  * Queue error code: The handle of the queue that is being read is invalid.
  *
  * Value: 0x0200060e
  *
  * Solution: Check whether the passed-in queue handle value is valid.
  */
#define LOS_ERRNO_QUEUE_READ_INVALID                    LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0e)

/**
  * @ingroup los_queue
  * Queue error code: The pointer passed in during queue reading is null.
  *
  * Value: 0x0200060f
  *
  * Solution: Check whether the passed-in pointer is null.
  */
#define LOS_ERRNO_QUEUE_READ_PTR_NULL                   LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0f)

/**
  * @ingroup los_queue
  * Queue error code: The buffer size passed in during queue reading is 0.
  *
  * Value: 0x02000610
  *
  * Solution: Pass in a correct buffer size.
  */
#define LOS_ERRNO_QUEUE_READSIZE_ISZERO                 LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x10)

/**
  * @ingroup los_queue
  * Queue error code: The queue handle passed in during queue writing is invalid.
  *
  * Value: 0x02000611
  *
  * Solution: Check whether the passed-in queue handle value is valid.
  */
#define LOS_ERRNO_QUEUE_WRITE_INVALID                   LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x11)

/**
  * @ingroup los_queue
  * Queue error code: The pointer passed in during queue writing is null.
  *
  * Value: 0x02000612
  *
  * Solution: Check whether the passed-in pointer is null.
  */
#define LOS_ERRNO_QUEUE_WRITE_PTR_NULL                  LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x12)

/**
  * @ingroup los_queue
  * Queue error code: The buffer size passed in during queue writing is 0.
  *
  * Value: 0x02000613
  *
  * Solution: Pass in a correct buffer size.
  */
#define LOS_ERRNO_QUEUE_WRITESIZE_ISZERO                LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x13)

/**
  * @ingroup los_queue
  * Queue error code: The queue into which the data is written is not created..
  *
  * Value: 0x02000614
  *
  * Solution: Pass in correct input parameters.
  */
#define LOS_ERRNO_QUEUE_WRITE_NOT_CREATE                LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x14)

/**
  * @ingroup los_queue
  * Queue error code: The buffer size passed in during queue writing is bigger than the queue size.
  *
  * Value: 0x02000615
  *
  * Solution: Decrease the buffer size, or use a queue in which nodes are bigger.
  */
#define LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG              LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x15)

/**
  * @ingroup los_queue
  * Queue error code: No free node is available during queue writing.
  *
  * Value: 0x02000616
  *
  * Solution: Ensure that free nodes are available before queue writing.
  */
#define LOS_ERRNO_QUEUE_ISFULL                          LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x16)

/**
  * @ingroup los_queue
  * Queue error code: The pointer passed in when the queue information is being obtained is null.
  *
  * Value: 0x02000617
  *
  * Solution: Check whether the passed-in pointer is null.
  */
#define LOS_ERRNO_QUEUE_PTR_NULL                        LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x17)

/**
  * @ingroup los_queue
  * Queue error code: The queue cannot be read during an interrupt when the time for waiting to processing the queue expires.
  *
  * Value: 0x02000618
  *
  * Solution: Set the expiry time to the never-waiting mode, or use asynchronous queues.
  */
#define LOS_ERRNO_QUEUE_READ_IN_INTERRUPT               LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x18)

/**
  * @ingroup los_queue
  * Queue error code: The handle of the queue passed-in when the memory for the queue is being freed is invalid.
  *
  * Value: 0x02000619
  *
  * Solution: Check whether the passed-in queue handle value is valid.
  */
#define LOS_ERRNO_QUEUE_MAIL_HANDLE_INVALID             LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x19)

/**
  * @ingroup los_queue
  * Queue error code: The pointer to the memory to be freed is null.
  *
  * Value: 0x0200061a
  *
  * Solution: Check whether the passed-in pointer is null.
  */
#define LOS_ERRNO_QUEUE_MAIL_PTR_INVALID                LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1a)

/**
  * @ingroup los_queue
  * Queue error code: The memory for the queue fails to be freed.
  *
  * Value: 0x0200061b
  *
  * Solution: Pass in correct input parameters.
  */
#define LOS_ERRNO_QUEUE_MAIL_FREE_ERROR                 LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1b)

/**
  * @ingroup los_queue
  * Queue error code: The queue to be read is not created.
  *
  * Value: 0x0200061c
  *
  * Solution: Pass in correct input parameters.
  */
#define LOS_ERRNO_QUEUE_READ_NOT_CREATE                 LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1c)

/**
  * @ingroup los_queue
  * Queue error code: No resource is in the queue that is being read when the time for waiting to processing the queue expires.
  *
  * Value: 0x0200061d
  *
  * Solution: Ensure that the queue contains messages when it is being read.
  */
#define LOS_ERRNO_QUEUE_ISEMPTY                         LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1d)

/**
  * @ingroup los_queue
  * Structure of the block for queue information query
  */
typedef struct tagQueueInfo
{
    UINT32     uwQueueID;                       /**< Queue ID                   */
    UINT16     usQueueLen;                      /**< Queue length               */
    UINT16     usQueueSize;                     /**< Node size                  */
    UINT16     usQueueHead;                     /**< Node head                  */
    UINT16     usQueueTail;                     /**< Node tail                  */
    UINT16     usWritableCnt;                   /**< Count of writable resources               */
    UINT16     usReadableCnt;                   /**< Count of readable resources               */
    UINT32     uwWaitReadTask;                  /**< Resource reading task                     */
    UINT32     uwWaitWriteTask;                 /**< Resource writing task                     */
    UINT32     uwWaitMemTask;                   /**< Memory task                               */
} QUEUE_INFO_S;

/**
 *@ingroup los_queue
 *@brief Create a message queue.
 *
 *@par Description:
 *This API is used to create a message queue.
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *@param pcQueueName        [IN]    Message queue name, which must not be null.
 *@param usLen              [IN]    Queue length. The value range is [1,0xffff].
 *@param puwQueueID         [OUT]   Queue ID. The value range is [1,LOSCFG_BASE_IPC_QUEUE_LIMIT].
 *@param uwFlags            [IN]    Queue mode. This parameter is not in use temporarily.
 *@param usMaxMsgSize       [IN]    Node size. The value range is [1,0xffff].
 *
 *@retval   #LOS_OK                                         0x00000000: The message queue is successfully created.
 *@retval   #OS_ERRNO_QUEUE_CB_UNAVAILABLE                  0x02000604: The upper limit of the number of created queues is exceeded.
 *@retval   #OS_ERRNO_QUEUE_CREATE_NO_MEMORY                0x02000602: Insufficient memory for queue creation.
 *@retval   #OS_ERRNO_QUEUE_CREAT_PTR_NULL             0x0200060c: Null pointer.
 *@retval   #OS_ERRNO_QUEUE_PARA_ISZERO                0x0200060d: The queue length or message node size passed in during queue creation is 0.
 *@par Dependency:
 *<ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_QueueDelete
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueCreate(CHAR *pcQueueName,
                             UINT16 usLen,
                             UINT32 *puwQueueID,
                             UINT32 uwFlags,
                             UINT16 usMaxMsgSize);


/**
 *@ingroup los_queue
 *@brief Read a queue.
 *
 *@par Description:
 *This API is used to read data in a specified queue, and store the obtained data to the address specified by pBufferAddr. The address and the size of the data to be read are defined by users.
 *@attention
 *<ul>
 *<li>Queue reading adopts the fist in first out (FIFO) mode. The data that is first stored in the queue is read first.</li>
 *<li>pBufferAddr stores the obtained data address.</li>
 *<li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 *<li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 *<li>The argument uwTimeOut is a relative time.</li>
 *</ul>
 *
 *@param uwQueueID        [IN]        Queue ID. The value range is [1,LOSCFG_BASE_IPC_QUEUE_LIMIT].
 *@param pBufferAddr      [OUT]       Starting address that stores the obtained data. The starting address must not be null.
 *@param uwBufferSize     [IN]        Passed-in buffer size, which must not be 0. The value range is [1,0xffffffff].
 *@param uwTimeOut        [IN]        Expiry time. The value range is [0,LOS_WAIT_FOREVER].
 *
 *@retval   #LOS_OK                                     0x00000000: The queue is successfully read.
 *@retval   #OS_ERRNO_QUEUE_READ_INVALID                0x0200060e: The handle of the queue that is being read is invalid.
 *@retval   #OS_ERRNO_QUEUE_READ_PTR_NULL               0x0200060f: The pointer passed in during queue reading is null.
 *@retval   #OS_ERRNO_QUEUE_READSIZE_ISZERO             0x02000610: The buffer size passed in during queue reading is 0.
 *@retval   #OS_ERRNO_QUEUE_READ_IN_INTERRUPT           0x02000618: The queue cannot be read during an interrupt when the time for waiting to processing the queue expires.
 *@retval   #OS_ERRNO_QUEUE_READ_NOT_CREATE             0x0200061c: The queue to be read is not created.
 *@retval   #OS_ERRNO_QUEUE_ISEMPTY                     0x0200061d: No resource is in the queue that is being read when the time for waiting to processing the queue expires.
 *@retval   #OS_ERRNO_QUEUE_PEND_IN_LOCK                0x02000606: The task is forbidden to be blocked on a queue when the task is locked.
 *@retval   #OS_ERRNO_QUEUE_TIMEOUT               0x02000607: The time set for waiting to processing the queue expires.
 *@par Dependency:
 *<ul><li>los_queue.h: The header file that contains the API declaration.</li></ul>
 *@see LOS_QueueWrite
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueRead(UINT32 uwQueueID,
                                VOID *pBufferAddr,
                                UINT32 uwBufferSize,
                                UINT32 uwTimeOut);

/**
 *@ingroup los_queue
 *@brief Write data into a queue.
 *
 *@par Description:
 *This API is used to write the data of the size specified by uwBufferSize and stored at the address specified by pBufferAddr into a queue.
 *@attention
 *<ul>
 *<li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 *<li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 *<li>The address of the data of the size specified by uwBufferSize and stored at the address specified by BufferAddr is to be written.</li>
 *<li>The argument uwTimeOut is a relative time.</li>
 *</ul>
 *
 *@param uwQueueID        [IN]        Queue ID. The value range is [1,LOSCFG_BASE_IPC_QUEUE_LIMIT].
 *@param pBufferAddr      [OUT]       Starting address that stores the data to be written. The starting address must not be null.
 *@param uwBufferSize     [IN]        Passed-in buffer size, which must not be 0. The value range is [1,0xffffffff].
 *@param uwTimeOut        [IN]        Expiry time. The value range is [0,LOS_WAIT_FOREVER].
 *
 *@retval   #LOS_OK                                     0x00000000: The data is successfully written into the queue.
 *@retval   #OS_ERRNO_QUEUE_WRITE_INVALID               0x02000611: The queue handle passed in during queue writing is invalid.
 *@retval   #OS_ERRNO_QUEUE_WRITE_PTR_NULL              0x02000612: The pointer passed in during queue writing is null.
 *@retval   #OS_ERRNO_QUEUE_WRITESIZE_ISZERO            0x02000613: The buffer size passed in during queue writing is 0.
 *@retval   #OS_ERRNO_QUEUE_WRITE_IN_INTERRUPT          0x02000609: The queue cannot be written during an interrupt when the time for waiting to processing the queue expires.
 *@retval   #OS_ERRNO_QUEUE_WRITE_NOT_CREATE            0x02000614: The queue into which the data is written is not created.
 *@retval   #OS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG          0x02000615: The buffer size passed in during queue writing is bigger than the queue size.
 *@retval   #OS_ERRNO_QUEUE_ISFULL                      0x02000616: No free node is available during queue writing.
 *@retval   #OS_ERRNO_QUEUE_PEND_IN_LOCK                0x02000606: The task is forbidden to be blocked on a queue when the task is locked.
 *@retval   #OS_ERRNO_QUEUE_TIMEOUT               0x02000607: The time set for waiting to processing the queue expires.
 *@par Dependency:
 *<ul><li>los_queue.h: The header file that contains the API declaration.</li></ul>
 *@see LOS_QueueRead
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueWrite(UINT32 uwQueueID,
                            VOID *pBufferAddr,
                            UINT32 uwBufferSize,
                            UINT32 uwTimeOut);

 /**
  *@ingroup los_queue
  *@brief Delete a queue.
  *
  *@par Description:
  *This API is used to delete a queue.
  *@attention
  *<ul>
  *<li>This API cannot be used to delete a queue that is not created.</li>
  *<li>A synchronous queue fails to be deleted if any tasks are blocked on it, or some queues are being read or written.</li>
  *</ul>
  *
  *@param uwQueueID     [IN]      Queue ID. The value range is [1,LOSCFG_BASE_IPC_QUEUE_LIMIT].
  *
  *@retval   #LOS_OK                            0x00000000: The queue is successfully deleted.
  *@retval   #OS_ERRNO_QUEUE_NOT_FOUND          0x02000605: The queue cannot be found.
  *@retval   #OS_ERRNO_QUEUE_NOT_CREATE         0x0200060a: The queue handle passed in when the queue is being deleted is incorrect.
  *@retval   #OS_ERRNO_QUEUE_IN_TSKUSE          0x02000608: The queue that blocks a task cannot be deleted.
  *@retval   #OS_ERRNO_QUEUE_IN_TSKWRITE        0x0200060b: Queue reading and writing are not synchronous.
  *@par Dependency:
  *<ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
  *@see LOS_QueueCreate
  *@since Huawei LiteOS V100R001C00
  */
extern UINT32 LOS_QueueDelete(UINT32 uwQueueID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_QUEUE_H */
