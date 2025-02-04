#ifndef PIO_CONFIG_H_
#define PIO_CONFIG_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "Driver_GPIO.h"

typedef enum {
    PIO_FUNC_GPIO,      // Controlled by PIO (PER=1)
    PIO_FUNC_PERIPH_A,  // ABCDSR[1:0] = 00 (PDR=1)
    PIO_FUNC_PERIPH_B,  // ABCDSR[1:0] = 01 (PDR=1)
    PIO_FUNC_PERIPH_C,  // ABCDSR[1:0] = 10 (PDR=1)
    PIO_FUNC_PERIPH_D   // ABCDSR[1:0] = 11 (PDR=1)
} pioPinFunction_t;

// Interrupt/Event configuration. NOTE Only valid for GPIO or WKUP pins
typedef struct {
    ARM_GPIO_EVENT_TRIGGER trigger;
    bool filterEnabled;             // PIO_IFER/IFDR
    bool filterClockEnabled;        // PIO_IFSCDR/IFSCER
    uint32_t filterClockDivider;    // PIO_SCDR
} pioInterruptConfig_t;

typedef struct {
    // Electrical characteristics
    ARM_GPIO_PULL_RESISTOR pullResistor;  // PIO_PUDR/PUER, PIO_PPDDR/PPDER
    uint32_t driveStrength;               // PIO_DRIVER
    pioPinFunction_t functionalType;   // Basic pin function selection
} pioBaseConfig_t;

// GPIO specific configuration
typedef struct {
    pioBaseConfig_t genCfg;
    ARM_GPIO_DIRECTION direction;    // PIO_OER/ODR
    bool outputWriteEnabled;         // PIO_OWER
    uint32_t initialState;          // PIO_ODSR initial value
    bool isOpenDrain;               // PIO_MDER/MDDR (multi-drive)
    // Optional interrupt configuration
    pioInterruptConfig_t irqConfig;
} pioGPIOConfig_t;

//NOTE this header is meant to be generic to SAMV71's but the PIN_PE5 is specific to samv71q21b
//TODO find another way to determine the max pin number
#define MAX_PIO_PIN     134U    // PE5 is the highest pin number (133)
#define PIN_IS_AVAILABLE(n) ((n) < MAX_PIO_PIN && (n) != PIO_PIN_NONE)

// The pin argument provided will be some value between 0-133 where each group of 32 pins maps to a port
// (with the exception of the final port, PORTE, which only has 5 pins)
// by shifting the pin argument by 5 we get some value between 0-4 which maps to ports A-E
#define PIN_TO_PORT(pin) (PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5)))
#define PORT_TO_IDX(port) ((port - PIOA_BASE_ADDRESS) / 0x200U)
#define PORT_A PORT_TO_IDX(PIOA_BASE_ADDRESS)
#define PORT_B PORT_TO_IDX(PIOB_BASE_ADDRESS)
#define PORT_C PORT_TO_IDX(PIOC_BASE_ADDRESS)
#define PORT_D PORT_TO_IDX(PIOD_BASE_ADDRESS)
#define PORT_E PORT_TO_IDX(PIOE_BASE_ADDRESS)
// by masking the lower byte with 0x1F we get some value between 0-31 which maps to the pin number within the port
// This is then shifted to the left by 1 to get the bit mask for the pin
#define PIN_MASK(pin) (0x1UL << (pin & 0x1FU))

#ifdef  __cplusplus
}
#endif

#endif /* PIO_CONFIG_H_ */
