module Atmel {
    @ I2C node driver for Atmel
    passive component I2cNodeDriver {

        output port write: Drv.I2c

        output port read: Drv.I2c
    }
}
