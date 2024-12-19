/*
 * AtmelTime.cpp:
 *
 * An implementation of AtmelTime used on Atmel so that standard system components can be run as
 * expected. The time format is (U32 seconds, U32 microseconds) and must be mapped to the Atmel
 * supplied "millis()" and "micros()" functions. Thus, the limitation is that this time will roll
 * over after 49 days of continuous use.
 *
 * @author lestarch
 */
#include <Atmel/Svc/AtmelTime/AtmelTime.hpp>
#include <TimeLib.h>
#include <FprimeAtmel.hpp>

namespace Atmel {

constexpr FwSizeType TIME_ROLLOVER_RETRIES = 3;
constexpr  U32 MICROSECONDS_PER_SECOND = 1000000;

AtmelTime::AtmelTime(const char* name) : AtmelTimeComponentBase(name) {}
AtmelTime::~AtmelTime() {}

void AtmelTime::getTime_handler(NATIVE_INT_TYPE portNum, /*!< The port number*/
                                      Fw::Time& time           /*!< The time to set */
) {
    time_t time_seconds = 0;
    U32 time_microseconds = 0;
    for (FwSizeType i = 0; i < TIME_ROLLOVER_RETRIES; i++) {
        U32 time_microseconds_rollover = ::micros() % MICROSECONDS_PER_SECOND;
        time_seconds = ::now();
        time_microseconds = ::micros() % MICROSECONDS_PER_SECOND;
        // No rollover exit the loop without further retries
        if (time_microseconds > time_microseconds_rollover) {
            break;
        }
    }
    TimeBase base = (::timeStatus() == timeStatus_t::timeNeedsSync) ? TimeBase::TB_PROC_TIME : TimeBase::TB_WORKSTATION_TIME;
    // Look for seconds overflow
    FW_ASSERT(time_seconds < std::numeric_limits<U32>::max());
    time.set(base, static_cast<U32>(time_seconds), time_microseconds);
}

void AtmelTime ::setTime(U32 year, U8 month, U8 day, U8 hour, U8 minute, U8 second) {
    year = (year > std::numeric_limits<int>::max()) ? std::numeric_limits<int>::max() : year;
    Fw::Time before_set = this->getTime();
    ::setTime(hour, minute, second, day, month, year);
    Fw::Time after_set = this->getTime();
    this->log_ACTIVITY_HI_TimeUpdate(before_set.getSeconds(), before_set.getUSeconds(), before_set.getTimeBase(),
                                  after_set.getSeconds(), after_set.getUSeconds(), after_set.getTimeBase());
}

void AtmelTime ::setTime_handler(FwIndexType portNum, U32 year, U8 month, U8 day, U8 hour, U8 minute, U8 second) {
    this->setTime(year, month, day, hour, minute, second);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void AtmelTime ::SET_TIME_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       U32 year,
                                       U8 month,
                                       U8 day,
                                       U8 hour,
                                       U8 minute,
                                       U8 second) {
    this->setTime(year, month, day, hour, minute, second);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
}  // namespace Atmel
