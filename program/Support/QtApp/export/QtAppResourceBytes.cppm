module;

#include <string>

export module Memly.QtApp:QtAppResourceBytes;

export namespace Memly::QtApp {
[[nodiscard]] auto ReadQtAppResourceBytes(const char*) -> std::string;
}
