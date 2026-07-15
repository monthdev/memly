#include "Application/Domain/Deck/Constraint/DeckConstraint.hpp"

#include <cassert>
#include <cstddef>
#include <string>
#

namespace Application::Domain::Deck::Constraint {
namespace {

constexpr std::size_t u_DeckNameMaximumLength{ 40 };

}

[[nodiscard]] bool IsDeckNameLengthValid(const std::string& DeckName) noexcept {
    assert(not DeckName.empty());
    return DeckName.size() <= u_DeckNameMaximumLength;
}

}
