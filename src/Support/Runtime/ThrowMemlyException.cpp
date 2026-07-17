#include "Support/Runtime/ThrowMemlyException.hpp"

#include <source_location>
#include <string_view>

#include "Support/Runtime/MemlyException.hpp"

namespace Support::Runtime {
[[noreturn]] void ThrowMemlyException(const std::string_view ErrorMessage, const std::source_location& SourceLocation) {
    throw MemlyException{ ErrorMessage, SourceLocation };
}
}
