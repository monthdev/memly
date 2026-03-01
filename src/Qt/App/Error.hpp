#pragma once

#include <string>

namespace Qt {
namespace App {
namespace Error {
[[noreturn]] void Exit(const std::string&);
}
}
}
