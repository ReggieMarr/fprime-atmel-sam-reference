/*
 * Copyright (c) 2023 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Driver_GPIO.h"
#include <stdint.h>
#include "Driver_Common.h"
#include "component/pio.h"
#include "hal_defs.h"
#include "peripheral/pio/plib_pio.h"

// Pin mapping
#define WRITE_PROTECTION_PASSWORD 0x50494F

// Callback event handler
static ARM_GPIO_SignalEvent_t gpio_cb_event = NULL;

static inline bool IS_PIN_AVAILABLE(ARM_GPIO_Pin_t pin) {
    // First we check that the supplied pin is within the range of valid pin enumerations
    bool isPinInValidRange = ((pin) < GPIO_MAX_PINS && (pin) != PIO_PIN_NONE);
    CHECK(isPinInValidRange, return false);

    // Next we determine if the pin is configured as a GPIO
    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;

    CHECK(pioPort->PIO_PER & pin_mask, return false);

    return true;
}

static int32_t SET_WRITE_PROTECTION(ARM_GPIO_Pin_t pin, bool enableWriteProtection)
{
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN)

    uint32_t port = PIN_TO_PORT(pin);
    uint32_t mask = PIO_WPMR_WPKEY_PASSWD;
    if (enableWriteProtection) {
        mask |= PIO_WPMR_WPEN_Msk;
    }
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;
    pioPort->PIO_WPMR = mask;

    return ARM_DRIVER_OK;
}

static int32_t GET_WRITE_PROTECTION_STATUS(ARM_GPIO_Pin_t pin, bool *writeProtectionStatus)
{
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN)
    CHECK(writeProtectionStatus, return ARM_GPIO_ERROR);

    uint32_t port = PIN_TO_PORT(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;
    uint32_t registerVal = pioPort->PIO_WPSR;
    // NOTE bits 8-23 stores the register at which an invalid write attempt has last occured

    // The last bit is all we care about here
    *writeProtectionStatus = registerVal & 0b1;

    return ARM_DRIVER_OK;
}

// Setup GPIO Interface
static int32_t GPIO_Setup(ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event)
{
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN)

    /* NOTE The PIO_Initialize() should have been called during system initialization */
    /* No need to call it here as it would reset all pin configurations, however we */
    /* should check the status */

    gpio_cb_event = cb_event;

    return ARM_DRIVER_OK;
}

static bool GetDirection(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION *direction) {
    CHECK(direction, return ARM_GPIO_ERROR);

    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;
    uint32_t registerVal = pioPort->PIO_OSR;

    *direction = (registerVal & pin_mask) ? ARM_GPIO_OUTPUT : ARM_GPIO_INPUT;
}

// Set GPIO Direction
static int32_t GPIO_SetDirection(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction)
{
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN)
    int32_t result = ARM_DRIVER_OK;

    result = SET_WRITE_PROTECTION(pin, false);
    CHECK(result = ARM_DRIVER_OK, return result);

    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;

    switch (direction) {
        case ARM_GPIO_INPUT:
            pioPort->PIO_ODR = pin_mask;
            break;

        case ARM_GPIO_OUTPUT:
            pioPort->PIO_OER = pin_mask;
            break;

        default:
            result = ARM_DRIVER_ERROR_PARAMETER;
            break;
    }

    CHECK(SET_WRITE_PROTECTION(pin, true) == ARM_DRIVER_OK, return ARM_DRIVER_ERROR);

    ARM_GPIO_DIRECTION directionStatus;
    GetDirection(pin, &directionStatus);
    CHECK(directionStatus == direction, return ARM_DRIVER_ERROR);

    return result;
}

// Set GPIO Output Mode
static int32_t GPIO_SetOutputMode(ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode)
{
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN)

    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;

    ARM_GPIO_DIRECTION directionStatus;
    GetDirection(pin, &directionStatus);
    CHECK(directionStatus == ARM_GPIO_OUTPUT, return ARM_DRIVER_ERROR);

    uint32_t result = SET_WRITE_PROTECTION(pin, false);
    CHECK(result = ARM_DRIVER_OK, return result);

    // Open-drain is controlled by the multi-driver registers
    // so to support open-drain mode we enable it
    switch (mode) {
    case ARM_GPIO_OPEN_DRAIN:
        pioPort->PIO_MDER = pin_mask;
        break;
    case ARM_GPIO_PUSH_PULL:
        pioPort->PIO_MDDR = pin_mask;
        break;
    default:
        result = ARM_DRIVER_ERROR_PARAMETER;
    }

    CHECK(SET_WRITE_PROTECTION(pin, true) == ARM_DRIVER_OK, return ARM_DRIVER_ERROR);

    return result;
}

// Set GPIO Pull Resistor
static int32_t ARM_GPIO_SetPullResistor(ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor)
{
    // Validate inputs
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN);

    // Get port and pin mask
    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t* pioPort = (pio_registers_t*)port;

    uint32_t result = SET_WRITE_PROTECTION(pin, false);
    CHECK(result = ARM_DRIVER_OK, return result);
    bool desiredPusr, desiredPpdr;

    switch (resistor)
    {
        case ARM_GPIO_PULL_UP:
            pioPort->PIO_PUER = pin_mask;   // Enable pull-up
            desiredPpdr = false;
            desiredPusr = true;
            break;

        case ARM_GPIO_PULL_DOWN:
            pioPort->PIO_PPDER = pin_mask;  // Enable pull-down
            desiredPpdr = true;
            desiredPusr = false;
            break;

        case ARM_GPIO_PULL_NONE:
            pioPort->PIO_PPDER = ~pin_mask;  // Disable pull-down
            pioPort->PIO_PUER = ~pin_mask;   // Disable pull-up
            break;

        default:
            result = ARM_DRIVER_ERROR_PARAMETER;
    }

    CHECK(SET_WRITE_PROTECTION(pin, true) == ARM_DRIVER_OK, return ARM_DRIVER_ERROR);

    bool pusr, ppdr;
    pusr = (pioPort->PIO_PUSR & pin_mask) == 1 ? true : false;
    ppdr = (pioPort->PIO_PPDSR & pin_mask) == 1 ? true : false;
    CHECK(pusr == desiredPusr, return ARM_DRIVER_ERROR);
    CHECK(ppdr == desiredPpdr, return ARM_DRIVER_ERROR);

    return ARM_DRIVER_OK;
}

// Set GPIO Event Trigger
static int32_t ARM_GPIO_SetEventTrigger(ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger)
{
    // Validate inputs
    CHECK(IS_PIN_AVAILABLE(pin), return ARM_GPIO_ERROR_PIN);

    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t* pioPort = (pio_registers_t*)port;

    // Disable write protection
    uint32_t result = SET_WRITE_PROTECTION(pin, false);
    CHECK(result = ARM_DRIVER_OK, return result);

    // First, disable any existing interrupt configuration
    pioPort->PIO_IDR = pin_mask;  // Disable interrupt

    if (trigger == ARM_GPIO_TRIGGER_NONE)
    {
        pioPort->PIO_AIMDR = pin_mask;  // Disable Additional Interrupt modes
        // Re-enable write protection
        CHECK(SET_WRITE_PROTECTION(pin, true) == ARM_DRIVER_OK, return ARM_DRIVER_ERROR);
        return ARM_DRIVER_OK;
    }

    // Enable Additional Interrupt modes
    pioPort->PIO_AIMER = pin_mask;
    pioPort->PIO_ESR = pin_mask;    // Enable Edge detection

    switch (trigger)
    {
        case ARM_GPIO_TRIGGER_RISING_EDGE:
            pioPort->PIO_REHLSR = pin_mask; // Select Rising edge
            break;

        case ARM_GPIO_TRIGGER_FALLING_EDGE:
            pioPort->PIO_FELLSR = pin_mask; // Select Falling edge
            break;

        case ARM_GPIO_TRIGGER_EITHER_EDGE:
            // Configure both edges
            pioPort->PIO_REHLSR = pin_mask; // Enable Rising edge
            pioPort->PIO_FELLSR = pin_mask; // Enable Falling edge
            break;

        default:
            // Re-enable write protection
            result = ARM_DRIVER_ERROR_PARAMETER;
    }

    // Enable interrupt if trigger is not NONE
    pioPort->PIO_IER = pin_mask;

    // Re-enable write protection
    CHECK(SET_WRITE_PROTECTION(pin, true) == ARM_DRIVER_OK, return ARM_DRIVER_ERROR);

    return ARM_DRIVER_OK;
}

// Set GPIO Output Level
static void GPIO_SetOutput(ARM_GPIO_Pin_t pin, uint32_t val)
{
    CHECK(IS_PIN_AVAILABLE(pin), return);

    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;

    if (val) {
        pioPort->PIO_SODR = pin_mask;
    }
    else {
        pioPort->PIO_CODR = pin_mask;
    }
    // NOTE alternatively we could use the output data status register however that provides
    // blanket writes to each pin which is less effective in this case
    /* pioPort->PIO_ODSR = (pioPort->PIO_ODSR & (~pin_mask)) | (pin_mask & val); */
}

// Get GPIO Input Level
static uint32_t GPIO_GetInput(ARM_GPIO_Pin_t pin)
{
    CHECK(IS_PIN_AVAILABLE(pin), return);
    uint32_t port = PIN_TO_PORT(pin);
    uint32_t pin_mask = PIN_MASK(pin);
    volatile pio_registers_t *pioPort = (pio_registers_t*)port;

    return (pioPort->PIO_PDSR & pin_mask) ? 1U : 0U;
}

// GPIO Driver access structure
ARM_DRIVER_GPIO Driver_GPIO = {
    GPIO_Setup,
    GPIO_SetDirection,
    GPIO_SetOutputMode,
    GPIO_SetPullResistor,
    GPIO_SetEventTrigger,
    GPIO_SetOutput,
    GPIO_GetInput
};

// Add similar handlers for PIOC, PIOD, and PIOE if needed
