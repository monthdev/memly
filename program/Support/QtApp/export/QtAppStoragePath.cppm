module;

#include <string>

export module Memly.QtApp:QtAppStoragePath;

export namespace Memly::QtApp {
[[nodiscard]] auto AudioDirectoryPath() -> std::string;

[[nodiscard]] auto ExceptionLogFilePath() -> std::string;

[[nodiscard]] auto DatabaseFilePath() -> std::string;
}
