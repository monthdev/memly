module;

#include <cassert>
#include <cstddef>
#include <limits>

module Memly.Database.QueryResultRowCountRange;

namespace Memly::Database {

QueryResultRowCountRange::QueryResultRowCountRange(
    const std::size_t MinimumRowCount,
    const std::size_t MaximumRowCount
) noexcept :
    m_MinimumRowCount{ MinimumRowCount }, m_MaximumRowCount{ MaximumRowCount } {
}

[[nodiscard]] QueryResultRowCountRange
QueryResultRowCountRange::ZeroOrMore() noexcept {
    return QueryResultRowCountRange{ std::size_t{ 0 },
        std::numeric_limits<std::size_t>::max() };
}

[[nodiscard]] QueryResultRowCountRange
QueryResultRowCountRange::Exactly(const std::size_t RowCount) noexcept {
    return QueryResultRowCountRange{ RowCount, RowCount };
}

[[nodiscard]] QueryResultRowCountRange
QueryResultRowCountRange::AtLeast(const std::size_t MinimumRowCount) noexcept {
    assert(MinimumRowCount > 0);
    return QueryResultRowCountRange{ MinimumRowCount,
        std::numeric_limits<std::size_t>::max() };
}

[[nodiscard]] QueryResultRowCountRange
QueryResultRowCountRange::AtMost(const std::size_t MaximumRowCount) noexcept {
    return QueryResultRowCountRange{ std::size_t{ 0 }, MaximumRowCount };
}

[[nodiscard]] QueryResultRowCountRange
QueryResultRowCountRange::Between(
    const std::size_t MinimumRowCount,
    const std::size_t MaximumRowCount
) noexcept {
    assert(MinimumRowCount < MaximumRowCount);
    return QueryResultRowCountRange{ MinimumRowCount, MaximumRowCount };
}

#if not defined(NDEBUG)
void
QueryResultRowCountRange::AssertContains(
    const std::size_t RowCount
) const noexcept {
    assert(
        RowCount >= this->m_MinimumRowCount and
        RowCount <= this->m_MaximumRowCount
    );
}
#endif

}
