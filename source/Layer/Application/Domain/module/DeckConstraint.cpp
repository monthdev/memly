module;

#include <cassert>
#include <cstddef>
#include <string>

module Memly.Domain.DeckConstraint;

namespace Memly::Domain {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string& DeckName) noexcept -> bool {
    static constexpr std::size_t s_DeckNameMaximumLength{ 40 };
    assert(not DeckName.empty());
    return bool{ DeckName.size() <= s_DeckNameMaximumLength };
}

}
