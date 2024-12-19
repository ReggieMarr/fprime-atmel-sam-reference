module Drv {

    port DutyCycle( dutyCycle: U8 ) -> Fw.Success

}

module Atmel {
    @ PWM Driver for Atmel Boards
    passive component PwmDriver {

        @ Set Duty Cycle 0 - 100%
        sync input port setDutyCycle: Drv.DutyCycle

    }
}