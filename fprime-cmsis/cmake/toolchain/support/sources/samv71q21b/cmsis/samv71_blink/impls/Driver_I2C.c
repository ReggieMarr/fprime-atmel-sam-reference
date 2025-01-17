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

#include "Driver_I2C.h"
#include <stdint.h>
// Harmony includes
#include "driver/i2c/drv_i2c.h"
#include "system/system_module.h"
#include "configuration.h"
#include "definitions.h"
#include "device.h"

#include "hal_defs.h"

#define ARM_I2C_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */
/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_I2C_API_VERSION,
    ARM_I2C_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_I2C_CAPABILITIES DriverCapabilities = {
    0  /* supports 10-bit addressing */
};

/* Instance Impl private data */
// TODO this should be inferred using ARM_DRIVER_I2C or similar
#define I2C_INSTANCE 0
static SYS_MODULE_INDEX drvIndex = I2C_INSTANCE;
static SYS_MODULE_INIT modInit = {.value = I2C_INSTANCE};

// Add these to your impl_t structure
typedef struct impl_s {
    SYS_MODULE_OBJ sysObj;
    DRV_HANDLE handle;
    ARM_I2C_SignalEvent_t cb_event;
    DRV_I2C_TRANSFER_SETUP transfer_setup; // Transfer setup configuration
} impl_t;

static impl_t ME;

// Default signal event handler
void __attribute__((weak)) ARM_I2C_SignalEvent(uint32_t event)
{
    switch(event)
    {
        case DRV_I2C_TRANSFER_EVENT_PENDING:
            // Signal ARM_I2C_EVENT_TRANSFER_INCOMPLETE
            break;

        case DRV_I2C_TRANSFER_EVENT_COMPLETE:
            // Signal ARM_I2C_EVENT_TRANSFER_DONE
            break;

        case DRV_I2C_TRANSFER_EVENT_HANDLE_EXPIRED:
        case DRV_I2C_TRANSFER_EVENT_HANDLE_INVALID:
        case DRV_I2C_TRANSFER_EVENT_ERROR:
        default:
            // Signal appropriate error event
            // (ARM_I2C_EVENT_ADDRESS_NACK, ARM_I2C_EVENT_BUS_ERROR, etc.)
            break;

    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************

/* I2C Client Objects Pool */
static DRV_I2C_CLIENT_OBJ drvI2C0ClientObjPool[DRV_I2C_CLIENTS_NUMBER_IDX0];

/* I2C PLib Interface Initialization */
const DRV_I2C_PLIB_INTERFACE drvI2C0PLibAPI = {

    /* I2C PLib Transfer Read Add function */
    .read = (DRV_I2C_PLIB_READ)TWIHS0_Read,

    /* I2C PLib Transfer Write Add function */
    .write = (DRV_I2C_PLIB_WRITE)TWIHS0_Write,


    /* I2C PLib Transfer Write Read Add function */
    .writeRead = (DRV_I2C_PLIB_WRITE_READ)TWIHS0_WriteRead,

    /*I2C PLib Transfer Abort function */
    .transferAbort = (DRV_I2C_PLIB_TRANSFER_ABORT)TWIHS0_TransferAbort,

    /* I2C PLib Transfer Status function */
    .errorGet = (DRV_I2C_PLIB_ERROR_GET)TWIHS0_ErrorGet,

    /* I2C PLib Transfer Setup function */
    .transferSetup = (DRV_I2C_PLIB_TRANSFER_SETUP)TWIHS0_TransferSetup,

    /* I2C PLib Callback Register */
    .callbackRegister = (DRV_I2C_PLIB_CALLBACK_REGISTER)TWIHS0_CallbackRegister,
};


/* I2C Driver Initialization Data */
const DRV_I2C_INIT drvI2C0InitData =
{
    /* I2C PLib API */
    .i2cPlib = &drvI2C0PLibAPI,

    /* I2C Number of clients */
    .numClients = DRV_I2C_CLIENTS_NUMBER_IDX0,

    /* I2C Client Objects Pool */
    .clientObjPool = (uintptr_t)&drvI2C0ClientObjPool[0],

    /* I2C Clock Speed */
    .clockSpeed = DRV_I2C_CLOCK_SPEED_IDX0,
};

//
//  Functions
//

static ARM_DRIVER_VERSION ARM_I2C_GetVersion(void)
{
  return DriverVersion;
}

static ARM_I2C_CAPABILITIES ARM_I2C_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ARM_I2C_Initialize(ARM_I2C_SignalEvent_t cb_event)
{
    ME.sysObj = DRV_I2C_Initialize(DRV_I2C_INDEX_0, (SYS_MODULE_INIT *)&drvI2C0InitData);

    CHECK(ME.sysObj != SYS_MODULE_OBJ_INVALID, return ARM_DRIVER_ERROR);

    ME.handle = DRV_I2C_Open(drvIndex, DRV_IO_INTENT_EXCLUSIVE);
    CHECK(ME.handle != DRV_HANDLE_INVALID, return ARM_DRIVER_ERROR);

    ME.cb_event = cb_event;

    return ARM_DRIVER_OK;
}

static int32_t ARM_I2C_Uninitialize(void)
{
    DRV_I2C_Close(ME.handle);
    ME.cb_event = NULL;

    return ARM_DRIVER_OK;
}

static int32_t ARM_I2C_PowerControl(ARM_POWER_STATE state)
{
    return ARM_DRIVER_OK;
    switch (state)
    {
    case ARM_POWER_OFF:
        ARM_I2C_Uninitialize();
        break;
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_POWER_FULL:
        CHECK(ARM_I2C_Initialize(ARM_I2C_SignalEvent) == ARM_DRIVER_OK, return ret);
        break;
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_I2C_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    bool success;
    // Convert 7-bit/10-bit address format
    addr = (uint16_t)(addr & ~ARM_I2C_ADDRESS_10BIT);

    // Call Harmony driver write function
    success = DRV_I2C_WriteTransfer(
        ME.handle,
        addr,
        (void*)data,
        (size_t)num
    );

    return (success) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

static int32_t ARM_I2C_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    bool success;
    // Convert 7-bit/10-bit address format
    addr = (uint16_t)(addr & ~ARM_I2C_ADDRESS_10BIT);

    // Call Harmony driver read function
    success = DRV_I2C_ReadTransfer(
        ME.handle,
        addr,
        (void*)data,
        (size_t)num
    );

    return (success) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

static int32_t ARM_I2C_SlaveTransmit(const uint8_t *data, uint32_t num)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static int32_t ARM_I2C_SlaveReceive(uint8_t *data, uint32_t num)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static int32_t ARM_I2C_GetDataCount(void)
{
    // You'll need to maintain a counter for transferred data
    // This could be implemented by tracking the transfer status and bytes transferred
    // through the Harmony driver callbacks

    static int32_t transferredBytes = 0;
    return transferredBytes;
}

static int32_t ARM_I2C_Control(uint32_t control, uint32_t arg)
{
    DRV_I2C_TRANSFER_SETUP setup = { 0 };

    if (ME.handle == DRV_HANDLE_INVALID) {
        return ARM_DRIVER_ERROR;
    }

    switch (control)
    {
    case ARM_I2C_BUS_SPEED:
        {
            // Configure transfer setup
            setup.clockSpeed = 0; // Initialize to 0

            switch (arg)
            {
            case ARM_I2C_BUS_SPEED_STANDARD:
                setup.clockSpeed = 100000; // 100 kHz
                break;

            case ARM_I2C_BUS_SPEED_FAST:
                setup.clockSpeed = 400000; // 400 kHz
                break;

            case ARM_I2C_BUS_SPEED_FAST_PLUS:
                setup.clockSpeed = 1000000; // 1 MHz
                break;

            case ARM_I2C_BUS_SPEED_HIGH:
                setup.clockSpeed = 3400000; // 3.4 MHz
                break;

            default:
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            // Store the transfer setup
            ME.transfer_setup = setup;

            // Apply the transfer setup
            if (!DRV_I2C_TransferSetup(ME.handle, &setup)) {
                return ARM_DRIVER_ERROR;
            }

            return ARM_DRIVER_OK;
        }

    case ARM_I2C_OWN_ADDRESS:
    case ARM_I2C_BUS_CLEAR:
    case ARM_I2C_ABORT_TRANSFER:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static ARM_I2C_STATUS ARM_I2C_GetStatus(void)
{
    /* SYS_STATUS sysStatus = DRV_I2C_Status(ME.sysMod); */
    ARM_I2C_STATUS armStatus = {};
    // NOTE unsupported
    return armStatus;
}

// End I2C Interface

extern \
ARM_DRIVER_I2C Driver_I2C0;
ARM_DRIVER_I2C Driver_I2C0 = {
    ARM_I2C_GetVersion,
    ARM_I2C_GetCapabilities,
    ARM_I2C_Initialize,
    ARM_I2C_Uninitialize,
    ARM_I2C_PowerControl,
    ARM_I2C_MasterTransmit,
    ARM_I2C_MasterReceive,
    ARM_I2C_SlaveTransmit,
    ARM_I2C_SlaveReceive,
    ARM_I2C_GetDataCount,
    ARM_I2C_Control,
    ARM_I2C_GetStatus
};
