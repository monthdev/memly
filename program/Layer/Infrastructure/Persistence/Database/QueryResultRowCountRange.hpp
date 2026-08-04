#pragma once

#include <cstddef>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Database {

class [[nodiscard]] QueryResultRowCountRange final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::size_t m_MinimumRowCount;
    std::size_t m_MaximumRowCount;

    explicit QueryResultRowCountRange(const std::size_t MinimumRowCount, const std::size_t MaximumRowCount) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_MinimumRowCount{ MinimumRowCount }
        , m_MaximumRowCount{ MaximumRowCount } {
    }

public:
    [[nodiscard]] static auto ZeroOrMore() noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto Exactly(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto AtLeast(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto AtMost(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto Between(std::size_t, std::size_t) noexcept -> QueryResultRowCountRange;

    [[nodiscard]] auto Contains(std::size_t) const noexcept -> bool;
};

}
