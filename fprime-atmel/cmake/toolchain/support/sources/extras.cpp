#include <Fw/Logger/Logger.hpp>

// Redirect write operations to F´ logger
#ifdef FPRIME_ATMEL_SYNTHETIC_WRITE
extern "C" {
    #include <stdlib.h>
    int _write(int handle, char *buf, int count) {
        // Create null-terminated string for logger
        char output[static_cast<unsigned int>(count + 1)];
        for (int i = 0; i < count; i++) {
            output[i] = buf[i];
        }
        output[count] = '\0';

        // Use F´ logger
        Fw::Logger::log(output);
        return count;
    }
}
#endif

// Provide C++ memory allocation operators
#ifdef FPRIME_ATMEL_SYNTHETIC_NEW_NOTHROW
#include <stdlib.h>
#include <new>

namespace std {
    const std::nothrow_t nothrow;
}

void* operator new(size_t size, const std::nothrow_t&) {
    return malloc(size);
}

void* operator new[](size_t size, const std::nothrow_t&) {
    return malloc(size);
}

// Optional: Add delete operators if needed
void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}
#endif
