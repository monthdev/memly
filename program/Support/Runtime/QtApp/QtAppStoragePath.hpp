#pragma once

#include <string>

namespace Support::Runtime::QtApp {
[[nodiscard]] auto AudioDirectoryPath() -> std::string;

[[nodiscard]] auto ExceptionLogFilePath() -> std::string;

[[nodiscard]] auto DatabaseFilePath() -> std::string;
}
