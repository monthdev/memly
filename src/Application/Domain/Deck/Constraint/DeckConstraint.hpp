#pragma once

#include <string>

namespace Application::Domain::Deck::Constraint {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

}
