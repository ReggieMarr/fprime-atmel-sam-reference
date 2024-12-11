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
  MPLABX Event Recorder Source File

  Company:
    Microchip Technology Inc.
 *******************************************************************************/

#include <string.h>
#include "EventRecorderRTOSTrace.h"
#include "EventRecorderRTOSHook.h"
#include "EventRecorder.h"

uint32_t gTaskNumber = 0;
uint32_t gTimerNumber = 0;
uint32_t gQueueNumber = 0;
uint32_t gMutexNumber = 0;
uint32_t gEventGroupNumber = 0;
uint32_t gStreamBufferNumber = 0;


/* Called before stepping the tick count after waking from tickless idle
 * sleep. */
void EventRecorderIncreaseTickCount(uint32_t xTicksToJump) {
    EventRecord2(FreeRTOS_TICK_JUMP, xTicksToJump, FreeRTOS_UNUSED);
}

void EventRecorderMoveTaskToReadyState(void* pxTCB) {
    uint32_t xTaskNumber = RTOSTaskGetTaskNumber(pxTCB);
    EventRecord2(FreeRTOS_TASK_READY, xTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderTaskIncrementTick(uint32_t xTickCount) {
//    EventRecord2(FreeRTOS_TICK_COUNT, xTickCount, FreeRTOS_UNUSED);
}

void EventRecorderTaskSwitchedIn() {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_SWITCHED_IN, xCurrentTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderTaskSwitchedOut() {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_SWITCHED_OUT, xCurrentTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderTaskCreate(void* pxNewTCB) {
    RTOSTaskSetTaskNumber(pxNewTCB, gTaskNumber);
    char* xTaskName = RTOSTaskGetTaskName(pxNewTCB);
    size_t length = strlen(xTaskName);
    char data[length + 4];
    memcpy(data, &gTaskNumber, 4);
    memcpy(&data[4], xTaskName, length);
    EventRecordData(FreeRTOS_TASK_CREATE, data, length + 4);
    gTaskNumber++;
}

void EventRecorderTaskCreateFailed() {
    EventRecord2(FreeRTOS_TASK_CREATE_FAILED, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderLowPowerIdleBegin() {
    EventRecord2(FreeRTOS_LOW_POWER_IDLE_BEGIN, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderLowPoweridleEnd() {
    EventRecord2(FreeRTOS_LOW_POWER_IDLE_END, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderTaskSuspend(void* pxTCB) {
    uint32_t xTaskNumber = RTOSTaskGetTaskNumber(pxTCB);
    EventRecord2(FreeRTOS_TASK_SUSPEND, xTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderTaskDelay() {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_DELAY, xCurrentTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderTaskDelayUntil(uint32_t x) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_DELAY_UNTIL, xCurrentTaskNumber, x);
}

void EventRecorderQueueCreate(void* pxNewQueue) {
    RTOSQueueSetQueueNumber(pxNewQueue, gQueueNumber);
    uint8_t type = RTOSQueueGetQueueType(pxNewQueue);
    EventRecord2(FreeRTOS_QUEUE_CREATE, gQueueNumber, type);
    gQueueNumber++;
}

void EventRecorderQueueCreateFailed(uint8_t ucQueueType) {
    EventRecord2(FreeRTOS_QUEUE_CREATE_FAILED, ucQueueType, FreeRTOS_UNUSED);
}

void EventRecorderQueueDelete(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_DELETE, xQueueNumber, type);
}

void EventRecorderCreateCountingSemaphore() {
    EventRecord2(FreeRTOS_SEMAPHORE_CREATE, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderCreateCountingSemaphoreFailed() {
    EventRecord2(FreeRTOS_SEMAPHORE_CREATE_FAILED, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderCreateMutex(void* pxNewQueue) {
    RTOSQueueSetQueueNumber(pxNewQueue, gMutexNumber);
    uint8_t type = RTOSQueueGetQueueType(pxNewQueue);
    EventRecord2(FreeRTOS_MUTEX_CREATE, gMutexNumber, type);
    gMutexNumber++;
}

void EventRecorderCreateMutexFailed() {
    EventRecord2(FreeRTOS_MUTEX_CREATE_FAILED, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderQueueSend(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_SEND, xQueueNumber, type);
}

void EventRecorderQueueSendFailed(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_SEND_FAILED, xQueueNumber, type);
}

void EventRecorderBlockingOnQueueSend(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_BLOCKING_ON_QUEUE_SEND, xQueueNumber, type);
}

void EventRecorderQueueSendFromISR(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_SEND_FROM_ISR, xQueueNumber, type);
}

void EventRecorderQueueSendFromISRFailed(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_SEND_FROM_ISR_FAILED, xQueueNumber, type);
}

void EventRecorderQueueReceive(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_RECEIVE, xQueueNumber, type);
}

void EventRecorderQueueReceiveFailed(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_RECEIVE_FAILED, xQueueNumber, type);
}

void EventRecorderBlockingOnQueueReceive(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_BLOCKING_ON_QUEUE_RECEIVE, xQueueNumber, type);
}

void EventRecorderQueuePeek(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_PEEK, xQueueNumber, type);
}

void EventRecorderQueuePeekFailed(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_PEEK_FAILED, xQueueNumber, type);
}

void EventRecorderBlockingOnQueuePeek(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_BLOCKING_ON_QUEUE_PEEK, xQueueNumber, type);
}

void EventRecorderQueueReceiveFromISR(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_RECEIVE_FROM_ISR, xQueueNumber, type);
}

void EventRecorderQueueReceiveFromISRFailed(void* pxQueue) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(pxQueue);
    uint8_t type = RTOSQueueGetQueueType(pxQueue);
    EventRecord2(FreeRTOS_QUEUE_RECEIVE_FROM_ISR_FAILED, xQueueNumber, type);
}

void EventRecorderQueueRegistryAdd(void* xQueue, char* pcQueueName) {
    uint32_t xQueueNumber = RTOSQueueGetQueueNumber(xQueue);
    size_t length = strlen(pcQueueName);
    char data[length + 4];
    memcpy(data, &xQueueNumber, 4);
    memcpy(&data[4], pcQueueName, length);
    EventRecordData(FreeRTOS_QUEUE_REGISTRY_ADD, data, length + 4);
    xQueueNumber++;
}

void EventRecorderTaskPrioritySet(void* pxTCB, unsigned long uxNewPriority) {
    uint32_t xTaskNumber = RTOSTaskGetTaskNumber(pxTCB);
    EventRecord2(FreeRTOS_TASK_PRIORITY_SET, xTaskNumber, uxNewPriority);
}

void EventRecorderTaskPriorityInherit(void* pxTCBOfMutexHolder, unsigned long uxInheritedPriority) {
    uint32_t xTaskNumber = RTOSTaskGetTaskNumber(pxTCBOfMutexHolder);
    EventRecord2(FreeRTOS_TASK_PRIORITY_INHERIT, xTaskNumber, uxInheritedPriority);
}

void EventRecorderTaskResume(void* pxTaskToResume) {
    uint32_t xTaskNumber = RTOSTaskGetTaskNumber(pxTaskToResume);
    EventRecord2(FreeRTOS_TASK_RESUME, xTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderTaskResumeFromISR(void* pxTaskToResume) {
    uint32_t xTaskNumber = RTOSTaskGetTaskNumber(pxTaskToResume);
    EventRecord2(FreeRTOS_TASK_RESUME_FROM_ISR, xTaskNumber, FreeRTOS_UNUSED);
}

void EventRecorderMalloc(void* pvAddress, size_t uiSize) {
    EventRecord2(FreeRTOS_MALLOC, (uintptr_t)pvAddress, uiSize);
}

void EventRecorderFree(void* pvAddress, size_t uiSize) {
    EventRecord2(FreeRTOS_FREE, (uintptr_t)pvAddress, uiSize);
}

void EventRecorderTimerCreate(void* pxNewTimer) {
    const char* xTimerName = RTOSTimerGetTimerName(pxNewTimer);
    RTOSTimerSetTimerNumber(pxNewTimer, gTimerNumber);
    size_t length = strlen(xTimerName);
    char data[length + 4];
    memcpy(data, &gTimerNumber, 4);
    memcpy(&data[4], xTimerName, length);
    EventRecordData(FreeRTOS_TIMER_CREATE, data, length + 4);
    gTimerNumber++;
}

void EventRecorderTimerCreateFailed() {
    EventRecord2(FreeRTOS_TIMER_CREATE_FAILED, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderTimerCommandSend(void* xTimer, uint32_t xCommandID, uint32_t xOptionalValue, uint32_t xReturn) {
    uint32_t xTimerNumber = RTOSTimerGetTimerNumber(xTimer);
    EventRecord4(FreeRTOS_TIMER_COMMAND_SEND, xTimerNumber, xCommandID, xOptionalValue, xReturn);
}

void EventRecorderTimerExpired(void* pxTimer) {
    uint32_t xTimerNumber = RTOSTimerGetTimerNumber(pxTimer);
    EventRecord2(FreeRTOS_TIMER_EXPIRED, xTimerNumber, FreeRTOS_UNUSED);
}

void EventRecorderPendFuncCall(void* xFunctionToPend, void* pvParameter1, uint32_t ulParameter2, uint32_t xReturn) {
    EventRecord4(FreeRTOS_TIMER_PEND_FUNC_CALL, (uintptr_t)xFunctionToPend, (uintptr_t)pvParameter1, ulParameter2, xReturn);
}

void EventRecorderPendFuncCallFromISR(void* xFunctionToPend, void* pvParameter1, uint32_t ulParameter2, uint32_t xReturn) {
    EventRecord4(FreeRTOS_TIMER_PEND_FUNC_CALL_FROM_ISR, (uintptr_t)xFunctionToPend, (uintptr_t)pvParameter1, ulParameter2, xReturn);
}

// ============================= Begin EVENT GROUP ==================================

void EventRecorderEventGroupCreate(void* xEventGroup) {
    RTOSEventGroupSetNumber(xEventGroup, gEventGroupNumber);
    EventRecord2(FreeRTOS_EVENT_GROUP_CREATE, gEventGroupNumber, FreeRTOS_UNUSED);
    gEventGroupNumber++;
}

void EventRecorderEventGroupDelete(void* xEventGroup) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord2(FreeRTOS_EVENT_GROUP_DELETE, xGroupNumber, FreeRTOS_UNUSED);
}

void EventRecorderEventGroupCreateFailed() {
    EventRecord2(FreeRTOS_EVENT_GROUP_CREATE_FAILED, FreeRTOS_UNUSED, FreeRTOS_UNUSED);
}

void EventRecorderEventGroupSyncBlock(void* xEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord4(FreeRTOS_EVENT_GROUP_SYNC_BLOCK, xGroupNumber, uxBitsToSet, uxBitsToWaitFor, FreeRTOS_UNUSED);
}

void EventRecorderEventGroupSyncEnd(void* xEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord4(FreeRTOS_EVENT_GROUP_SYNC_END, xGroupNumber, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred);
}

void EventRecorderEventGroupWaitBitsBlock(void* xEventGroup, uint32_t uxBitsToWaitFor) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord2(FreeRTOS_EVENT_GROUP_WAIT_BITS_BLOCK, xGroupNumber, uxBitsToWaitFor);
}

void EventRecorderEventGroupWaitBitsEnd(void* xEventGroup, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord4(FreeRTOS_EVENT_GROUP_WAIT_BITS_END, xGroupNumber, uxBitsToWaitFor, xTimeoutOccurred, FreeRTOS_UNUSED);
}

void EventRecorderEventGroupClearBits(void* xEventGroup, uint32_t uxBitsToClear) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord2(FreeRTOS_EVENT_GROUP_CLEAR_BITS, xGroupNumber, uxBitsToClear);
}

void EventRecorderEventGroupClearBitsFromISR(void* xEventGroup, uint32_t uxBitsToClear) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord2(FreeRTOS_EVENT_GROUP_CLEAR_BITS_FROM_ISR, xGroupNumber, uxBitsToClear);
}

void EventRecorderEventGroupSetBits(void* xEventGroup, uint32_t uxBitsToSet) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord2(FreeRTOS_EVENT_GROUP_SET_BITS, xGroupNumber, uxBitsToSet);
}

void EventRecorderEventGroupSetBitsFromISR(void* xEventGroup, uint32_t uxBitsToSet) {
    uint32_t xGroupNumber = RTOSEventGroupGetNumber(xEventGroup);
    EventRecord2(FreeRTOS_EVENT_GROUP_SET_BITS_FROM_ISR, xGroupNumber, uxBitsToSet);
}

// ============================= End EVENT GROUP ==================================

void EventRecorderTaskNotifyTake(uint32_t uxIndexToWait) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY_TAKE, xCurrentTaskNumber, uxIndexToWait);
}

void EventRecorderTaskNotifyTakeBlock(uint32_t uxIndexToWait) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY_TAKE_BLOCK, xCurrentTaskNumber, uxIndexToWait);
}

void EventRecorderTaskNotifyWait(uint32_t uxIndexToWait) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY_WAIT, xCurrentTaskNumber, uxIndexToWait);
}

void EventRecorderTaskNotifyWaitBlock(uint32_t uxIndexToWait) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY_WAIT_BLOCK, xCurrentTaskNumber, uxIndexToWait);
}

void EventRecorderTaskNotify(uint32_t uxIndexToNotify) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY, xCurrentTaskNumber, uxIndexToNotify);
}

void EventRecorderTaskNotifyFromISR(uint32_t uxIndexToNotify) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY_FROM_ISR, xCurrentTaskNumber, uxIndexToNotify);
}

void EventRecorderTaskNotifyGiveFromISR(uint32_t uxIndexToNotify) {
    uint32_t xCurrentTaskNumber = RTOSTaskGetCurrentTaskNumber();
    EventRecord2(FreeRTOS_TASK_NOTIFY_GIVE_FROM_ISR, xCurrentTaskNumber, uxIndexToNotify);
}

// ============================= Begin STREAM BUFFER ==================================

void EventRecorderStreamBufferCreate(void* pxStreamBuffer, uint32_t xIsMessageBuffer) {
    RTOSStreamBufferSetNumber(pxStreamBuffer, gStreamBufferNumber);
    EventRecord2(FreeRTOS_STREAM_BUFFER_CREATE, gStreamBufferNumber, xIsMessageBuffer);
}

void EventRecorderStreamBufferCreateFailed(uint32_t xIsMessageBuffer) {
    EventRecord2(FreeRTOS_STREAM_BUFFER_CREATE_FAILED, xIsMessageBuffer, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferCreateStaticFailed(void* xReturn, uint32_t xIsMessageBuffer) {
    EventRecord2(FreeRTOS_STREAM_BUFFER_CREATE_STATIC_FAILED, (uintptr_t)xReturn, xIsMessageBuffer);
}

void EventRecorderStreamBufferDelete(void* xStreamBuffer) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_DELETE, xStreamBufferNumber, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferReset(void* xStreamBuffer) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_RESET, xStreamBufferNumber, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferSend(void* xStreamBuffer, size_t xBytesSent) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_SEND, xStreamBufferNumber, xBytesSent);
}

void EventRecorderBlockingOnStreamBufferSend(void* xStreamBuffer) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_BLOCKING_ON_STREAM_BUFFER_SEND, xStreamBufferNumber, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferSendFailed(void* xStreamBuffer) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_SEND_FAILED, xStreamBufferNumber, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferReceive(void* xStreamBuffer, size_t xReceivedLength) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_RECEIVE, xStreamBufferNumber, xReceivedLength);
}

void EventRecorderBlockingOnStreamBufferReceive(void* xStreamBuffer) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_BLOCKING_ON_STREAM_BUFFER_RECEIVE, xStreamBufferNumber, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferReceiveFailed(void* xStreamBuffer) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_RECEIVE_FAILED, xStreamBufferNumber, FreeRTOS_UNUSED);
}

void EventRecorderStreamBufferSendFromISR(void* xStreamBuffer, size_t xBytesSent) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_SEND_FROM_ISR, xStreamBufferNumber, xBytesSent);
}

void EventRecorderStreamBufferReceiveFromISR(void* xStreamBuffer, size_t xReceivedLength) {
    uint32_t xStreamBufferNumber = RTOSStreamBufferGetNumber(xStreamBuffer);
    EventRecord2(FreeRTOS_STREAM_BUFFER_RECEIVE_FROM_ISR, xStreamBufferNumber, xReceivedLength);
}
