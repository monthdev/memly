#include "Domain/Deck/DeckConstraint.hpp"

#include <cstddef>
#include <string_view>

namespace Domain::Deck {
namespace {

constexpr std::size_t u_DeckNameMaximumLength{ 40 };

}

[[nodiscard]] bool IsDeckNameLengthValid(const std::string_view DeckNameStringView) noexcept {
    return not DeckNameStringView.empty() and DeckNameStringView.size() <= u_DeckNameMaximumLength;
}

}
