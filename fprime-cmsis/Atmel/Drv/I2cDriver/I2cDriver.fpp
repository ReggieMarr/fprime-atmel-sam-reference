module Atmel {
    @ I2C Driver for Atmel
    passive component I2cDriver {

        guarded input port write: Drv.I2c

        guarded input port read: Drv.I2c

    }
}
