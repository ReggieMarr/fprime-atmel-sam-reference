#include <FpConfig.hpp>
#include <HardwareRateDriver.hpp>
#include "IntervalTimer.hpp"
#include <Atmel.h>

namespace Atmel {
IntervalTimer s_itimer;

void HardwareRateDriver::start() {
    (void) s_itimer.begin(HardwareRateDriver::s_timerISR, m_interval * 1000);
}

void HardwareRateDriver::stop() {
    s_itimer.end();
}

void HardwareRateDriver::s_timerISR() {
    s_timer(s_driver);
}

};
