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
#include "driver/usart/drv_usart.h"
#include "peripheral/usart/plib_usart1.h"
#include "system/system_module.h"
#include "hal_defs.h"

#define ARM_USART_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

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

static ARM_DRIVER_VERSION ARM_USART_GetVersion(void)
{
  return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
{
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
    .serialSetup = (DRV_USART_PLIB_SERIAL_SETUP)USART1_SerialSetup
};

const uint32_t drvUsart0remapDataWidth[] = { 0x0, 0x40, 0x80, 0xC0, 0x20000 };
const uint32_t drvUsart0remapParity[] = { 0x800, 0x0, 0x200, 0x600, 0x400, 0xC00 };
const uint32_t drvUsart0remapStopBits[] = { 0x0, 0x1000, 0x2000 };
const uint32_t drvUsart0remapError[] = { 0x20, 0x80, 0x40 };

const DRV_USART_INIT drvUsart0InitData =
{
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

static int32_t ARM_USART_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    if (ME.initialized) {
        return ARM_DRIVER_OK;
    }

    ME.cb_event = cb_event;

    ME.sysObj = DRV_USART_Initialize(DRV_USART_INDEX_0, (SYS_MODULE_INIT *)&drvUsart0InitData);

    if (ME.sysObj == SYS_MODULE_OBJ_INVALID) {
        return ARM_DRIVER_ERROR;
    }

    // Wait for the driver to be ready
    while (DRV_USART_Status(ME.sysObj) != SYS_STATUS_READY) {
        // NOTE Could add timeout here
    }

    ME.handle = DRV_USART_Open(DRV_USART_INDEX_0,
                              DRV_IO_INTENT_READWRITE |
                              DRV_IO_INTENT_NONBLOCKING);
    CHECK(ME.handle == DRV_HANDLE_INVALID, return ARM_DRIVER_ERROR);

    ME.initialized = true;
    ME.powered = false;

    return ARM_DRIVER_OK;
}

static int32_t ARM_USART_Uninitialize(void)
{
    if (!ME.initialized) {
        return ARM_DRIVER_OK;
    }

    // Check if powered
    if (ME.powered) {
        return ARM_DRIVER_ERROR;
    }

    // Close the driver handle
    if (ME.handle != DRV_HANDLE_INVALID) {
        DRV_USART_Close(ME.handle);
        ME.handle = DRV_HANDLE_INVALID;
    }

    ME.cb_event = NULL;

    return ARM_DRIVER_OK;
}

static int32_t ARM_USART_Send(const void *data, uint32_t num)
{
    if (!data || !num) return ARM_DRIVER_ERROR_PARAMETER;
    if (!ME.initialized) return ARM_DRIVER_ERROR;
    if (ME.status.tx_busy) return ARM_DRIVER_ERROR_BUSY;

    ME.status.tx_busy = 1;
    ME.tx_count = 0;

    bool success = DRV_USART_WriteBuffer(
        ME.handle,
        (void*)data,
        num
    );

    if (!success)
    {
        ME.status.tx_busy = 0;
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_USART_Receive(void *data, uint32_t num)
{
    if (!data || !num) return ARM_DRIVER_ERROR_PARAMETER;
    if (!ME.initialized) return ARM_DRIVER_ERROR;
    if (ME.status.rx_busy) return ARM_DRIVER_ERROR_BUSY;

    ME.status.rx_busy = 1;
    ME.rx_count = 0;

    bool success = DRV_USART_ReadBuffer(
        ME.handle,
        data,
        num
    );

    if (!success)
    {
        ME.status.rx_busy = 0;
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_USART_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    // Not supported in this implementation as Harmony doesn't provide direct transfer
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static uint32_t ARM_USART_GetTxCount(void)
{
    return ME.tx_count;
}

static uint32_t ARM_USART_GetRxCount(void)
{
    return ME.rx_count;
}

static int32_t ARM_USART_Control(uint32_t control, uint32_t arg)
{
    if (!ME.initialized) return ARM_DRIVER_ERROR;

    switch (control & ARM_USART_CONTROL_Msk)
    {
        case ARM_USART_MODE_ASYNCHRONOUS:
        {
            DRV_USART_SERIAL_SETUP setup = {
                .baudRate = arg,
                .dataWidth = DRV_USART_DATA_8_BIT,
                .parity = DRV_USART_PARITY_NONE,
                .stopBits = DRV_USART_STOP_1_BIT
            };

            // Apply data bits setting
            switch (control & ARM_USART_DATA_BITS_Msk)
            {
                case ARM_USART_DATA_BITS_5: setup.dataWidth = DRV_USART_DATA_5_BIT; break;
                case ARM_USART_DATA_BITS_6: setup.dataWidth = DRV_USART_DATA_6_BIT; break;
                case ARM_USART_DATA_BITS_7: setup.dataWidth = DRV_USART_DATA_7_BIT; break;
                case ARM_USART_DATA_BITS_8: setup.dataWidth = DRV_USART_DATA_8_BIT; break;
                case ARM_USART_DATA_BITS_9: setup.dataWidth = DRV_USART_DATA_9_BIT; break;
                default: return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            // Apply parity setting
            switch (control & ARM_USART_PARITY_Msk)
            {
                case ARM_USART_PARITY_NONE: setup.parity = DRV_USART_PARITY_NONE; break;
                case ARM_USART_PARITY_EVEN: setup.parity = DRV_USART_PARITY_EVEN; break;
                case ARM_USART_PARITY_ODD:  setup.parity = DRV_USART_PARITY_ODD; break;
                default: return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            // Apply stop bits setting
            switch (control & ARM_USART_STOP_BITS_Msk)
            {
                case ARM_USART_STOP_BITS_1: setup.stopBits = DRV_USART_STOP_1_BIT; break;
                case ARM_USART_STOP_BITS_2: setup.stopBits = DRV_USART_STOP_2_BIT; break;
                default: return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            if (!DRV_USART_SerialSetup(ME.handle, &setup))
            {
                return ARM_DRIVER_ERROR;
            }
            break;
        }

        case ARM_USART_CONTROL_TX:
            // Enable/disable TX
            // Note: Harmony might not support direct TX enable/disable
            break;

        case ARM_USART_CONTROL_RX:
            // Enable/disable RX
            // Note: Harmony might not support direct RX enable/disable
            break;

        case ARM_USART_ABORT_SEND:
            // Abort ongoing TX operation
            ME.status.tx_busy = 0;
            break;

        case ARM_USART_ABORT_RECEIVE:
            // Abort ongoing RX operation
            ME.status.rx_busy = 0;
            break;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_USART_PowerControl(ARM_POWER_STATE state)
{
    if (!ME.initialized) {
        return ARM_DRIVER_ERROR;
    }

    switch (state)
    {
        case ARM_POWER_OFF:
            if (!ME.powered) {
                return ARM_DRIVER_OK;
            }

            // Disable TX and RX
            ARM_USART_Control(ARM_USART_CONTROL_TX, 0);
            ARM_USART_Control(ARM_USART_CONTROL_RX, 0);

            // Abort any ongoing transfers
            if (ME.status.tx_busy) {
                ARM_USART_Control(ARM_USART_ABORT_SEND, 0);
            }
            if (ME.status.rx_busy) {
                ARM_USART_Control(ARM_USART_ABORT_RECEIVE, 0);
            }

            // Clear status
            memset(&ME.status, 0, sizeof(ARM_USART_STATUS));
            ME.powered = false;
            break;

        case ARM_POWER_LOW:
            // Optional: Implement low power mode if supported
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            if (ME.powered) {
                return ARM_DRIVER_OK;
            }

            // Initialize hardware if needed
            // This might involve setting up clocks, pins, etc.

            // Reset counters
            ME.tx_count = 0;
            ME.rx_count = 0;

            // Clear status
            memset(&ME.status, 0, sizeof(ARM_USART_STATUS));

            // Enable TX and RX if needed
            ARM_USART_Control(ARM_USART_CONTROL_TX, 1);
            ARM_USART_Control(ARM_USART_CONTROL_RX, 1);

            ME.powered = true;
            break;

        default:
            return ARM_DRIVER_ERROR_PARAMETER;
    }

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS ARM_USART_GetStatus(void)
{
    return ME.status;
}

static ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus(void)
{
    return ME.modem_status;
}

static int32_t ARM_USART_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    return 0;
}

void __attribute__((weak)) ARM_USART_SignalEvent(uint32_t event)
{
    switch(event)
    {
        case DRV_USART_BUFFER_EVENT_PENDING:
            // Signal ARM_I2C_EVENT_TRANSFER_INCOMPLETE
            break;

        case DRV_USART_BUFFER_EVENT_COMPLETE:
            // Signal ARM_I2C_EVENT_TRANSFER_DONE
            break;

        case DRV_USART_BUFFER_EVENT_HANDLE_EXPIRED:
        case DRV_USART_BUFFER_EVENT_HANDLE_INVALID:
        case DRV_USART_BUFFER_EVENT_ERROR:
        default:
            // Signal appropriate error event
            // (ARM_I2C_EVENT_ADDRESS_NACK, ARM_I2C_EVENT_BUS_ERROR, etc.)
            break;

    }
}

// End USART Interface

extern \
ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART Driver_USART0 = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART_Initialize,
    ARM_USART_Uninitialize,
    ARM_USART_PowerControl,
    ARM_USART_Send,
    ARM_USART_Receive,
    ARM_USART_Transfer,
    ARM_USART_GetTxCount,
    ARM_USART_GetRxCount,
    ARM_USART_Control,
    ARM_USART_GetStatus,
    ARM_USART_SetModemControl,
    ARM_USART_GetModemStatus
};
