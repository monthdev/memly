module;

#include <string>

export module Memly.QtApp.QtAppStoragePath;

export namespace Memly::QtApp {
[[nodiscard]] std::string
AudioDirectoryPath();

[[nodiscard]] std::string
ExceptionLogFilePath();

[[nodiscard]] std::string
DatabaseFilePath();
}
