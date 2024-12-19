#include "IntervalTimer.hpp"
#include "sam.h"  // SAMV71 peripheral access

// Static member initialization
bool IntervalTimer::RTT_enabled = false;
bool IntervalTimer::RTT_used = false;
IntervalTimer::ISR IntervalTimer::RTT_ISR = nullptr;

// Constructor
IntervalTimer::IntervalTimer() : status(TIMER_OFF), myISR(nullptr) {
}

// Destructor
IntervalTimer::~IntervalTimer() {
    this->end();
}

// RTT interrupt handler
extern "C" void RTT_Handler(void) {
    // Clear status register by reading it
    RTT->RTT_SR;

    // Call user ISR if set
    if (IntervalTimer::RTT_ISR != nullptr) {
        IntervalTimer::RTT_ISR();
    }
}

bool IntervalTimer::begin(ISR newISR, uint32_t newPeriod) {
    // Store callback pointer
    this->myISR = newISR;

    // Check range and calculate value based on period
    if (newPeriod == 0 || newPeriod > MAX_PERIOD) {
        return false;
    }

    // Calculate RTT value (converting microseconds to RTT ticks)
    uint32_t newValue = (RTT_PRESCALER * newPeriod) / 1000000;

    // Attempt to allocate this timer
    if (this->allocate_RTT(newValue)) {
        this->status = TIMER_RTT;
    } else {
        this->status = TIMER_OFF;
    }

    // Check for success and return
    return (this->status != TIMER_OFF);
}

void IntervalTimer::end() {
    if (this->status == TIMER_RTT) {
        this->stop_RTT();
    }
    this->status = TIMER_OFF;
}

void IntervalTimer::enable_RTT() {
    // Enable RTT peripheral clock
    PMC->PMC_PCER0 |= PMC_PCER0_PID3;  // ID_RTT = 3

    // Configure RTT for 32.768 kHz external crystal
    RTT->RTT_MR = RTT_MR_RTPRES(1);  // Set prescaler to 1

    RTT_enabled = true;
}

void IntervalTimer::disable_RTT() {
    // Disable RTT peripheral clock
    PMC->PMC_PCDR0 |= PMC_PCDR0_PID3;

    RTT_enabled = false;
}

bool IntervalTimer::allocate_RTT(uint32_t newValue) {
    // Enable clock to the RTT module if necessary
    if (!RTT_enabled) {
        enable_RTT();
    }

    // Check if RTT is available
    if (!RTT_used) {
        start_RTT(newValue);
        RTT_used = true;
        return true;
    }

    return false;
}

void IntervalTimer::start_RTT(uint32_t newValue) {
    // Store ISR
    RTT_ISR = this->myISR;

    // Configure RTT
    RTT->RTT_MR |= RTT_MR_ALMIEN;  // Enable alarm interrupt
    RTT->RTT_AR = newValue;         // Set alarm value

    // Enable RTT interrupt in NVIC
    NVIC_EnableIRQ(RTT_IRQn);
}

void IntervalTimer::stop_RTT() {
    // Disable RTT interrupt
    NVIC_DisableIRQ(RTT_IRQn);

    // Disable RTT alarm interrupt
    RTT->RTT_MR &= ~RTT_MR_ALMIEN;

    // Clear ISR pointer
    RTT_ISR = nullptr;

    // Free RTT for future use
    RTT_used = false;

    // If RTT is no longer used, disable it
    if (!RTT_used) {
        disable_RTT();
    }
}
