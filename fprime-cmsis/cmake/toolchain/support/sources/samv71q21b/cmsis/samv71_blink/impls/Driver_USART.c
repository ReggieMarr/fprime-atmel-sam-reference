#include "Driver_USART.h"
#include <string.h>
#include "sam.h"

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

typedef struct {
    ARM_USART_SignalEvent_t cb_event;
    uint32_t tx_count;
    uint32_t rx_count;
    bool initialized;
    bool powered;
    ARM_USART_STATUS status;
    ARM_USART_MODEM_STATUS modem_status;
} impl_t;

static impl_t ME;

// USART Ring Buffer Definitions
#define USART1_TX_BUFFER_SIZE 128
#define USART1_RX_BUFFER_SIZE 128

static uint8_t txBuffer[USART1_TX_BUFFER_SIZE];
static uint8_t rxBuffer[USART1_RX_BUFFER_SIZE];
static volatile size_t txCount = 0;
static volatile size_t rxCount = 0;
static ARM_USART_SignalEvent_t USART1_Callback = NULL;

// USART Driver Instance
typedef struct {
    bool initialized;
    bool powered;
    ARM_USART_STATUS status;
    ARM_USART_MODEM_STATUS modem_status;
} USART1_CONTROL_BLOCK;

static USART1_CONTROL_BLOCK usart1_cb = {false, false, {0}, {0}};

#define USART1 USART1_REGS
#define PMC PMC_REGS

static void USART1_ErrorClear(void) {
    uint8_t dummyData = 0u;

    if (USART1_REGS->US_CSR & (US_CSR_USART_OVRE_Msk | US_CSR_USART_PARE_Msk | US_CSR_USART_FRAME_Msk)) {
        /* Clear the error flags */
        USART1_REGS->US_CR = US_CR_USART_RSTSTA_Msk;

        /* Flush existing error bytes from the RX FIFO */
        while (USART1_REGS->US_CSR & US_CSR_USART_RXRDY_Msk) {
            dummyData = (USART1_REGS->US_RHR & US_RHR_RXCHR_Msk);
        }
    }

    /* Ignore the warning */
    (void)dummyData;
}

// USART1 IRQ Handler: Handles TX and RX Interrupts
void USART1_InterruptHandler(void) {
    uint32_t status = USART1->US_CSR;

    /* Error status */
    uint32_t errorStatus =
        (USART1_REGS->US_CSR & (US_CSR_USART_OVRE_Msk | US_CSR_USART_FRAME_Msk | US_CSR_USART_PARE_Msk));

    if (errorStatus != 0) {
        /* Save the error to be reported later */
        /* usart1Obj.errorStatus = (USART_ERROR)errorStatus; */

        /* Clear error flags and flush the error data */
        USART1_ErrorClear();

        /* Disable Read, Overrun, Parity and Framing error interrupts */
        USART1_REGS->US_IDR =
            (US_IDR_USART_RXRDY_Msk | US_IDR_USART_FRAME_Msk | US_IDR_USART_PARE_Msk | US_IDR_USART_OVRE_Msk);

        /* usart1Obj.rxBusyStatus = false; */

        /* USART errors are normally associated with the receiver, hence calling
         * receiver callback */
        /* if (usart1Obj.rxCallback != NULL) { */
        /*     usart1Obj.rxCallback(usart1Obj.rxContext); */
        /* } */
    }

    if (status & US_CSR_USART_RXRDY_Msk) {
        uint8_t rxd = USART1->US_RHR & US_RHR_RXCHR_Msk;
        if (rxCount < USART1_RX_BUFFER_SIZE) {
            rxBuffer[rxCount++] = rxd;
        }
        if (USART1_Callback)
            USART1_Callback(ARM_USART_EVENT_RECEIVE_COMPLETE);
    }

    if (status & US_CSR_USART_TXRDY_Msk && txCount > 0) {
        USART1->US_THR = txBuffer[--txCount];
        if (txCount == 0) {
            USART1->US_IDR = US_IDR_USART_TXRDY_Msk;  // Disable TX interrupt
            if (USART1_Callback)
                USART1_Callback(ARM_USART_EVENT_SEND_COMPLETE);
        }
    }
}

/// Initialize USART
static int32_t USART1_Initialize(ARM_USART_SignalEvent_t cb_event) {
    if (usart1_cb.initialized)
        return ARM_DRIVER_OK;

    USART1_Callback = cb_event;

    PMC->PMC_PCER0 |= (1 << ID_USART1);  // Enable Peripheral Clock

    USART1->US_CR = US_CR_USART_RSTRX_Msk | US_CR_USART_RSTTX_Msk | US_CR_USART_RSTSTA_Msk;
    USART1->US_CR = US_CR_USART_TXEN_Msk | US_CR_USART_RXEN_Msk;

    usart1_cb.initialized = true;
    return ARM_DRIVER_OK;
}

/// Uninitialize USART
static int32_t USART1_Uninitialize(void) {
    if (!usart1_cb.initialized)
        return ARM_DRIVER_OK;
    if (usart1_cb.powered)
        return ARM_DRIVER_ERROR;

    USART1_Callback = NULL;
    usart1_cb.initialized = false;
    return ARM_DRIVER_OK;
}

/// Power Control
static int32_t USART1_PowerControl(ARM_POWER_STATE state) {
    if (!usart1_cb.initialized)
        return ARM_DRIVER_ERROR;

    if (state == ARM_POWER_FULL) {
        usart1_cb.powered = true;
        PMC->PMC_PCER0 |= (1 << ID_USART1);
    } else if (state == ARM_POWER_OFF) {
        usart1_cb.powered = false;
        PMC->PMC_PCDR0 |= (1 << ID_USART1);
    }

    return ARM_DRIVER_OK;
}

/// Set USART Configuration
static int32_t USART1_Control(uint32_t control, uint32_t arg) {
    if (!usart1_cb.initialized)
        return ARM_DRIVER_ERROR;

    uint32_t mode = US_MR_USART_CHRL_8_BIT | US_MR_USART_PAR_NO | US_MR_USART_NBSTOP_1_BIT;

    if (control & ARM_USART_PARITY_NONE)
        mode |= US_MR_USART_PAR_NO;
    if (control & ARM_USART_PARITY_ODD)
        mode |= US_MR_USART_PAR_ODD;
    if (control & ARM_USART_PARITY_EVEN)
        mode |= US_MR_USART_PAR_EVEN;

    if (control & ARM_USART_STOP_BITS_2)
        mode |= US_MR_USART_NBSTOP_2_BIT;

    USART1->US_MR = mode;
    USART1->US_BRGR = SystemCoreClock / (16 * arg);

    return ARM_DRIVER_OK;
}

/// Send Data
static int32_t USART1_Send(const void* data, uint32_t num) {
    if (!usart1_cb.initialized || txCount > 0)
        return ARM_DRIVER_ERROR_BUSY;

    memcpy(txBuffer, data, num);
    txCount = num;

    USART1->US_IER = US_IER_USART_TXRDY_Msk;  // Enable TX Interrupt
    return ARM_DRIVER_OK;
}

/// Receive Data
static int32_t USART1_Receive(void* data, uint32_t num) {
    if (!usart1_cb.initialized)
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

/// Abort Send or Receive
static int32_t USART1_Transfer(const void* data_out, void* data_in, uint32_t num) {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/// Get USART Status
static ARM_USART_STATUS USART1_GetStatus(void) {
    return usart1_cb.status;
}

/// Get Modem Status
static ARM_USART_MODEM_STATUS USART1_GetModemStatus(void) {
    return usart1_cb.modem_status;
}

/// Set Modem Control
static int32_t USART1_SetModemControl(ARM_USART_MODEM_CONTROL control) {
    return 0;
}

/// CMSIS USART Driver Instance
ARM_DRIVER_USART Driver_USART1 = {.GetVersion = NULL,
                                  .GetCapabilities = NULL,
                                  .Initialize = USART1_Initialize,
                                  .Uninitialize = USART1_Uninitialize,
                                  .PowerControl = USART1_PowerControl,
                                  .Send = USART1_Send,
                                  .Receive = USART1_Receive,
                                  .Transfer = USART1_Transfer,
                                  .GetTxCount = USART1_GetTxCount,
                                  .GetRxCount = USART1_GetRxCount,
                                  .Control = USART1_Control,
                                  .GetStatus = USART1_GetStatus,
                                  .GetModemStatus = USART1_GetModemStatus,
                                  .SetModemControl = USART1_SetModemControl};
