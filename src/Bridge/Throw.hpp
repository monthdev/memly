#pragma once

#include <source_location>

[[noreturn]] void
Throw(const std::source_location& = std::source_location::current());
