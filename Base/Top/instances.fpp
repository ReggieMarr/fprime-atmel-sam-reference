module atemlBase {

  # ----------------------------------------------------------------------
  # Default Constants
  # ----------------------------------------------------------------------
  module Default {
    constant QUEUE_SIZE = 3
    constant STACK_SIZE = 64 * 1024
  }

  # ----------------------------------------------------------------------
  # Active Component Instances
  # ----------------------------------------------------------------------
  instance cmdDisp: Svc.CommandDispatcher base id 0x0100 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 101

  # instance commQueue: Svc.ComQueue base id 0x0200 \
  #     queue size 15 \
  #     stack size Default.STACK_SIZE \
  #     priority 100 \

  instance eventLogger: Svc.ActiveLogger base id 0x0300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 98

  instance tlmSend: Svc.TlmChan base id 0x0400 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 97

  # ----------------------------------------------------------------------
  # Passive Component Instances
  # ----------------------------------------------------------------------
  instance bufferManager: Svc.BufferManager base id 0x0500

  instance rateGroup1: Svc.PassiveRateGroup base id 0x1000
  instance rateGroup2: Svc.PassiveRateGroup base id 0x1100

  instance comDriver: Arduino.StreamDriver base id 0x4000
  instance framer: Svc.Framer base id 0x4100
  instance fatalAdapter: Svc.AssertFatalAdapter base id 0x4200

  instance systemResources: Svc.SystemResources base id 0x4300

  instance systemTime: Arduino.ArduinoTime base id 0x4400
  instance rateGroupDriver: Svc.RateGroupDriver base id 0x4500
  instance staticMemory: Svc.StaticMemory base id 0x4600
  instance textLogger: Svc.PassiveTextLogger base id 0x4700
  instance deframer: Svc.Deframer base id 0x4800

  instance rateDriver: Arduino.HardwareRateDriver base id 0x4A00
  instance gpioDriver: Arduino.GpioDriver base id 0x5000

  instance blinker: Components.LedBlinker base id 0x6000

}
