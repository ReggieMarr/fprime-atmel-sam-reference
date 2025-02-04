// ======================================================================
// \title  GpioDriver.hpp
// \author ReggieMarr
// \brief  hpp file for GpioDriver component implementation class
// ======================================================================

#ifndef ATMEL_DRV_GpioDriver_HPP
#define ATMEL_DRV_GpioDriver_HPP

#include <Drv/GpioDriverComponentAc.hpp>
#include "Driver_GPIO.h"
#include "hal_defs.h"
#include "sam.h"  // SAMV71 peripheral access

namespace Atmel {

typedef struct {
    ARM_GPIO_DIRECTION direction;
    ARM_GPIO_PULL_RESISTOR embeddedResistorMode;
    ARM_GPIO_OUTPUT_MODE outputMode;
    ARM_GPIO_EVENT_TRIGGER eventTriggerMode;
} pioConfig_t;

class GpioDriver : public GpioDriverComponentBase {
  public:
    //! Configure GPIO direction
    enum GpioDirection {
        IN,   //!< input
        OUT,  //!< output
    };

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object GpioDriver
    GpioDriver(const char* const compName);

    //! Initialize object GpioDriver
    void init(const NATIVE_INT_TYPE instance = 0);

    //! Destroy object GpioDriver
    ~GpioDriver();

    //! Open GPIO with specified configuration
    bool open(NATIVE_INT_TYPE gpioPin, GpioDirection direction);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for gpioRead
    Drv::GpioStatus gpioRead_handler(FwIndexType portNum,  //!< The port number
                                     Fw::Logic& state      //!< The state of the GPIO
                                     ) override;

    //! Handler implementation for gpioWrite
    Drv::GpioStatus gpioWrite_handler(FwIndexType portNum,    //!< The port number
                                      const Fw::Logic& state  //!< The state to write to the GPIO
                                      ) override;

    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    ARM_DRIVER_GPIO* m_gpio = &DRIVER_GPIO;
    bool m_initialized;  //!< Initialization status
};

}  // namespace Atmel
#endif
