#pragma once

#include <cstddef>

#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class [[nodiscard]] QueryResultRowCountRange final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::size_t m_MinimumRowCount;
    std::size_t m_MaximumRowCount;

    explicit QueryResultRowCountRange(std::size_t, std::size_t) noexcept;

public:
    [[nodiscard]] static auto ZeroOrMore() noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto Exactly(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto AtLeast(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto AtMost(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto Between(std::size_t, std::size_t) noexcept -> QueryResultRowCountRange;

    void AssertContains(std::size_t) const noexcept;
};

}
