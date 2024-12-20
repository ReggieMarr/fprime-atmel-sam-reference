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
#include "sam.h"

// Pin mapping
#define GPIO_MAX_PINS         64U
#define PIN_IS_AVAILABLE(n)   ((n) < GPIO_MAX_PINS)

// GPIO port and pin calculation
#define GPIO_PORT(n)          ((n) >> 5)    // Divide by 32
#define GPIO_PIN(n)          (1UL << ((n) & 0x1F))

// PIO Controller structure array
static Pio* const pio_controllers[] = {
    PIOA,
    PIOB,
    PIOC,
    PIOD,
    PIOE
};

// Callback event handler
static ARM_GPIO_SignalEvent_t gpio_cb_event = NULL;

// Setup GPIO Interface
static int32_t GPIO_Setup (ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event) {
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        // Enable PIO clock
        PMC->PMC_PCER0 = (1UL << (ID_PIOA + GPIO_PORT(pin)));

        // Store callback
        gpio_cb_event = cb_event;

        // Enable PIO controller
        pio_controllers[GPIO_PORT(pin)]->PIO_PER = GPIO_PIN(pin);
    } else {
        result = ARM_GPIO_ERROR_PIN;
    }
    return result;
}

// Set GPIO Direction
static int32_t GPIO_SetDirection (ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction) {
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        switch (direction) {
            case ARM_GPIO_INPUT:
                pio_controllers[GPIO_PORT(pin)]->PIO_ODR = GPIO_PIN(pin);  // Disable output
                break;

            case ARM_GPIO_OUTPUT:
                pio_controllers[GPIO_PORT(pin)]->PIO_OER = GPIO_PIN(pin);  // Enable output
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
static int32_t GPIO_SetOutputMode (ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode) {
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        switch (mode) {
            case ARM_GPIO_PUSH_PULL:
                pio_controllers[GPIO_PORT(pin)]->PIO_MDER &= ~GPIO_PIN(pin);  // Disable multi-drive
                break;

            case ARM_GPIO_OPEN_DRAIN:
                pio_controllers[GPIO_PORT(pin)]->PIO_MDER |= GPIO_PIN(pin);   // Enable multi-drive
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

// Set GPIO Pull Resistor
static int32_t GPIO_SetPullResistor (ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor) {
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        switch (resistor) {
            case ARM_GPIO_PULL_NONE:
                pio_controllers[GPIO_PORT(pin)]->PIO_PUDR = GPIO_PIN(pin);  // Disable pull-up
                break;

            case ARM_GPIO_PULL_UP:
                pio_controllers[GPIO_PORT(pin)]->PIO_PUER = GPIO_PIN(pin);  // Enable pull-up
                break;

            case ARM_GPIO_PULL_DOWN:
                // SAMV71 doesn't have built-in pull-down resistors
                result = ARM_DRIVER_ERROR_UNSUPPORTED;
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

// Set GPIO Event Trigger
static int32_t GPIO_SetEventTrigger (ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger) {
    int32_t result = ARM_DRIVER_OK;

    if (PIN_IS_AVAILABLE(pin)) {
        switch (trigger) {
            case ARM_GPIO_TRIGGER_NONE:
                pio_controllers[GPIO_PORT(pin)]->PIO_IDR = GPIO_PIN(pin);  // Disable interrupt
                break;

            case ARM_GPIO_TRIGGER_RISING_EDGE:
                pio_controllers[GPIO_PORT(pin)]->PIO_AIMER = GPIO_PIN(pin);   // Enable Additional Interrupt Mode
                pio_controllers[GPIO_PORT(pin)]->PIO_ESR = GPIO_PIN(pin);     // Edge Select Register
                pio_controllers[GPIO_PORT(pin)]->PIO_REHLSR = GPIO_PIN(pin);  // Rising Edge/High Level Select
                pio_controllers[GPIO_PORT(pin)]->PIO_IER = GPIO_PIN(pin);     // Enable interrupt
                break;

            case ARM_GPIO_TRIGGER_FALLING_EDGE:
                pio_controllers[GPIO_PORT(pin)]->PIO_AIMER = GPIO_PIN(pin);   // Enable Additional Interrupt Mode
                pio_controllers[GPIO_PORT(pin)]->PIO_ESR = GPIO_PIN(pin);     // Edge Select Register
                pio_controllers[GPIO_PORT(pin)]->PIO_FELLSR = GPIO_PIN(pin);  // Falling Edge/Low Level Select
                pio_controllers[GPIO_PORT(pin)]->PIO_IER = GPIO_PIN(pin);     // Enable interrupt
                break;

            case ARM_GPIO_TRIGGER_EITHER_EDGE:
                pio_controllers[GPIO_PORT(pin)]->PIO_AIMDR = GPIO_PIN(pin);   // Disable Additional Interrupt Mode
                pio_controllers[GPIO_PORT(pin)]->PIO_IER = GPIO_PIN(pin);     // Enable interrupt
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

// Set GPIO Output Level
static void GPIO_SetOutput (ARM_GPIO_Pin_t pin, uint32_t val) {
    if (PIN_IS_AVAILABLE(pin)) {
        if (val) {
            pio_controllers[GPIO_PORT(pin)]->PIO_SODR = GPIO_PIN(pin);  // Set output high
        } else {
            pio_controllers[GPIO_PORT(pin)]->PIO_CODR = GPIO_PIN(pin);  // Set output low
        }
    }
}

// Get GPIO Input Level
static uint32_t GPIO_GetInput (ARM_GPIO_Pin_t pin) {
    uint32_t val = 0U;

    if (PIN_IS_AVAILABLE(pin)) {
        if (pio_controllers[GPIO_PORT(pin)]->PIO_PDSR & GPIO_PIN(pin)) {
            val = 1U;
        }
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
void PIOA_Handler(void) {
    uint32_t status = PIOA->PIO_ISR;
    if (gpio_cb_event) {
        // Call callback for each pin that triggered the interrupt
        for (int i = 0; i < 32; i++) {
            if (status & (1UL << i)) {
                gpio_cb_event(i);
            }
        }
    }
}

void PIOB_Handler(void) {
    uint32_t status = PIOB->PIO_ISR;
    if (gpio_cb_event) {
        for (int i = 0; i < 32; i++) {
            if (status & (1UL << i)) {
                gpio_cb_event(i + 32);
            }
        }
    }
}

// Add similar handlers for PIOC, PIOD, and PIOE if needed
