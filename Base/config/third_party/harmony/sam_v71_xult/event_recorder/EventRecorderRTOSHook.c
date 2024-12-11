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

#include "EventRecorderRTOSHook.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"

unsigned long RTOSTaskGetTaskNumber(void* handle) {
    return uxTaskGetTaskNumber(handle);
}

unsigned long RTOSTaskGetCurrentTaskNumber() {
    void* pxCurrentTCB = xTaskGetCurrentTaskHandle();
    return uxTaskGetTaskNumber(pxCurrentTCB);
}

void RTOSTaskSetTaskNumber(void* handle, unsigned long number) {
    vTaskSetTaskNumber(handle, number);
}

char* RTOSTaskGetTaskName(void* handle) {
    return pcTaskGetName(handle);
}

uint16_t RTOSTaskGetState(void* handle) {
    switch(eTaskGetState(handle)) {
        case eBlocked:
            return FreeRTOS_TASK_BLOCKED;
        case eDeleted:
            return FreeRTOS_TASK_DELETED;
        case eInvalid:
            return FreeRTOS_TASK_INVALID;
        case eReady:
            return FreeRTOS_TASK_READY;
        case eRunning:
            return FreeRTOS_TASK_RUNNING;
        case eSuspended:
            return FreeRTOS_TASK_SUSPENDED;
    }
    return FreeRTOS_TASK_INVALID; 
}

void RTOSQueueSetQueueNumber(void* xQueue, unsigned long uxQueueNumber) {
    vQueueSetQueueNumber(xQueue, uxQueueNumber);
}

unsigned long RTOSQueueGetQueueNumber(void* xQueue) {
    return uxQueueGetQueueNumber(xQueue);
}

uint8_t RTOSQueueGetQueueType(void* xQueue) {
    return ucQueueGetQueueType(xQueue);
}

const char* RTOSTimerGetTimerName(void* pxTimer) {
#if (configUSE_TIMERS == 1)
    return pcTimerGetName(pxTimer);
#else
    return NULL;
#endif
}

void RTOSTimerSetTimerNumber(void* pxTimer, unsigned long number) {
#if (configUSE_TIMERS == 1)
    vTimerSetTimerNumber(pxTimer, number);
#endif
}

uint32_t RTOSTimerGetTimerNumber(void* pxTimer) {
#if (configUSE_TIMERS == 1)
    return uxTimerGetTimerNumber(pxTimer);
#else
    return -1;
#endif
}

void RTOSEventGroupSetNumber(void* xEventGroup, unsigned long number) {
    vEventGroupSetNumber(xEventGroup, number);
}

unsigned long RTOSEventGroupGetNumber(void* xEventGroup) {
    return uxEventGroupGetNumber(xEventGroup);
}


void RTOSStreamBufferSetNumber(void* xEventGroup, unsigned long number) {
    vStreamBufferSetStreamBufferNumber(xEventGroup, number);
}

unsigned long RTOSStreamBufferGetNumber(void * xEventGroup) {
    return uxStreamBufferGetStreamBufferNumber(xEventGroup);
}