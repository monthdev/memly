#pragma once

#include <string_view>

namespace Domain::Deck {

[[nodiscard]] bool IsDeckNameLengthValid(const std::string_view) noexcept;

}
