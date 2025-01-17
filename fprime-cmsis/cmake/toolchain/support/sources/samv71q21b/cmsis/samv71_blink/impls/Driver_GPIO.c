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
#include "peripheral/pio/plib_pio.h"

// Pin mapping
#define GPIO_MAX_PINS     134U    // PE5 is the highest pin number (133)
#define PIN_IS_AVAILABLE(n) ((n) < GPIO_MAX_PINS && (n) != PIO_PIN_NONE)

// Callback event handler
static ARM_GPIO_SignalEvent_t gpio_cb_event = NULL;

// Setup GPIO Interface
static int32_t GPIO_Setup(ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event)
{
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        // Store callback
        gpio_cb_event = cb_event;

        // The PIO_Initialize() should have been called during system initialization
        // No need to call it here as it would reset all pin configurations
    } else {
        result = ARM_GPIO_ERROR_PIN;
    }
    return result;
}

// Set GPIO Direction
static int32_t GPIO_SetDirection(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction)
{
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        switch (direction) {
            case ARM_GPIO_INPUT:
                PIO_PinInputEnable(pin);
                break;

            case ARM_GPIO_OUTPUT:
                PIO_PinOutputEnable(pin);
                break;

            default:
                result = ARM_DRIVER_ERROR_PARAMETER;
                break;
        }
    } else {
        result = ARM_GPIO_ERROR_PIN;
    }
    return result;
}

// Set GPIO Output Mode
static int32_t GPIO_SetOutputMode(ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode)
{
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        // Note: The existing PIO library doesn't provide direct control over
        // push-pull vs open-drain mode. You might need to modify the PIO library
        // or access registers directly for this functionality.
        result = ARM_DRIVER_ERROR_UNSUPPORTED;
    } else {
        result = ARM_GPIO_ERROR_PIN;
    }
    return result;
}

// Set GPIO Pull Resistor
static int32_t GPIO_SetPullResistor(ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor)
{
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        // Note: The existing PIO library doesn't provide direct control over
        // pull resistors. You might need to modify the PIO library or
        // access registers directly for this functionality.
        result = ARM_DRIVER_ERROR_UNSUPPORTED;
    } else {
        result = ARM_GPIO_ERROR_PIN;
    }
    return result;
}

// Set GPIO Event Trigger
static int32_t GPIO_SetEventTrigger(ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger)
{
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        // Note: The existing PIO library doesn't provide interrupt configuration.
        // You might need to modify the PIO library or access registers directly
        // for this functionality.
        result = ARM_DRIVER_ERROR_UNSUPPORTED;
    } else {
        result = ARM_GPIO_ERROR_PIN;
    }
    return result;
}

// Set GPIO Output Level
static void GPIO_SetOutput(ARM_GPIO_Pin_t pin, uint32_t val)
{
    if (PIN_IS_AVAILABLE(pin)) {
        PIO_PinWrite(pin, val ? true : false);
    }
}

// Get GPIO Input Level
static uint32_t GPIO_GetInput(ARM_GPIO_Pin_t pin)
{
    uint32_t val = 0U;

    if (PIN_IS_AVAILABLE(pin)) {
        val = PIO_PinRead(pin) ? 1U : 0U;
    }
    return val;
}

// GPIO Driver access structure
ARM_DRIVER_GPIO Driver_GPIO0 = {
    GPIO_Setup,
    GPIO_SetDirection,
    GPIO_SetOutputMode,
    GPIO_SetPullResistor,
    GPIO_SetEventTrigger,
    GPIO_SetOutput,
    GPIO_GetInput
};

// Interrupt handlers for each PIO controller
/* void PIOA_Handler(void) { */
/*     uint32_t status = PIOA->PIO_ISR; */
/*     if (gpio_cb_event) { */
/*         // Call callback for each pin that triggered the interrupt */
/*         for (int i = 0; i < 32; i++) { */
/*             if (status & (1UL << i)) { */
/*                 gpio_cb_event(i); */
/*             } */
/*         } */
/*     } */
/* } */

/* void PIOB_Handler(void) { */
/*     uint32_t status = PIOB->PIO_ISR; */
/*     if (gpio_cb_event) { */
/*         for (int i = 0; i < 32; i++) { */
/*             if (status & (1UL << i)) { */
/*                 gpio_cb_event(i + 32); */
/*             } */
/*         } */
/*     } */
/* } */

// Add similar handlers for PIOC, PIOD, and PIOE if needed
