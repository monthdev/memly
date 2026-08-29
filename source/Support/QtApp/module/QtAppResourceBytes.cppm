module;

#include <string>

export module Memly.QtApp.QtAppResourceBytes;

export namespace Memly::QtApp {
[[nodiscard]] std::string
ReadQtAppResourceBytes(const char*);
}
