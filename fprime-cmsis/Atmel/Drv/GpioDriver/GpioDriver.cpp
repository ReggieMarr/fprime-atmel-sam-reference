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

bool GpioDriver::open(NATIVE_INT_TYPE gpioPin, GpioDirection direction) {
    // Store GPIO number
    this->m_pin = gpioPin;

    // Enable peripheral clock for the corresponding PIO controller
    this->m_gpio->Setup(this->m_pin);

    // Configure the pin
    if (direction == IN) {
        // Configure as input
        // this->m_gpio->PIO_ODR = (1u << this->m_pin);  // Disable output
        // this->m_gpio->PIO_PER = (1u << this->m_pin);  // Enable PIO control
        this->m_gpio->SetDirection(this->m_pin, ARM_GPIO_DIRECTION::ARM_GPIO_INPUT);
        // this->m_gpio->SetDirection(this->m_pin, ARM_GPIO_INPUT);
        // Enable PIO control (as opposed to peripheral control)
    } else {
        // Configure as output
        // this->m_port->PIO_OER = (1u << this->m_pin);  // Enable output
        // this->m_port->PIO_PER = (1u << this->m_pin);  // Enable PIO control
        this->m_gpio->SetDirection(this->m_pin, ARM_GPIO_DIRECTION::ARM_GPIO_OUTPUT);
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

}  // namespace Atmel

#endif // SAMV71_GPIO_DRIVER_HPP
