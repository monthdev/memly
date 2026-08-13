#pragma once

#include <string>

namespace Support::QtApp {
[[nodiscard]] auto ReadQtAppResourceBytes(const char*) -> std::string;
}
