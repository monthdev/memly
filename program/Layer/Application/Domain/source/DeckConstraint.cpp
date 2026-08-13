#include "Memly/Domain/DeckConstraint.hpp"

#include <cassert>
#include <cstddef>
#include <string>

namespace Layer::Application::Domain {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string& DeckName) noexcept -> bool {
    static constexpr std::size_t s_DeckNameMaximumLength{ 40 };
    assert(not DeckName.empty());
    return bool{ DeckName.size() <= s_DeckNameMaximumLength };
}

}
