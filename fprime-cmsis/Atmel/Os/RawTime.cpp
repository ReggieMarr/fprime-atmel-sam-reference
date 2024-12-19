// ======================================================================
// \title Atmel/Os/RawTime.cpp
// \brief stub implementation for Os::RawTime
// ======================================================================
#include "Atmel/Os/RawTime.hpp"
#include "FprimeAtmel.hpp"
namespace Os {
namespace Atmel {

//! \brief check is a is newer than b
bool isNewer(const AtmelRawTimeHandle& a, const AtmelRawTimeHandle& b) {
    return ((a.m_seconds > b.m_seconds) ||
           ((a.m_seconds == b.m_seconds) && (a.m_micros >= b.m_seconds)));
}

RawTimeHandle* AtmelRawTime::getHandle() {
    return &this->m_handle;
}

RawTime::Status AtmelRawTime::now() {
    U32 milliseconds_now = millis();
    U32 microseconds_now = micros() % 1000000;
    U32 milliseconds_no_seconds = milliseconds_now % 1000;
    // Microsecond portion and millisecond portion don't agree, assume roll-over and ask for milliseconds again
    if (milliseconds_no_seconds != (microseconds_now/1000)) {
        microseconds_now = millis();
    }
    this->m_handle.m_micros = microseconds_now % 1000000;
    this->m_handle.m_seconds = milliseconds_now / 1000;
    return Status::OP_OK;
}

RawTime::Status AtmelRawTime::getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const {
    interval.set(0, 0);
    const AtmelRawTimeHandle& my_handle = this->m_handle;
    const AtmelRawTimeHandle& other_handle = static_cast<const AtmelRawTimeHandle&>(*const_cast<Os::RawTime&>(other).getHandle());

    const AtmelRawTimeHandle& newer = isNewer(my_handle, other_handle) ? my_handle : other_handle;
    const AtmelRawTimeHandle& older = isNewer(my_handle, other_handle) ? other_handle : my_handle;

    if (newer.m_micros < older.m_micros) {
        interval.set(newer.m_seconds - older.m_seconds - 1, 1000000 + newer.m_micros - older.m_micros);
    } else {
        interval.set(newer.m_seconds - older.m_seconds, newer.m_micros - older.m_micros);
    }

    return Status::OP_OK;
}

Fw::SerializeStatus AtmelRawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus status = Fw::SerializeStatus::FW_SERIALIZE_OK;
    status = buffer.serialize(this->m_handle.m_seconds);
    if (status == Fw::FW_SERIALIZE_OK) {
        status = buffer.serialize(this->m_handle.m_micros);
    }
    return status;
}

Fw::SerializeStatus AtmelRawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    Fw::SerializeStatus status = Fw::SerializeStatus::FW_SERIALIZE_OK;
    status = buffer.deserialize(this->m_handle.m_seconds);
    if (status == Fw::FW_SERIALIZE_OK) {
        status = buffer.deserialize(this->m_handle.m_micros);
    }
    return status;
}
}  // namespace Atmel
}  // namespace Os
