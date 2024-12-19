#ifndef ATMEL_DRV_HARDWARERATEDRIVER_HPP
#define ATMEL_DRV_HARDWARERATEDRIVER_HPP
#include <Os/RawTime.hpp>
#include <Atmel/Drv/HardwareRateDriver/HardwareRateDriverComponentAc.hpp>

namespace Atmel {
    /**
     * HardwareRateDriver:
     *
     * Used to drive the rate group driver via the CycleIn port. That port is
     * triggered via a timer interrupt.
     */
    class HardwareRateDriver : public HardwareRateDriverComponentBase  {
        public:
            /**
             * Construct the rate driver. Takes in a name (if configured) and a
             * rate at which to drive.
             * \param const char* compName: name of the component (only supply if configured)
             */
            HardwareRateDriver(const char* compName);

            /**
             * Configure this component with the interval time in milliseconds.
             * \param U32 intervalMs: interval to ping in milliseconds
             */
            void configure(U32 intervalMs);

            /**
             * Cycle simulated timer.
             */
            void cycle();

            /**
             * Starts this driver, kicking off the hardware interrupt or the Linux
             * task that virtualizes this driver.
             */
            void start();
            /**
             * Stops the hardware driver, enabling some clean-up.
             */
            void stop();
            //!< Driver deconstructor
            ~HardwareRateDriver(void);
            //!< Static callback for task function
            static void s_timer(void * comp);
            //!< Interval of the driver
            U32 m_interval;
        private:
            //!< Last time of run
            Os::RawTime m_last;
            //!< Pointer to the driver
            static HardwareRateDriver* s_driver;
            //!< Static callback to the ISR triggered via a timer
            static void s_timerISR();
    };
}
#endif