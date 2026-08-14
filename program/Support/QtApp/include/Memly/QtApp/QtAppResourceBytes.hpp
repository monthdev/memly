#pragma once

#include <string>

namespace Memly::QtApp {
[[nodiscard]] auto ReadQtAppResourceBytes(const char*) -> std::string;
}
