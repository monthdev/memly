#pragma once

#include <initializer_list>
#include <source_location>
#include <string_view>

namespace Support::Runtime {
[[noreturn]] void ThrowMemlyException(std::initializer_list<std::string_view>, const std::source_location& = std::source_location::current());
}
