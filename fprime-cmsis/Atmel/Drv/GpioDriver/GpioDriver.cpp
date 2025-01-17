// ======================================================================
// \title  GpioDriverImpl.cpp
// \author ReggieMarr
// \brief  cpp file for GpioDriver component implementation class
// ======================================================================

// GpioDriver.hpp
#ifndef SAMV71_GPIO_DRIVER_HPP
#define SAMV71_GPIO_DRIVER_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Drv/GpioDriverComponentAc.hpp>
#include "GpioDriver.hpp"
#include <FpConfig.hpp>
#include "Driver_GPIO.h"

namespace Atmel {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

GpioDriver::GpioDriver(const char* const compName) :
    GpioDriverComponentBase(compName),
    this->m_gpio(-1),
    this->m_port(nullptr),
    this->m_pin(0),
    this->m_initialized(false)
{
}

void GpioDriver::init(const NATIVE_INT_TYPE instance) {
    GpioDriverComponentBase::init(instance);
}

GpioDriver::~GpioDriver() {
}

bool GpioDriver::open(NATIVE_INT_TYPE gpio, GpioDirection direction) {
    // Store GPIO number
    this->m_gpio = gpio;

    // Convert GPIO number to port and pin
    this->convertGpioNumber(gpio, &this->m_port, this->m_pin);

    if (this->m_port == nullptr) {
        return false;
    }

    // Enable peripheral clock for the corresponding PIO controller
    if (this->m_port == PIOA) {
        PMC->PMC_PCER0 |= PMC_PCER0_PID10;  // ID_PIOA = 10
    } else if (this->m_port == PIOB) {
        PMC->PMC_PCER0 |= PMC_PCER0_PID11;  // ID_PIOB = 11
    } else if (this->m_port == PIOC) {
        PMC->PMC_PCER0 |= PMC_PCER0_PID12;  // ID_PIOC = 12
    } else if (this->m_port == PIOD) {
        PMC->PMC_PCER0 |= PMC_PCER0_PID16;  // ID_PIOD = 16
    } else if (this->m_port == PIOE) {
        PMC->PMC_PCER0 |= PMC_PCER0_PID17;  // ID_PIOE = 17
    }

    // Configure the pin
    if (direction == IN) {
        // Configure as input
        this->m_port->PIO_ODR = (1u << this->m_pin);  // Disable output
        this->m_port->PIO_PER = (1u << this->m_pin);  // Enable PIO control
    } else {
        // Configure as output
        this->m_port->PIO_OER = (1u << this->m_pin);  // Enable output
        this->m_port->PIO_PER = (1u << this->m_pin);  // Enable PIO control
    }

    this->m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------
// Handler implementations
// ----------------------------------------------------------------------

Drv::GpioStatus GpioDriver::gpioRead_handler(
    const NATIVE_INT_TYPE portNum,
    Fw::Logic& state
) {
    if (!this->m_initialized) {
        return Drv::GpioStatus::GPIO_UNINIT;
    }

    // Read pin state
    U32 pinState = (this->m_port->PIO_PDSR & (1u << this->m_pin));
    state = (pinState) ? Fw::Logic::HIGH : Fw::Logic::LOW;

    return Drv::GpioStatus::GPIO_OK;
}

Drv::GpioStatus GpioDriver::gpioWrite_handler(
    const NATIVE_INT_TYPE portNum,
    const Fw::Logic& state
) {
    if (!this->m_initialized) {
        return Drv::GpioStatus::GPIO_UNINIT;
    }

    // Write pin state
    if (state == Fw::Logic::HIGH) {
        this->m_port->PIO_SODR = (1u << this->m_pin);  // Set output data
    } else {
        this->m_port->PIO_CODR = (1u << this->m_pin);  // Clear output data
    }

    return Drv::GpioStatus::GPIO_OK;
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void GpioDriver::convertGpioNumber(
    NATIVE_INT_TYPE gpio,
    Pio** port,
    U32& pin
) {
    // Convert GPIO number to port and pin
    // Assuming mapping: PA0-PA31 = 0-31, PB0-PB31 = 32-63, etc.
    NATIVE_INT_TYPE portNum = gpio / 32;
    pin = gpio % 32;

    switch (portNum) {
        case 0:
            *port = PIOA;
            break;
        case 1:
            *port = PIOB;
            break;
        case 2:
            *port = PIOC;
            break;
        case 3:
            *port = PIOD;
            break;
        case 4:
            *port = PIOE;
            break;
        default:
            *port = nullptr;
            break;
    }
}

}  // namespace Atmel

#endif // SAMV71_GPIO_DRIVER_HPP
