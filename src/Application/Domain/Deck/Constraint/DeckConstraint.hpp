#pragma once

#include <string>

namespace Application::Domain::Deck::Constraint {

[[nodiscard]] bool IsDeckNameLengthValid(const std::string&) noexcept;

}
