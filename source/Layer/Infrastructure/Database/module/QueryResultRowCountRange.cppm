module;

#include <cstddef>

export module Memly.Database.QueryResultRowCountRange;

export namespace Memly::Database {

class [[nodiscard]] QueryResultRowCountRange final {
private:
    std::size_t m_MinimumRowCount;
    std::size_t m_MaximumRowCount;

    explicit QueryResultRowCountRange(std::size_t, std::size_t) noexcept;

public:
    explicit QueryResultRowCountRange(const QueryResultRowCountRange&) = delete;
    QueryResultRowCountRange&
    operator=(const QueryResultRowCountRange&) = delete;

    explicit QueryResultRowCountRange(QueryResultRowCountRange&&) = delete;
    QueryResultRowCountRange&
    operator=(QueryResultRowCountRange&&) = delete;

    ~QueryResultRowCountRange() noexcept = default;

    [[nodiscard]] static QueryResultRowCountRange
    ZeroOrMore() noexcept;

    [[nodiscard]] static QueryResultRowCountRange
    Exactly(std::size_t) noexcept;

    [[nodiscard]] static QueryResultRowCountRange
    AtLeast(std::size_t) noexcept;

    [[nodiscard]] static QueryResultRowCountRange
    AtMost(std::size_t) noexcept;

    [[nodiscard]] static QueryResultRowCountRange
    Between(std::size_t, std::size_t) noexcept;

#if not defined(NDEBUG)
    void
    AssertContains(std::size_t) const noexcept;
#endif
};

}
