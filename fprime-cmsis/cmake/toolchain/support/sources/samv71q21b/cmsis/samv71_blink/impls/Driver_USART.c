/*
 * Copyright (c) 2013-2020 Arm Limited. All rights reserved.
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

#include "Driver_USART.h"
#include <stdint.h>
#include <string.h>
#include "configuration.h"
#include "driver/usart/drv_usart.h"
#include "hal_defs.h"
#include "peripheral/usart/plib_usart1.h"
#include "system/system_module.h"

#define ARM_USART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {ARM_USART_API_VERSION, ARM_USART_DRV_VERSION};

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
    0  /* Reserved (must be zero) */
};

//
//   Functions
//

static ARM_DRIVER_VERSION ARM_USART_GetVersion(void) {
    return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void) {
    return DriverCapabilities;
}

static DRV_USART_CLIENT_OBJ drvUSART0ClientObjPool[DRV_USART_CLIENTS_NUMBER_IDX0];

const DRV_USART_PLIB_INTERFACE drvUsart0PlibAPI = {
    .readCallbackRegister = (DRV_USART_PLIB_READ_CALLBACK_REG)USART1_ReadCallbackRegister,
    .read = (DRV_USART_PLIB_READ)USART1_Read,
    .readIsBusy = (DRV_USART_PLIB_READ_IS_BUSY)USART1_ReadIsBusy,
    .readCountGet = (DRV_USART_PLIB_READ_COUNT_GET)USART1_ReadCountGet,
    .readAbort = (DRV_USART_PLIB_READ_ABORT)USART1_ReadAbort,
    .writeCallbackRegister = (DRV_USART_PLIB_WRITE_CALLBACK_REG)USART1_WriteCallbackRegister,
    .write = (DRV_USART_PLIB_WRITE)USART1_Write,
    .writeIsBusy = (DRV_USART_PLIB_WRITE_IS_BUSY)USART1_WriteIsBusy,
    .writeCountGet = (DRV_USART_PLIB_WRITE_COUNT_GET)USART1_WriteCountGet,
    .errorGet = (DRV_USART_PLIB_ERROR_GET)USART1_ErrorGet,
    .serialSetup = (DRV_USART_PLIB_SERIAL_SETUP)USART1_SerialSetup};

const uint32_t drvUsart0remapDataWidth[] = {0x0, 0x40, 0x80, 0xC0, 0x20000};
const uint32_t drvUsart0remapParity[] = {0x800, 0x0, 0x200, 0x600, 0x400, 0xC00};
const uint32_t drvUsart0remapStopBits[] = {0x0, 0x1000, 0x2000};
const uint32_t drvUsart0remapError[] = {0x20, 0x80, 0x40};

const DRV_USART_INIT drvUsart0InitData = {
    .usartPlib = &drvUsart0PlibAPI,

    /* USART Number of clients */
    .numClients = DRV_USART_CLIENTS_NUMBER_IDX0,

    /* USART Client Objects Pool */
    .clientObjPool = (uintptr_t)&drvUSART0ClientObjPool[0],

    .remapDataWidth = drvUsart0remapDataWidth,

    .remapParity = drvUsart0remapParity,

    .remapStopBits = drvUsart0remapStopBits,

    .remapError = drvUsart0remapError,

    .dataWidth = DRV_USART_DATA_8_BIT,
};

typedef struct {
    SYS_MODULE_OBJ sysObj;
    DRV_HANDLE handle;
    ARM_USART_SignalEvent_t cb_event;
    uint32_t tx_count;
    uint32_t rx_count;
    bool initialized;
    bool powered;
    ARM_USART_STATUS status;
    ARM_USART_MODEM_STATUS modem_status;
    DRV_USART_CLIENT_OBJ* clientObj;
} impl_t;

static impl_t ME;

#include <stddef.h>
#include <string.h>
#include "Driver_USART.h"
#include "sam.h"  // Contains USART1 register definitions

// USART Ring Buffers
#define USART1_TX_BUFFER_SIZE 128
#define USART1_RX_BUFFER_SIZE 128

static uint8_t txBuffer[USART1_TX_BUFFER_SIZE];
static uint8_t rxBuffer[USART1_RX_BUFFER_SIZE];
static size_t txCount = 0;
static size_t rxCount = 0;
static ARM_USART_SignalEvent_t USART1_Callback;

// USART Driver Control Block
static ARM_USART_STATUS usart1_status;
static bool usart1_initialized = false, usart1_powered = false;
#define USART1 USART1_REGS

// USART1 IRQ Handler
void USART1_Handler(void) {
    uint32_t status = USART1->US_CSR;

    // Check if RX ready
    if (status & US_CSR_RXRDY_Msk) {
        uint8_t rxd = USART1->US_RHR & US_RHR_RXCHR_Msk;
        if (rxCount < USART1_RX_BUFFER_SIZE) {
            rxBuffer[rxCount++] = rxd;
        }
        if (USART1_Callback)
            USART1_Callback(ARM_USART_EVENT_RECEIVE_COMPLETE);
    }

    // Check if TX ready
    if (status & US_CSR_TXRDY_Msk && txCount > 0) {
        USART1->US_THR = txBuffer[--txCount];
        if (txCount == 0) {
            USART1->US_IDR = US_IDR_TXRDY_Msk;  // Disable TX interrupt
            if (USART1_Callback)
                USART1_Callback(ARM_USART_EVENT_SEND_COMPLETE);
        }
    }
}

/// Initialize USART
static int32_t USART1_Initialize(ARM_USART_SignalEvent_t cb_event) {
    if (usart1_initialized)
        return ARM_DRIVER_OK;

    // Store callback
    USART1_Callback = cb_event;

    // Enable peripheral clock
    PMC->PMC_PCER0 |= (1 << ID_USART1);

    // Reset USART1 and enable TX/RX
    USART1->US_CR = US_CR_RSTRX_Msk | US_CR_RSTTX_Msk | US_CR_RSTSTA_Msk;
    USART1->US_CR = US_CR_TXEN_Msk | US_CR_RXEN_Msk;

    usart1_initialized = true;
    return ARM_DRIVER_OK;
}

/// Power Control
static int32_t USART1_PowerControl(ARM_POWER_STATE state) {
    if (!usart1_initialized)
        return ARM_DRIVER_ERROR;

    if (state == ARM_POWER_FULL) {
        usart1_powered = true;
        PMC->PMC_PCER0 |= (1 << ID_USART1);
    } else if (state == ARM_POWER_OFF) {
        usart1_powered = false;
        PMC->PMC_PCDR0 |= (1 << ID_USART1);
    }

    return ARM_DRIVER_OK;
}

/// Set USART Configuration
static int32_t USART1_Control(uint32_t control, uint32_t arg) {
    if (!usart1_initialized)
        return ARM_DRIVER_ERROR;

    uint32_t mode = US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT;

    if (control & ARM_USART_PARITY_NONE)
        mode |= US_MR_PAR_NO;
    if (control & ARM_USART_PARITY_ODD)
        mode |= US_MR_PAR_ODD;
    if (control & ARM_USART_PARITY_EVEN)
        mode |= US_MR_PAR_EVEN;

    if (control & ARM_USART_STOP_BITS_2)
        mode |= US_MR_NBSTOP_2_BIT;

    USART1->US_MR = mode;
    USART1->US_BRGR = SystemCoreClock / (16 * arg);

    return ARM_DRIVER_OK;
}

/// Send Data
static int32_t USART1_Send(const void* data, uint32_t num) {
    if (!usart1_initialized || txCount > 0)
        return ARM_DRIVER_ERROR_BUSY;

    memcpy(txBuffer, data, num);
    txCount = num;

    // Enable TX interrupt
    USART1->US_IER = US_IER_TXRDY_Msk;
    return ARM_DRIVER_OK;
}

/// Receive Data
static int32_t USART1_Receive(void* data, uint32_t num) {
    if (!usart1_initialized)
        return ARM_DRIVER_ERROR;

    size_t count = 0;
    while (count < num && rxCount > 0) {
        ((uint8_t*)data)[count++] = rxBuffer[--rxCount];
    }

    return count;
}

/// Get TX Count
static uint32_t USART1_GetTxCount(void) {
    return USART1_TX_BUFFER_SIZE - txCount;
}

/// Get RX Count
static uint32_t USART1_GetRxCount(void) {
    return rxCount;
}

/// Get Status
static ARM_USART_STATUS USART1_GetStatus(void) {
    return usart1_status;
}

/// CMSIS USART Driver Instance
ARM_DRIVER_USART Driver_USART1 = {.GetVersion = NULL,
                                  .GetCapabilities = NULL,
                                  .Initialize = USART1_Initialize,
                                  .PowerControl = USART1_PowerControl,
                                  .Send = USART1_Send,
                                  .Receive = USART1_Receive,
                                  .GetTxCount = USART1_GetTxCount,
                                  .GetRxCount = USART1_GetRxCount,
                                  .Control = USART1_Control,
                                  .GetStatus = USART1_GetStatus};
