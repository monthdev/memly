#pragma once

#include <string>

namespace Memly::Domain {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

}
