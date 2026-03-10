#pragma once

#include <source_location>

namespace Support {
[[noreturn]] void ThrowRuntimeError(
    const std::source_location& = std::source_location::current());
}
