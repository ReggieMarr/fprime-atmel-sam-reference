#ifndef __STARTUP_SAMV71Q21B__
#define __STARTUP_SAMV71Q21B__
#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"

// Required function prototypes due to config settings

extern void vApplicationMallocFailedHook(void);
extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName);

extern int main(void);

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

// Reset any app specific pins,
// ensure hardware states are as desired for startup conditions
// NOTE this is pre-access to the C library and/or other important sections
extern void __attribute__((long_call)) _on_reset(void);
extern void __attribute__((weak, long_call)) _on_bootstrap(void);

extern void __libc_init_array(void);

#ifdef __cplusplus
}
#endif
#endif
