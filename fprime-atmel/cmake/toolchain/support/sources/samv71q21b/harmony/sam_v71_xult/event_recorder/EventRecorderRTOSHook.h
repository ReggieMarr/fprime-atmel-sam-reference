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

#ifndef EVENTRECORDERRTOSHOOK_H
#define	EVENTRECORDERRTOSHOOK_H

#include "FreeRTOS.h"
#include "EventRecorderRTOSTrace.h"

#ifdef	__cplusplus
extern "C" {
#endif

#undef traceINCREASE_TICK_COUNT
#define traceINCREASE_TICK_COUNT(xTicksToJump) EventRecorderIncreaseTickCount(xTicksToJump);

#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE(pxTCB) EventRecorderMoveTaskToReadyState(pxTCB);

#undef traceTASK_INCREMENT_TICK
#define traceTASK_INCREMENT_TICK(xTickCount) EventRecorderTaskIncrementTick(xTickCount);

#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() EventRecorderTaskSwitchedIn();

#undef traceTASK_SWITCHED_OUT
#define traceTASK_SWITCHED_OUT() EventRecorderTaskSwitchedOut();
    
#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) EventRecorderTaskCreate(pxNewTCB);

#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() EventRecorderTaskCreateFailed();

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() EventRecorderLowPowerIdleBegin();

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() EventRecorderLowPoweridleEnd();

#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND(pxTCB) EventRecorderTaskSuspend(pxTCB);

#undef traceTASK_DELAY
#define traceTASK_DELAY() EventRecorderTaskDelay();

#undef traceTASK_DELAY_UNTIL
#define traceTASK_DELAY_UNTIL(x) EventRecorderTaskDelayUntil(x);

#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE(pxNewQueue) EventRecorderQueueCreate(pxNewQueue);

#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED(ucQueueType) EventRecorderQueueCreateFailed(ucQueueType);

#undef traceQUEUE_DELETE
#define traceQUEUE_DELETE(pxQueue) EventRecorderQueueDelete(pxQueue);

#undef traceCREATE_COUNTING_SEMAPHORE
#define traceCREATE_COUNTING_SEMAPHORE() EventRecorderCreateCountingSemaphore();

#undef traceCREATE_COUNTING_SEMAPHORE_FAILED
#define traceCREATE_COUNTING_SEMAPHORE_FAILED() EventRecorderCreateCountingSemaphoreFailed();

#undef traceCREATE_MUTEX
#define traceCREATE_MUTEX(pxNewQueue) EventRecorderCreateMutex(pxNewQueue);

#undef traceCREATE_MUTEX_FAILED
#define traceCREATE_MUTEX_FAILED() EventRecorderCreateMutexFailed();

#undef traceQUEUE_SEND
#define traceQUEUE_SEND(pxQueue) EventRecorderQueueSend(pxQueue);

#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED(pxQueue) EventRecorderQueueSendFailed(pxQueue);

#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND(pxQueue) EventRecorderBlockingOnQueueSend(pxQueue);

#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR(pxQueue) EventRecorderQueueSendFromISR(pxQueue);

#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED(pxQueue) EventRecorderQueueSendFromISRFailed(pxQueue);

#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE(pxQueue) EventRecorderQueueReceive(pxQueue);

#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED(pxQueue) EventRecorderQueueReceiveFailed(pxQueue);

#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE(pxQueue) EventRecorderBlockingOnQueueReceive(pxQueue);

#undef traceQUEUE_PEEK_FAILED
#define traceQUEUE_PEEK_FAILED(pxQueue) EventRecorderQueuePeekFailed(pxQueue);

#undef traceBLOCKING_ON_QUEUE_PEEK
#define traceBLOCKING_ON_QUEUE_PEEK(pxQueue) EventRecorderBlockingOnQueuePeek(pxQueue);

#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR(pxQueue) EventRecorderQueueReceiveFromISR(pxQueue);

#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED(pxQueue) EventRecorderQueueReceiveFromISRFailed(pxQueue);

#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK(pxQueue) EventRecorderQueuePeek(pxQueue);

#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET(pxTCB, uxNewPriority) EventRecorderTaskPrioritySet(pxTCB, uxNewPriority);

#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority) EventRecorderTaskPriorityInherit(pxTCBOfMutexHolder, uxInheritedPriority);

#undef traceTASK_RESUME
#define traceTASK_RESUME(pxTaskToResume) EventRecorderTaskResume(pxTaskToResume);

#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR(pxTaskToResume) EventRecorderTaskResumeFromISR(pxTaskToResume);

#undef traceMALLOC
#define traceMALLOC(pvAddress, uiSize) EventRecorderMalloc(pvAddress, uiSize);

#undef traceFREE
#define traceFREE(pvAddress, uiSize) EventRecorderFree(pvAddress, uiSize);

#undef traceTIMER_CREATE
#define traceTIMER_CREATE(pxNewTimer) EventRecorderTimerCreate(pxNewTimer);

#undef traceTIMER_CREATE_FAILED
#define traceTIMER_CREATE_FAILED() EventRecorderTimerCreateFailed();

#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(xTimer, xMessageID, xMessageValueValue, xReturn) EventRecorderTimerCommandSend(xTimer, xMessageID, xMessageValueValue, xReturn);

#undef traceTIMER_EXPIRED
#define traceTIMER_EXPIRED(pxTimer) EventRecorderTimerExpired(pxTimer);

#undef tracePEND_FUNC_CALL
#define tracePEND_FUNC_CALL(xFunctionToPend, pvParameter1, ulParameter2, ret) EventRecorderPendFuncCall(xFunctionToPend, pvParameter1, ulParameter2, ret);

#undef tracePEND_FUNC_CALL_FROM_ISR
#define tracePEND_FUNC_CALL_FROM_ISR(xFunctionToPend, pvParameter1, ulParameter2, ret) EventRecorderPendFuncCallFromISR(xFunctionToPend, pvParameter1, ulParameter2, ret);

    // ============================= Begin EVENT GROUP ==================================

#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(xEventGroup) EventRecorderEventGroupCreate(xEventGroup);

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(xEventGroup) EventRecorderEventGroupDelete(xEventGroup);

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() EventRecorderEventGroupCreateFailed();

#undef traceEVENT_GROUP_SYNC_BLOCK
#define traceEVENT_GROUP_SYNC_BLOCK(xEventGroup, uxBitsToSet, uxBitsToWaitFor) EventRecorderEventGroupSyncBlock(xEventGroup, uxBitsToSet, uxBitsToWaitFor);

#undef traceEVENT_GROUP_SYNC_END
#define traceEVENT_GROUP_SYNC_END(xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred) EventRecorderEventGroupSyncEnd(xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred);

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(xEventGroup, uxBitsToWaitFor) EventRecorderEventGroupWaitBitsBlock(xEventGroup, uxBitsToWaitFor);

#undef traceEVENT_GROUP_WAIT_BITS_END
#define traceEVENT_GROUP_WAIT_BITS_END(xEventGroup, uxBitsToWaitFor, xTimeoutOccurred) EventRecorderEventGroupWaitBitsEnd(xEventGroup, uxBitsToWaitFor, xTimeoutOccurred);

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(xEventGroup, uxBitsToClear) EventRecorderEventGroupClearBits(xEventGroup, uxBitsToClear);

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(xEventGroup, uxBitsToClear) EventRecorderEventGroupClearBitsFromISR(xEventGroup, uxBitsToClear);

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(xEventGroup, uxBitsToSet) EventRecorderEventGroupSetBits(xEventGroup, uxBitsToSet);

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(xEventGroup, uxBitsToSet) EventRecorderEventGroupSetBitsFromISR(xEventGroup, uxBitsToSet);

    // ============================= End EVENT GROUP ==================================

#undef traceTASK_NOTIFY_TAKE
#define traceTASK_NOTIFY_TAKE(uxIndexToWait) EventRecorderTaskNotifyTake(uxIndexToWait);

#undef traceTASK_NOTIFY_TAKE_BLOCK
#define traceTASK_NOTIFY_TAKE_BLOCK(uxIndexToWait) EventRecorderTaskNotifyTakeBlock(uxIndexToWait);

#undef traceTASK_NOTIFY_WAIT
#define traceTASK_NOTIFY_WAIT(uxIndexToWait) EventRecorderTaskNotifyWait(uxIndexToWait);

#undef traceTASK_NOTIFY_WAIT_BLOCK
#define traceTASK_NOTIFY_WAIT_BLOCK(uxIndexToWait) EventRecorderTaskNotifyWaitBlock(uxIndexToWait);

#undef traceTASK_NOTIFY
#define traceTASK_NOTIFY(uxIndexToNotify) EventRecorderTaskNotify(uxIndexToNotify);

#undef traceTASK_NOTIFY_FROM_ISR
#define traceTASK_NOTIFY_FROM_ISR(uxIndexToNotify) EventRecorderTaskNotifyFromISR(uxIndexToNotify);

#undef traceTASK_NOTIFY_GIVE_FROM_ISR
#define traceTASK_NOTIFY_GIVE_FROM_ISR(uxIndexToNotify) EventRecorderTaskNotifyGiveFromISR(uxIndexToNotify);

#undef traceQUEUE_REGISTRY_ADD
#define traceQUEUE_REGISTRY_ADD(xQueue, pcQueueName) EventRecorderQueueRegistryAdd(xQueue, pcQueueName);

    // ============================= Begin STREAM BUFFER ==================================

#undef traceSTREAM_BUFFER_CREATE
#define traceSTREAM_BUFFER_CREATE(pxStreamBuffer, xIsMessageBuffer) EventRecorderStreamBufferCreate(pxStreamBuffer, xIsMessageBuffer);

#undef traceSTREAM_BUFFER_CREATE_FAILED
#define traceSTREAM_BUFFER_CREATE_FAILED(xIsMessageBuffer) EventRecorderStreamBufferCreateFailed(xIsMessageBuffer);

#undef traceSTREAM_BUFFER_CREATE_STATIC_FAILED
#define traceSTREAM_BUFFER_CREATE_STATIC_FAILED(xReturn, xIsMessageBuffer) EventRecorderStreamBufferCreateStaticFailed(xReturn, xIsMessageBuffer);

#undef traceSTREAM_BUFFER_DELETE
#define traceSTREAM_BUFFER_DELETE(xStreamBuffer) EventRecorderStreamBufferDelete(xStreamBuffer);

#undef traceSTREAM_BUFFER_RESET
#define traceSTREAM_BUFFER_RESET(xStreamBuffer) EventRecorderStreamBufferReset(xStreamBuffer);

#undef traceSTREAM_BUFFER_SEND
#define traceSTREAM_BUFFER_SEND(xStreamBuffer, xBytesSent) EventRecorderStreamBufferSend(xStreamBuffer, xBytesSent);

#undef traceBLOCKING_ON_STREAM_BUFFER_SEND
#define traceBLOCKING_ON_STREAM_BUFFER_SEND(xStreamBuffer) EventRecorderBlockingOnStreamBufferSend(xStreamBuffer);

#undef traceSTREAM_BUFFER_SEND_FAILED
#define traceSTREAM_BUFFER_SEND_FAILED(xStreamBuffer) EventRecorderStreamBufferSendFailed(xStreamBuffer);

#undef traceSTREAM_BUFFER_RECEIVE
#define traceSTREAM_BUFFER_RECEIVE(xStreamBuffer, xReceivedLength) EventRecorderStreamBufferReceive(xStreamBuffer, xReceivedLength);

#undef traceBLOCKING_ON_STREAM_BUFFER_RECEIVE
#define traceBLOCKING_ON_STREAM_BUFFER_RECEIVE(xStreamBuffer) EventRecorderBlockingOnStreamBufferReceive(xStreamBuffer);

#undef traceSTREAM_BUFFER_RECEIVE_FAILED
#define traceSTREAM_BUFFER_RECEIVE_FAILED(xStreamBuffer) EventRecorderStreamBufferReceiveFailed(xStreamBuffer);

#undef traceSTREAM_BUFFER_SEND_FROM_ISR
#define traceSTREAM_BUFFER_SEND_FROM_ISR(xStreamBuffer, xBytesSent) EventRecorderStreamBufferSendFromISR(xStreamBuffer, xBytesSent);

#undef traceSTREAM_BUFFER_RECEIVE_FROM_ISR
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR(xStreamBuffer, xReceivedLength) EventRecorderStreamBufferReceiveFromISR(xStreamBuffer, xReceivedLength);

    // ============================= End STREAM BUFFER ==================================

unsigned long RTOSTaskGetTaskNumber(void* handle);

unsigned long RTOSTaskGetCurrentTaskNumber();

void RTOSTaskSetTaskNumber(void* handle, unsigned long number);

char* RTOSTaskGetTaskName(void* handle);

uint16_t RTOSTaskGetState(void* handle);

void RTOSQueueSetQueueNumber(void* xQueue, unsigned long uxQueueNumber);

unsigned long RTOSQueueGetQueueNumber(void* xQueue);

uint8_t RTOSQueueGetQueueType(void* xQueue);

const char* RTOSTimerGetTimerName(void* pxTimer);

void RTOSTimerSetTimerNumber(void* pxTimer, unsigned long number);

unsigned long RTOSTimerGetTimerNumber(void* pxTimer);

void RTOSEventGroupSetNumber(void* xEventGroup, unsigned long number);

unsigned long RTOSEventGroupGetNumber(void* xEventGroup);

void RTOSStreamBufferSetNumber(void* xEventGroup, unsigned long number);

unsigned long RTOSStreamBufferGetNumber(void * xEventGroup);


#ifdef	__cplusplus
}
#endif

#endif	/* EVENTRECORDERRTOSHOOK_H */
