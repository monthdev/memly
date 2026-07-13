#pragma once

#include <string>

namespace Support::Runtime::QtApp {
[[nodiscard]] std::string AudioDirectoryPath();

[[nodiscard]] std::string ExceptionLogFilePath();

[[nodiscard]] std::string DatabaseFilePath();
}
