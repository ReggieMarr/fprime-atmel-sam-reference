// ======================================================================
// \title  I2cDriver.cpp
// \author ethanchee
// \brief  cpp file for I2cDriver component implementation class
// ======================================================================


#include <Atmel/Drv/I2cDriver/I2cDriver.hpp>
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"

namespace Atmel {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  I2cDriver ::
    I2cDriver(
        const char *const compName
    ) : I2cDriverComponentBase(compName),
        m_port_pointer(static_cast<POINTER_CAST>(NULL))
  {

  }

  I2cDriver ::
    ~I2cDriver()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  Drv::I2cStatus I2cDriver ::
    read_handler(
        const NATIVE_INT_TYPE portNum,
        U32 addr,
        Fw::Buffer &serBuffer
    )
  {
    // Ensure buffer is not a nullptr
    FW_ASSERT(serBuffer.getData());

    return read_data(addr, serBuffer);
  }

  Drv::I2cStatus I2cDriver ::
    write_handler(
        const NATIVE_INT_TYPE portNum,
        U32 addr,
        Fw::Buffer &serBuffer
    )
  {
    // Ensure buffer is not a nullptr
    FW_ASSERT(serBuffer.getData());
    
    return write_data(addr, serBuffer);
  }

} // end namespace Atmel
