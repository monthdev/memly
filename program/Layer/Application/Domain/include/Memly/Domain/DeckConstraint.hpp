#pragma once

#include <string>

namespace Layer::Application::Domain {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

}
