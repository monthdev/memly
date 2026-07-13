#pragma once

#include <string>
#include <string_view>

namespace Support::Runtime::QtApp {
[[nodiscard]] std::string ReadQtAppResourceBytes(const std::string_view);
}
