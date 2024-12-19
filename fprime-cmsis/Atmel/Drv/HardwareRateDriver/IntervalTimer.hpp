#ifndef INTERVAL_TIMER_H
#define INTERVAL_TIMER_H

#include <stdint.h>

class IntervalTimer {
public:
    // Function pointer type for ISR callbacks
    typedef void (*ISR)();

    // Constructor/Destructor
    IntervalTimer();
    ~IntervalTimer();

    // Public interface
    bool begin(ISR newISR, uint32_t newPeriod);
    void end();

private:
    // Timer states
    enum TimerState {
        TIMER_OFF,
        TIMER_RTT
    };

    // Constants
    static constexpr uint8_t NUM_RTT = 1;  // SAMV71 has 1 RTT
    static constexpr uint32_t RTT_PRESCALER = 32768; // RTT clock frequency
    static constexpr uint32_t MAX_PERIOD = UINT32_MAX / (RTT_PRESCALER / 1000000.0);

    // Static members for RTT management
    static bool RTT_enabled;
    static bool RTT_used;
    static ISR RTT_ISR;

    // Private methods
    static void enable_RTT();
    static void disable_RTT();
    bool allocate_RTT(uint32_t newValue);
    void start_RTT(uint32_t newValue);
    void stop_RTT();

    // Instance members
    TimerState status;
    ISR myISR;
};

#endif // INTERVAL_TIMER_H
