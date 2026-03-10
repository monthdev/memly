#include "ThrowRuntimeError.hpp"

#include <format>
#include <source_location>

namespace Support {
[[noreturn]] void
ThrowRuntimeError(const std::source_location& SourceLocation) {
    throw std::runtime_error(std::format("Exception thrown in {}, {}, line {}",
                                         SourceLocation.file_name(),
                                         SourceLocation.function_name(),
                                         SourceLocation.line()));
}
}
