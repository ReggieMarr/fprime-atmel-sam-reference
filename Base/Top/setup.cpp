#include "setup.hpp"

/**
 * \brief setup the program
 *
 * NOTE This is based off of an extraction of the Arduino setup() function.
 *
 */
bool setup() {
    // Initialize OSAL
    Os::init();

    // // Setup Serial and Logging
    // Serial.begin(115200);
    // static_cast<Os::Arduino::StreamConsoleHandle*>(Os::Console::getSingleton().getHandle())->setStreamHandler(Serial);

    // delay(1000);
    Fw::Logger::log("Program Started\n");

    // Object for communicating state to the reference topology
    atmelBase::TopologyState inputs;
    inputs.uartNumber = 0;
    inputs.uartBaud = 115200;

    // Setup, cycle, and teardown topology
    atmelBase::setupTopology(inputs);
}

/**
 * \brief run the program
 *
 * NOTE this based on an extraction of the Arduino loop() function.
 *
 */
void loop() {
#ifdef USE_BASIC_TIMER
    rateDriver.cycle();
#endif
    Os::Baremetal::TaskRunner::getSingleton().run();
}
