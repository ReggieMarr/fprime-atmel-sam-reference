#ifndef _DEVICE_COMMON_H
#define _DEVICE_COMMON_H
#include "Driver_GPIO.h"
#include "samv71q21b.h"

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

/* Optional application-provided functions */
extern void __attribute__((weak, long_call)) _on_reset(void);
extern void __attribute__((weak, long_call)) _on_bootstrap(void);

#define __builtin_software_breakpoint() __BKPT(0);

/* Reset handler */
extern void Reset_Handler(void);

extern void Dummy_Handler(void);

extern ARM_DRIVER_GPIO DRIVER_GPIO;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

#define OK 0
#define ERROR -1
#define CHECK(cond, err_string, ...) \
    do {                             \
        if (!(cond)) {               \
            __VA_ARGS__;             \
            break;                   \
        }                            \
    } while (0);

#endif  // _DEVICE_COMMON_H
