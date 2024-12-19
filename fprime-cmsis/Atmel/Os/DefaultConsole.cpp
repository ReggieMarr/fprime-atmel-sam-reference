// ======================================================================
// \title Os/Atmel/DefaultConsole.cpp
// \brief sets default Os::Console to atmel implementation via linker
// ======================================================================
#include "Os/Console.hpp"
#include "Atmel/Os/Console.hpp"
#include "Os/Delegate.hpp"

namespace Os {
ConsoleInterface* ConsoleInterface::getDelegate(ConsoleHandleStorage& aligned_new_memory, const ConsoleInterface* to_copy) {
    return Os::Delegate::makeDelegate<ConsoleInterface, Os::Atmel::StreamConsole>(aligned_new_memory, to_copy);
}
}
