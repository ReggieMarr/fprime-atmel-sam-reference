#include "samv71_bsp_cfg.h"
#include <stdint.h>
#include <string.h>
#include "Driver_GPIO.h"
#include "hal_defs.h"
#include "pioConfig.h"
#include "sam.h"
#include "samv71q21b.h"

#ifdef __cplusplus
extern "C" {
#endif

pio_registers_t DEFAULT_PIO_REGISTERS[] = {[PORT_A] =
                                               {
                                                   .PIO_PER = ~0x200018,  // Enable PIO control on non-peripheral pins
                                                   .PIO_PDR = 0x200018,   // Disable PIO control on peripheral pins
                                                   .PIO_MDDR = 0xFFFFFFFFU,    // Disable multi-drive (open-drain)
                                                   .PIO_PUDR = 0xFFFFFFFFU,    // Disable all pull-ups
                                                   .PIO_PPDDR = 0xFFFFFFFFU,   // Disable all pull-downs
                                                   .PIO_OWER = PIO_OWER_Msk,   // Enable synchronous data output write
                                                   .PIO_OER = 0x1 | LED0_PIN,  // Enable output mode on pin 0
                                                   .PIO_ODR = ~0x1U,           // Disable output on all other pins
                                                   .PIO_ODSR = 0x1,    // Set initial output state to HIGH for pin 0
                                                   .PIO_DRIVER = 0x0,  // Default drive strength
                                                   .PIO_ABCDSR[0] = 0x00000000,  // Default to Peripheral A
                                                   .PIO_ABCDSR[1] = 0x00000000   // Default to Peripheral A
                                               },
                                           [PORT_B] =
                                               {
                                                   .PIO_PER = ~0x10,  // Enable PIO control on non-peripheral pins
                                                   .PIO_PDR = 0x10,   // Disable PIO control on peripheral pins
                                                   .PIO_MDDR = 0xFFFFFFFFU,   // Disable multi-drive (open-drain)
                                                   .PIO_PUDR = 0xFFFFFFFFU,   // Disable all pull-ups
                                                   .PIO_PPDDR = 0xFFFFFFFFU,  // Disable all pull-downs
                                                   .PIO_OWER = PIO_OWER_Msk,  // Enable synchronous data output write
                                                   .PIO_OER = 0x0,            // No output pins enabled
                                                   .PIO_ODR = ~0x0U,          // All pins set as input
                                                   .PIO_ODSR = 0x0,           // Initial state is LOW
                                                   .PIO_DRIVER = 0x0,         // Default drive strength
                                                   .PIO_ABCDSR[0] = 0x10,     // Default to Peripheral B for pin 4
                                                   .PIO_ABCDSR[1] = 0x10      // Default to Peripheral B for pin 4
                                               },
                                           [PORT_C] =
                                               {
                                                   .PIO_PER = 0xFFFFFFFFU,       // Enable PIO control on all pins
                                                   .PIO_MDDR = 0xFFFFFFFFU,      // Disable multi-drive (open-drain)
                                                   .PIO_PUDR = 0xFFFFFFFFU,      // Disable all pull-ups
                                                   .PIO_PPDDR = 0xFFFFFFFFU,     // Disable all pull-downs
                                                   .PIO_OWER = PIO_OWER_Msk,     // Enable synchronous data output write
                                                   .PIO_OER = 0x0,               // No output pins enabled
                                                   .PIO_ODR = ~0x0U,             // All pins set as input
                                                   .PIO_ODSR = 0x0,              // Initial state is LOW
                                                   .PIO_DRIVER = 0x0,            // Default drive strength
                                                   .PIO_ABCDSR[0] = 0x00000000,  // Default to Peripheral A
                                                   .PIO_ABCDSR[1] = 0x00000000   // Default to Peripheral A
                                               },
                                           [PORT_D] =
                                               {
                                                   .PIO_PER = 0xFFFFFFFFU,       // Enable PIO control on all pins
                                                   .PIO_MDDR = 0xFFFFFFFFU,      // Disable multi-drive (open-drain)
                                                   .PIO_PUDR = 0xFFFFFFFFU,      // Disable all pull-ups
                                                   .PIO_PPDDR = 0xFFFFFFFFU,     // Disable all pull-downs
                                                   .PIO_OWER = PIO_OWER_Msk,     // Enable synchronous data output write
                                                   .PIO_OER = 0x0,               // No output pins enabled
                                                   .PIO_ODR = ~0x0U,             // All pins set as input
                                                   .PIO_ODSR = 0x0,              // Initial state is LOW
                                                   .PIO_DRIVER = 0x0,            // Default drive strength
                                                   .PIO_ABCDSR[0] = 0x00000000,  // Default to Peripheral A
                                                   .PIO_ABCDSR[1] = 0x00000000   // Default to Peripheral A
                                               },
                                           [PORT_E] = {
                                               .PIO_PER = 0xFFFFFFFFU,       // Enable PIO control on all pins
                                               .PIO_MDDR = 0xFFFFFFFFU,      // Disable multi-drive (open-drain)
                                               .PIO_PUDR = 0xFFFFFFFFU,      // Disable all pull-ups
                                               .PIO_PPDDR = 0xFFFFFFFFU,     // Disable all pull-downs
                                               .PIO_OWER = PIO_OWER_Msk,     // Enable synchronous data output write
                                               .PIO_OER = 0x0,               // No output pins enabled
                                               .PIO_ODR = ~0x0U,             // All pins set as input
                                               .PIO_ODSR = 0x0,              // Initial state is LOW
                                               .PIO_DRIVER = 0x0,            // Default drive strength
                                               .PIO_ABCDSR[0] = 0x00000000,  // Default to Peripheral A
                                               .PIO_ABCDSR[1] = 0x00000000   // Default to Peripheral A
                                           }};

static bool setPioPortConfigVal(volatile pio_registers_t* currentPioCfg, pio_registers_t* newPioCfg) {
    CHECK(newPioCfg, return false);

    // Since the register contains read-only values we explicitly set each value that we can write to
    currentPioCfg->PIO_PER = newPioCfg->PIO_PER;
    currentPioCfg->PIO_PDR = newPioCfg->PIO_PDR;
    currentPioCfg->PIO_OER = newPioCfg->PIO_OER;
    currentPioCfg->PIO_ODR = newPioCfg->PIO_ODR;
    currentPioCfg->PIO_IFER = newPioCfg->PIO_IFER;
    currentPioCfg->PIO_IFDR = newPioCfg->PIO_IFDR;
    currentPioCfg->PIO_SODR = newPioCfg->PIO_SODR;
    currentPioCfg->PIO_CODR = newPioCfg->PIO_CODR;
    currentPioCfg->PIO_ODSR = newPioCfg->PIO_ODSR;
    currentPioCfg->PIO_IER = newPioCfg->PIO_IER;
    currentPioCfg->PIO_IDR = newPioCfg->PIO_IDR;
    currentPioCfg->PIO_MDER = newPioCfg->PIO_MDER;
    currentPioCfg->PIO_MDDR = newPioCfg->PIO_MDDR;
    currentPioCfg->PIO_MDER = newPioCfg->PIO_MDER;
    currentPioCfg->PIO_MDDR = newPioCfg->PIO_MDDR;
    currentPioCfg->PIO_PUER = newPioCfg->PIO_PUER;
    currentPioCfg->PIO_PUDR = newPioCfg->PIO_PUDR;
    currentPioCfg->PIO_ABCDSR[0] = newPioCfg->PIO_ABCDSR[0];
    currentPioCfg->PIO_ABCDSR[1] = newPioCfg->PIO_ABCDSR[1];
    currentPioCfg->PIO_IFSCER = newPioCfg->PIO_IFSCER;
    currentPioCfg->PIO_IFSCDR = newPioCfg->PIO_IFSCDR;
    currentPioCfg->PIO_PPDER = newPioCfg->PIO_PPDER;
    currentPioCfg->PIO_PPDDR = newPioCfg->PIO_PPDDR;
    currentPioCfg->PIO_OWER = newPioCfg->PIO_OWER;
    currentPioCfg->PIO_OWDR = newPioCfg->PIO_OWDR;
    currentPioCfg->PIO_AIMER = newPioCfg->PIO_AIMER;
    currentPioCfg->PIO_AIMDR = newPioCfg->PIO_AIMDR;
    currentPioCfg->PIO_ESR = newPioCfg->PIO_ESR;
    currentPioCfg->PIO_LSR = newPioCfg->PIO_LSR;
    currentPioCfg->PIO_FELLSR = newPioCfg->PIO_FELLSR;
    currentPioCfg->PIO_REHLSR = newPioCfg->PIO_REHLSR;
    currentPioCfg->PIO_WPMR = newPioCfg->PIO_WPMR;
    currentPioCfg->PIO_SCHMITT = newPioCfg->PIO_SCHMITT;
    currentPioCfg->PIO_DRIVER = newPioCfg->PIO_DRIVER;
    currentPioCfg->PIO_PCMR = newPioCfg->PIO_PCMR;
    currentPioCfg->PIO_PCIER = newPioCfg->PIO_PCIER;
    currentPioCfg->PIO_PCIDR = newPioCfg->PIO_PCIDR;

    return true;
}

void applyPinCfgToPortCfg(pioGPIOConfig_t const* cfg, uint32_t const pinMask, pio_registers_t* port) {
    /*** Apply Function Selection ***/
    if (cfg->base.functionalType == PIO_FUNC_GPIO) {
        port->PIO_PER |= pinMask;  // Enable PIO control
        port->PIO_PDR &= ~pinMask;
    } else {
        port->PIO_PER &= ~pinMask;  // Disable PIO control
        port->PIO_PDR |= pinMask;

        // Configure ABCD multiplexer
        if (cfg->base.functionalType == PIO_FUNC_PERIPH_A) {
            port->PIO_ABCDSR[0] &= ~pinMask;
            port->PIO_ABCDSR[1] &= ~pinMask;
        } else if (cfg->base.functionalType == PIO_FUNC_PERIPH_B) {
            port->PIO_ABCDSR[0] |= pinMask;
            port->PIO_ABCDSR[1] &= ~pinMask;
        } else if (cfg->base.functionalType == PIO_FUNC_PERIPH_C) {
            port->PIO_ABCDSR[0] &= ~pinMask;
            port->PIO_ABCDSR[1] |= pinMask;
        } else if (cfg->base.functionalType == PIO_FUNC_PERIPH_D) {
            port->PIO_ABCDSR[0] |= pinMask;
            port->PIO_ABCDSR[1] |= pinMask;
        }
    }

    /*** Apply Direction Configuration ***/
    if (cfg->direction == ARM_GPIO_OUTPUT) {
        port->PIO_OER |= pinMask;
        port->PIO_ODR &= ~pinMask;
    } else {
        port->PIO_ODR |= pinMask;
    }

    /*** Apply Pull Resistors ***/
    if (cfg->base.pullResistor == ARM_GPIO_PULL_UP) {
        port->PIO_PUER |= pinMask;
        port->PIO_PUDR &= ~pinMask;
    } else {
        port->PIO_PUDR |= pinMask;
    }

    if (cfg->base.pullResistor == ARM_GPIO_PULL_DOWN) {
        port->PIO_PPDER |= pinMask;
    } else {
        port->PIO_PPDDR |= pinMask;
    }

    /*** Apply Initial Output State ***/
    if (cfg->direction == ARM_GPIO_OUTPUT) {
        if (cfg->isHighOnStart) {
            port->PIO_SODR |= pinMask;
        } else {
            port->PIO_CODR |= pinMask;
        }
    }

    /*** Apply Open-Drain (Multi-Driver) Mode ***/
    if (cfg->isOpenDrain) {
        port->PIO_MDER |= pinMask;
        port->PIO_MDDR &= ~pinMask;
    } else {
        port->PIO_MDER &= ~pinMask;
        port->PIO_MDDR |= pinMask;
    }

    /*** Apply Interrupt/Event Configuration ***/
    if (cfg->irqConfig.trigger != ARM_GPIO_TRIGGER_NONE) {
        port->PIO_AIMER |= pinMask;  // Enable additional interrupt modes

        if (cfg->irqConfig.trigger == ARM_GPIO_TRIGGER_RISING_EDGE) {
            port->PIO_ESR |= pinMask;
            port->PIO_REHLSR |= pinMask;
        } else if (cfg->irqConfig.trigger == ARM_GPIO_TRIGGER_FALLING_EDGE) {
            port->PIO_ESR |= pinMask;
            port->PIO_FELLSR |= pinMask;
        } else if (cfg->irqConfig.trigger == ARM_GPIO_TRIGGER_EITHER_EDGE) {
            port->PIO_ESR |= pinMask;
            port->PIO_REHLSR |= pinMask;
            port->PIO_FELLSR |= pinMask;
        }
    }

    // Apply glitch filter
    if (cfg->irqConfig.filterEnabled) {
        port->PIO_IFER |= pinMask;
    } else {
        port->PIO_IFDR |= pinMask;
    }
}

void initPio(const pioGPIOConfig_t* configs, uint32_t numConfigs) {
    // Create a local copy of the default register set to modify
    pio_registers_t pio_registers[ARRAY_SIZE(DEFAULT_PIO_REGISTERS)];

    /* Copy default configuration to working buffer */
    for (uint32_t i = 0; i < ARRAY_SIZE(DEFAULT_PIO_REGISTERS); i++) {
        setPioPortConfigVal(&pio_registers[i], &DEFAULT_PIO_REGISTERS[i]);
    }

    /* Clear & Configure System IO Pins */
    MATRIX_REGS->CCFG_SYSIO = 0;
    MATRIX_REGS->CCFG_SYSIO |= (1 << 4);  // Enables PB4 as a regular GPIO instead of a system function
    PMC_REGS->PMC_PCER0 = 0;

    // Apply each pin-specific configuration
    for (uint32_t i = 0; i < numConfigs; i++) {
        const pioGPIOConfig_t* cfg = &configs[i];
        ARM_PORT_ID pinId = cfg->pinId;
        uint32_t portIdx = PORT_TO_IDX(PIN_TO_PORT(pinId));

        uint32_t portId = 0;
        switch (portIdx) {
            case PORT_A:
                portId = ID_PIOA;
                break;
            case PORT_B:
                portId = ID_PIOB;
                break;
            case PORT_C:
                portId = ID_PIOC;
                break;
            case PORT_D:
                portId = ID_PIOD;
                break;
            case PORT_E:
                portId = ID_PIOE;
                break;
            default:
                // NOTE we need to figure some way to notify that this error has occured
                // for now we'll just skip it
                continue;
                break;
        }

        /* Enable Peripheral Clocks for the relevant PIO Controller */
        PMC_REGS->PMC_PCER0 |= (1 << portId);

        // Get pointer to the port configuration
        applyPinCfgToPortCfg(cfg, PIN_MASK(pinId), &pio_registers[portIdx]);
    }

    // Apply changes to hardware
    for (uint32_t i = 0; i < ARRAY_SIZE(DEFAULT_PIO_REGISTERS); i++) {
        // NOTE I forget why this is + 0x200U rather than sizeof(pio_registers_t)
        // TODO make 0x200U a well named constant
        volatile pio_registers_t* pioPort = (pio_registers_t*)(PIOA_BASE_ADDRESS + (0x200U * i));
        setPioPortConfigVal(pioPort, &pio_registers[i]);
    }
}

#ifdef __cplusplus
}
#endif
