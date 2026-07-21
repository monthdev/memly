#pragma once

#include <source_location>
#include <string_view>

namespace Support::Runtime {
[[noreturn]] void ThrowMemlyException(std::string_view, const std::source_location& = std::source_location::current());
}
