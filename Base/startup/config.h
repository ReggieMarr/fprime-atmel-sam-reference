#ifndef __BASE_CONFIG__
#define __BASE_CONFIG__
#include "FreeRTOS.h"
#include "task.h"
// Required function prototypes due to config settings

extern void vApplicationMallocFailedHook(void);
extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

extern int main(void);
extern void main_cpp();

#endif
