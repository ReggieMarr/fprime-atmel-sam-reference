module atemlBase {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
    rateGroup2
  }

  topology atemlBase {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance cmdDisp
    # instance commQueue
    instance deframer
    instance eventLogger
    instance fatalAdapter
    # instance fatalHandler
    instance framer
    instance rateDriver
    instance rateGroup1
    instance rateGroup2
    instance rateGroupDriver
    instance staticMemory
    instance systemResources
    instance textLogger
    instance systemTime
    instance tlmSend
    instance bufferManager

    # Arduino specific components
    instance blinker
    instance gpioDriver
    # instance i2cDriver
    instance comDriver ## disable when using radio

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance systemTime

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {
      # Block driver
      rateDriver.CycleOut -> rateGroupDriver.CycleIn

      # Rate group 1
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1.CycleIn
      rateGroup1.RateGroupMemberOut[0] -> comDriver.schedIn #rfm69.run
      rateGroup1.RateGroupMemberOut[1] -> blinker.run

      # Rate group 2
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2.CycleIn
      rateGroup2.RateGroupMemberOut[0] -> tlmSend.Run
      rateGroup2.RateGroupMemberOut[1] -> bufferManager.schedIn
      rateGroup2.RateGroupMemberOut[2] -> systemResources.run
      # rateGroup2.RateGroupMemberOut[2] -> imu.run
    }

    # connections FaultProtection {
    #   eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    # }

    connections SerialComms {

      # Downlink

      tlmSend.PktSend -> framer.comIn
      eventLogger.PktSend -> framer.comIn

      framer.framedAllocate -> bufferManager.bufferGetCallee
      framer.framedOut -> comDriver.$send
      comDriver.deallocate -> bufferManager.bufferSendIn

      # Uplink
      comDriver.$recv ->deframer.framedIn
      comDriver.allocate -> bufferManager.bufferGetCallee

      deframer.framedDeallocate -> bufferManager.bufferSendIn

      deframer.comOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> deframer.cmdResponseIn

      deframer.bufferAllocate -> bufferManager.bufferGetCallee
      deframer.bufferDeallocate -> bufferManager.bufferSendIn
    }

    connections atemlBase {
      # Add here connections to user-defined components
      blinker.gpioSet -> gpioDriver.gpioWrite
    }

  }

}
