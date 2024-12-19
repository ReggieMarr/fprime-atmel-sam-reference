/*******************************************************************************
 * Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/

/*******************************************************************************
  MPLABX Event Recorder Header File

  Company:
    Microchip Technology Inc.
 *******************************************************************************/

#ifndef EVENTRECORDERRTOSTRACE_H
#define	EVENTRECORDERRTOSTRACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define GET_VALUE(COMPONENT, MESSAGE) (MESSAGE + (COMPONENT << 8U)) 

/* Free RTOS event component numbers      */
#define FreeRTOS_COMPONENT_SYSTEM 0xA1U
#define FreeRTOS_COMPONENT_TASK 0xA2U
#define FreeRTOS_COMPONENT_QUEUE 0xA3U
#define FreeRTOS_COMPONENT_MEMORY 0xA4U
#define FreeRTOS_COMPONENT_TIMER 0xA5U
#define FreeRTOS_COMPONENT_EVENT_GROUP 0xA6U
#define FreeRTOS_COMPONENT_STREAM_BUFFER 0xA7U
#define FreeRTOS_COMPONENT_STATE 0xA8U
/* ====================================== */

/* Free RTOS System related          */
#define FreeRTOS_TICK_JUMP                            GET_VALUE(FreeRTOS_COMPONENT_SYSTEM, 0x1U)
#define FreeRTOS_TICK_COUNT                           GET_VALUE(FreeRTOS_COMPONENT_SYSTEM, 0x2U)
#define FreeRTOS_LOW_POWER_IDLE_BEGIN                 GET_VALUE(FreeRTOS_COMPONENT_SYSTEM, 0x3U)
#define FreeRTOS_LOW_POWER_IDLE_END                   GET_VALUE(FreeRTOS_COMPONENT_SYSTEM, 0x4U)
/* ====================================== */

/* Free RTOS Task action related          */
#define FreeRTOS_TASK_SWITCHED_IN                     GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x1U)
#define FreeRTOS_TASK_SWITCHED_OUT                    GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x2U)
#define FreeRTOS_TASK_CREATE                          GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x3U)
#define FreeRTOS_TASK_CREATE_FAILED                   GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x4U)
#define FreeRTOS_TASK_SUSPEND                         GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x5U)
#define FreeRTOS_TASK_DELAY                           GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x6U)
#define FreeRTOS_TASK_DELAY_UNTIL                     GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x7U)
#define FreeRTOS_TASK_PRIORITY_SET                    GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x8U)
#define FreeRTOS_TASK_PRIORITY_INHERIT                GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x9U)
#define FreeRTOS_TASK_RESUME                          GET_VALUE(FreeRTOS_COMPONENT_TASK, 0xAU)
#define FreeRTOS_TASK_RESUME_FROM_ISR                 GET_VALUE(FreeRTOS_COMPONENT_TASK, 0xBU)
#define FreeRTOS_TASK_NOTIFY                          GET_VALUE(FreeRTOS_COMPONENT_TASK, 0xCU)
#define FreeRTOS_TASK_NOTIFY_FROM_ISR                 GET_VALUE(FreeRTOS_COMPONENT_TASK, 0xDU)
#define FreeRTOS_TASK_NOTIFY_GIVE_FROM_ISR            GET_VALUE(FreeRTOS_COMPONENT_TASK, 0xEU)
#define FreeRTOS_TASK_NOTIFY_TAKE                     GET_VALUE(FreeRTOS_COMPONENT_TASK, 0xFU)
#define FreeRTOS_TASK_NOTIFY_TAKE_BLOCK               GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x10U)
#define FreeRTOS_TASK_NOTIFY_WAIT                     GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x11U)
#define FreeRTOS_TASK_NOTIFY_WAIT_BLOCK               GET_VALUE(FreeRTOS_COMPONENT_TASK, 0x12U)
/* ====================================== */

/* Free RTOS Queue action related         */
#define FreeRTOS_QUEUE_CREATE                         GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x1U)
#define FreeRTOS_QUEUE_CREATE_FAILED                  GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x2U)
#define FreeRTOS_QUEUE_DELETE                         GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x3U)
#define FreeRTOS_QUEUE_SEND                           GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x4U)
#define FreeRTOS_QUEUE_SEND_FAILED                    GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x5U)
#define FreeRTOS_BLOCKING_ON_QUEUE_SEND               GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x6U)
#define FreeRTOS_QUEUE_SEND_FROM_ISR                  GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x7U)
#define FreeRTOS_QUEUE_SEND_FROM_ISR_FAILED           GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x8U)
#define FreeRTOS_QUEUE_RECEIVE                        GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x9U)
#define FreeRTOS_QUEUE_RECEIVE_FAILED                 GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0xAU)
#define FreeRTOS_BLOCKING_ON_QUEUE_RECEIVE            GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0xBU)
#define FreeRTOS_QUEUE_PEEK                           GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0xCU)
#define FreeRTOS_QUEUE_PEEK_FAILED                    GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0xDU)
#define FreeRTOS_BLOCKING_ON_QUEUE_PEEK               GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0xEU)
#define FreeRTOS_QUEUE_RECEIVE_FROM_ISR               GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0xFU)
#define FreeRTOS_QUEUE_RECEIVE_FROM_ISR_FAILED        GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x10U)
#define FreeRTOS_SEMAPHORE_CREATE                     GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x11U)
#define FreeRTOS_SEMAPHORE_CREATE_FAILED              GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x12U)
#define FreeRTOS_MUTEX_CREATE                         GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x13U)
#define FreeRTOS_MUTEX_CREATE_FAILED                  GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x14U)
#define FreeRTOS_QUEUE_REGISTRY_ADD                   GET_VALUE(FreeRTOS_COMPONENT_QUEUE, 0x15U)
/* ====================================== */

/* Free RTOS Memory action related        */
#define FreeRTOS_MALLOC                               GET_VALUE(FreeRTOS_COMPONENT_MEMORY, 0x1U)
#define FreeRTOS_FREE                                 GET_VALUE(FreeRTOS_COMPONENT_MEMORY, 0x2U)
/* ====================================== */

/* Free RTOS Timer action related         */
#define FreeRTOS_TIMER_CREATE                         GET_VALUE(FreeRTOS_COMPONENT_TIMER, 0x1U)
#define FreeRTOS_TIMER_CREATE_FAILED                  GET_VALUE(FreeRTOS_COMPONENT_TIMER, 0x2U)
#define FreeRTOS_TIMER_COMMAND_SEND                   GET_VALUE(FreeRTOS_COMPONENT_TIMER, 0x3U)
#define FreeRTOS_TIMER_EXPIRED                        GET_VALUE(FreeRTOS_COMPONENT_TIMER, 0x4U)
#define FreeRTOS_TIMER_PEND_FUNC_CALL                 GET_VALUE(FreeRTOS_COMPONENT_TIMER, 0x5U)
#define FreeRTOS_TIMER_PEND_FUNC_CALL_FROM_ISR        GET_VALUE(FreeRTOS_COMPONENT_TIMER, 0x6U)
/* ====================================== */

/* Free RTOS event group related          */
#define FreeRTOS_EVENT_GROUP_CREATE                   GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x1U)
#define FreeRTOS_EVENT_GROUP_DELETE                   GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x2U)
#define FreeRTOS_EVENT_GROUP_CREATE_FAILED            GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x3U)
#define FreeRTOS_EVENT_GROUP_SYNC_BLOCK               GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x4U)
#define FreeRTOS_EVENT_GROUP_SYNC_END                 GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x5U)
#define FreeRTOS_EVENT_GROUP_WAIT_BITS_BLOCK          GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x6U)
#define FreeRTOS_EVENT_GROUP_WAIT_BITS_END            GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x7U)
#define FreeRTOS_EVENT_GROUP_CLEAR_BITS               GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x8U)
#define FreeRTOS_EVENT_GROUP_CLEAR_BITS_FROM_ISR      GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0x9U)
#define FreeRTOS_EVENT_GROUP_SET_BITS                 GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0xAU)
#define FreeRTOS_EVENT_GROUP_SET_BITS_FROM_ISR        GET_VALUE(FreeRTOS_COMPONENT_EVENT_GROUP, 0xBU)
/* ====================================== */

/* Free RTOS Stream Buffer related        */
#define FreeRTOS_STREAM_BUFFER_CREATE                 GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x1U)
#define FreeRTOS_STREAM_BUFFER_CREATE_FAILED          GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x2U)
#define FreeRTOS_STREAM_BUFFER_CREATE_STATIC_FAILED   GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x3U)
#define FreeRTOS_STREAM_BUFFER_DELETE                 GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x4U)
#define FreeRTOS_STREAM_BUFFER_RESET                  GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x5U)
#define FreeRTOS_STREAM_BUFFER_SEND                   GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x6U)
#define FreeRTOS_BLOCKING_ON_STREAM_BUFFER_SEND       GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x7U)
#define FreeRTOS_STREAM_BUFFER_SEND_FAILED            GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x8U)
#define FreeRTOS_STREAM_BUFFER_RECEIVE                GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0x9U)
#define FreeRTOS_BLOCKING_ON_STREAM_BUFFER_RECEIVE    GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0xAU)
#define FreeRTOS_STREAM_BUFFER_RECEIVE_FAILED         GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0xBU)
#define FreeRTOS_STREAM_BUFFER_SEND_FROM_ISR          GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0xCU)
#define FreeRTOS_STREAM_BUFFER_RECEIVE_FROM_ISR       GET_VALUE(FreeRTOS_COMPONENT_STREAM_BUFFER, 0xDU)
/* ====================================== */

/* Free RTOS Task state related           */
#define FreeRTOS_TASK_RUNNING                         GET_VALUE(FreeRTOS_COMPONENT_STATE, 0x1U)
#define FreeRTOS_TASK_READY                           GET_VALUE(FreeRTOS_COMPONENT_STATE, 0x2U)
#define FreeRTOS_TASK_BLOCKED                         GET_VALUE(FreeRTOS_COMPONENT_STATE, 0x3U)
#define FreeRTOS_TASK_SUSPENDED                       GET_VALUE(FreeRTOS_COMPONENT_STATE, 0x4U)
#define FreeRTOS_TASK_DELETED                         GET_VALUE(FreeRTOS_COMPONENT_STATE, 0x5U)
#define FreeRTOS_TASK_INVALID                         GET_VALUE(FreeRTOS_COMPONENT_STATE, 0x6U)
/* ====================================== */

#define FreeRTOS_UNUSED 0x00

#ifdef	__cplusplus
extern "C" {
#endif

    void EventRecorderIncreaseTickCount(uint32_t xTicksToJump);

    void EventRecorderMoveTaskToReadyState(void* pxTCB);

    void EventRecorderTaskIncrementTick(uint32_t xTickCount);

    void EventRecorderTaskSwitchedIn();
    
    void EventRecorderTaskSwitchedOut();

    void EventRecorderTaskCreate(void* pxNewTCB);

    void EventRecorderTaskCreateFailed();

    void EventRecorderLowPowerIdleBegin();

    void EventRecorderLowPoweridleEnd();

    void EventRecorderTaskSuspend(void* pxTCB);

    void EventRecorderTaskDelay();

    void EventRecorderTaskDelayUntil(uint32_t x);

    void EventRecorderQueueCreate(void* pxNewQueue);

    void EventRecorderQueueCreateFailed(uint8_t ucQueueType);

    void EventRecorderQueueDelete(void* pxQueue);

    void EventRecorderCreateCountingSemaphore();

    void EventRecorderCreateCountingSemaphoreFailed();

    void EventRecorderCreateMutex(void* pxNewQueue);

    void EventRecorderCreateMutexFailed();

    void EventRecorderQueueSend(void* pxQueue);

    void EventRecorderQueueSendFailed(void* pxQueue);

    void EventRecorderBlockingOnQueueSend(void* pxQueue);

    void EventRecorderQueueSendFromISR(void* pxQueue);

    void EventRecorderQueueSendFromISRFailed(void* pxQueue);

    void EventRecorderQueueReceive(void* pxQueue);

    void EventRecorderQueueReceiveFailed(void* pxQueue);

    void EventRecorderBlockingOnQueueReceive(void* pxQueue);

    void EventRecorderQueuePeekFailed(void* pxQueue);

    void EventRecorderBlockingOnQueuePeek(void* pxQueue);

    void EventRecorderQueueReceiveFromISR(void* pxQueue);

    void EventRecorderQueueReceiveFromISRFailed(void* pxQueue);

    void EventRecorderQueuePeek(void* pxQueue);

    void EventRecorderQueueRegistryAdd(void* xQueue, char* pcQueueName);

    void EventRecorderTaskPrioritySet(void* pxTCB, uint32_t uxNewPriority);

    void EventRecorderTaskPriorityInherit(void* pxTCBOfMutexHolder, uint32_t uxInheritedPriority);

    void EventRecorderTaskResume(void* pxTaskToResume);

    void EventRecorderTaskResumeFromISR(void* pxTaskToResume);

    void EventRecorderMalloc(void* pvAddress, size_t uiSize);

    void EventRecorderFree(void* pvAddress, size_t uiSize);

    void EventRecorderTimerCreate(void* pxNewTimer);

    void EventRecorderTimerCreateFailed();

    void EventRecorderTimerCommandSend(void* xTimer, uint32_t xCommandID, uint32_t xOptionalValue, uint32_t xReturn);

    void EventRecorderTimerExpired(void* pxTimer);

    void EventRecorderPendFuncCall(void* xFunctionToPend, void* pvParameter1, uint32_t ulParameter2, uint32_t xReturn);

    void EventRecorderPendFuncCallFromISR(void* xFunctionToPend, void* pvParameter1, uint32_t ulParameter2, uint32_t xReturn);

    // ============================= Begin EVENT GROUP ==================================

    void EventRecorderEventGroupCreate(void* xEventGroup);

    void EventRecorderEventGroupDelete(void* xEventGroup);

    void EventRecorderEventGroupCreateFailed();

    void EventRecorderEventGroupSyncBlock(void* xEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor);

    void EventRecorderEventGroupSyncEnd(void* xEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred);

    void EventRecorderEventGroupWaitBitsBlock(void* xEventGroup, uint32_t uxBitsToWaitFor);

    void EventRecorderEventGroupWaitBitsEnd(void* xEventGroup, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred);

    void EventRecorderEventGroupClearBits(void* xEventGroup, uint32_t uxBitsToClear);

    void EventRecorderEventGroupClearBitsFromISR(void* xEventGroup, uint32_t uxBitsToClear);

    void EventRecorderEventGroupSetBits(void* xEventGroup, uint32_t uxBitsToSet);

    void EventRecorderEventGroupSetBitsFromISR(void* xEventGroup, uint32_t uxBitsToSet);

    // ============================= End EVENT GROUP ==================================

    void EventRecorderTaskNotifyTake(uint32_t uxIndexToWait);

    void EventRecorderTaskNotifyTakeBlock(uint32_t uxIndexToWait);

    void EventRecorderTaskNotifyWait(uint32_t uxIndexToWait);

    void EventRecorderTaskNotifyWaitBlock(uint32_t uxIndexToWait);

    void EventRecorderTaskNotify(uint32_t uxIndexToNotify);

    void EventRecorderTaskNotifyFromISR(uint32_t uxIndexToNotify);

    void EventRecorderTaskNotifyGiveFromISR(uint32_t uxIndexToNotify);

    // ============================= Begin STREAM BUFFER ==================================

    void EventRecorderStreamBufferCreate(void* pxStreamBuffer, uint32_t xIsMessageBuffer);

    void EventRecorderStreamBufferCreateFailed(uint32_t xIsMessageBuffer);

    void EventRecorderStreamBufferCreateStaticFailed(void* xReturn, uint32_t xIsMessageBuffer);

    void EventRecorderStreamBufferDelete(void* xStreamBuffer);

    void EventRecorderStreamBufferReset(void* xStreamBuffer);

    void EventRecorderStreamBufferSend(void* xStreamBuffer, size_t xBytesSent);

    void EventRecorderBlockingOnStreamBufferSend(void* xStreamBuffer);

    void EventRecorderStreamBufferSendFailed(void* xStreamBuffer);

    void EventRecorderStreamBufferReceive(void* xStreamBuffer, size_t xReceivedLength);

    void EventRecorderBlockingOnStreamBufferReceive(void* xStreamBuffer);

    void EventRecorderStreamBufferReceiveFailed(void* xStreamBuffer);

    void EventRecorderStreamBufferSendFromISR(void* xStreamBuffer, size_t xBytesSent);

    void EventRecorderStreamBufferReceiveFromISR(void* xStreamBuffer, size_t xReceivedLength);


#ifdef	__cplusplus
}
#endif

#endif	/* EVENTRECORDERRTOSTRACE_H */
