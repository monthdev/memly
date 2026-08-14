#pragma once

#include <cstddef>

namespace Memly::Database {

class [[nodiscard]] QueryResultRowCountRange final {
private:
    std::size_t m_MinimumRowCount;
    std::size_t m_MaximumRowCount;

    explicit QueryResultRowCountRange(std::size_t, std::size_t) noexcept;

public:
    explicit QueryResultRowCountRange(const QueryResultRowCountRange&) = delete;
    auto operator=(const QueryResultRowCountRange&) -> QueryResultRowCountRange& = delete;

    explicit QueryResultRowCountRange(QueryResultRowCountRange&&) = delete;
    auto operator=(QueryResultRowCountRange&&) -> QueryResultRowCountRange& = delete;

    ~QueryResultRowCountRange() noexcept = default;

    [[nodiscard]] static auto ZeroOrMore() noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto Exactly(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto AtLeast(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto AtMost(std::size_t) noexcept -> QueryResultRowCountRange;
    [[nodiscard]] static auto Between(std::size_t, std::size_t) noexcept -> QueryResultRowCountRange;

    void AssertContains(std::size_t) const noexcept;
};

}
