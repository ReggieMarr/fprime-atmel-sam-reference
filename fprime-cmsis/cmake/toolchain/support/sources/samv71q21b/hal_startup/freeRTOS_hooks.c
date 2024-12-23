#include "FreeRTOS.h"
#include "startup_samv71q21b.h"
#include "sam.h"

// Runtime statistics variables
static unsigned long ulHighFrequencyTimerTicks = 0;

#ifndef configSUPPORT_STATIC_ALLOCATION
#error "This rtos implementation assumes static allocation support"
#endif

#ifdef IDLE_TASK_SIZE
#undef IDLE_TASK_SIZE
#endif
#define IDLE_TASK_SIZE 2000
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[IDLE_TASK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer,
                                              uint32_t* pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = IDLE_TASK_SIZE;
}
#ifndef configGENERATE_RUN_TIME_STATS
#error "This rtos implementation assumes runtime stat support"
#endif
void configureDWTForRunTimeStats(void)
{
    /* Configure DWT for runtime stats */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    ulHighFrequencyTimerTicks = 0;
}

unsigned long getTimerValue(void)
{
    /* Return the current timer value for runtime stats */
    return DWT->CYCCNT;
}

// TODO port over those used in arduino example
/* void vApplicationMallocFailedHook(void) */
/* { */
/*     /\* Called if a malloc() fails *\/ */
/*     taskDISABLE_INTERRUPTS(); */
/*     for(;;); */
/* } */

/* void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) */
/* { */
/*     /\* Called if a stack overflow is detected *\/ */
/*     (void) xTask; */
/*     (void) pcTaskName; */
/*     taskDISABLE_INTERRUPTS(); */
/*     for(;;); */
/* } */
