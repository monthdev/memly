#pragma once

#include <string>

namespace Layer::Application::Domain::Deck::Constraint {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

}
