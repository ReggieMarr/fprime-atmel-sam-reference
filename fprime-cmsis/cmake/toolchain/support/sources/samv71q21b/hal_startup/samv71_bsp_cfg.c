#include "samv71_bsp_cfg.h"
#include "Driver_GPIO.h"
#include "pioConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

pio_registers_t pio_default_registers[] = {[PORT_A] =
                                               {
                                                   .PIO_PER = ~0x200018,  // Enable PIO control on non-peripheral pins
                                                   .PIO_PDR = 0x200018,   // Disable PIO control on peripheral pins
                                                   .PIO_MDDR = 0xFFFFFFFFU,   // Disable multi-drive (open-drain)
                                                   .PIO_PUDR = 0xFFFFFFFFU,   // Disable all pull-ups
                                                   .PIO_PPDDR = 0xFFFFFFFFU,  // Disable all pull-downs
                                                   .PIO_OWER = PIO_OWER_Msk,  // Enable synchronous data output write
                                                   .PIO_OER = 0x1,            // Enable output mode on pin 0
                                                   .PIO_ODR = ~0x1U,          // Disable output on all other pins
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

#ifdef __cplusplus
}
#endif
