#pragma once

#include <string>

namespace Support::Runtime::QtApp {
[[nodiscard]] auto ReadQtAppResourceBytes(const char* const) -> std::string;
}
