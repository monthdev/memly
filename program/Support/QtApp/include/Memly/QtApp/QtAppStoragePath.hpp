#pragma once

#include <string>

namespace Support::QtApp {
[[nodiscard]] auto AudioDirectoryPath() -> std::string;

[[nodiscard]] auto ExceptionLogFilePath() -> std::string;

[[nodiscard]] auto DatabaseFilePath() -> std::string;
}
