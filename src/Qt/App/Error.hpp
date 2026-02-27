#pragma once

#include <string>

namespace App {
namespace Error {
[[noreturn]] void Exit(const std::string&);
}
}
