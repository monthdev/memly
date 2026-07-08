#pragma once

#include <string_view>

namespace Application::Domain::Deck::Constraint {

[[nodiscard]] bool IsDeckNameLengthValid(const std::string_view) noexcept;

}
