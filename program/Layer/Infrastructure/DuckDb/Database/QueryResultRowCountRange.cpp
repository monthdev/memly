#include "Layer/Infrastructure/DuckDb/Database/QueryResultRowCountRange.hpp"

#include <cassert>
#include <cstddef>
#include <limits>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

QueryResultRowCountRange::QueryResultRowCountRange(const std::size_t MinimumRowCount, const std::size_t MaximumRowCount) noexcept
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_MinimumRowCount{ MinimumRowCount }
    , m_MaximumRowCount{ MaximumRowCount } {
}

[[nodiscard]] auto QueryResultRowCountRange::ZeroOrMore() noexcept -> QueryResultRowCountRange {
    return QueryResultRowCountRange{ std::size_t{ 0 }, std::numeric_limits<std::size_t>::max() };
}

[[nodiscard]] auto QueryResultRowCountRange::Exactly(const std::size_t RowCount) noexcept -> QueryResultRowCountRange {
    return QueryResultRowCountRange{ RowCount, RowCount };
}

[[nodiscard]] auto QueryResultRowCountRange::AtLeast(const std::size_t MinimumRowCount) noexcept -> QueryResultRowCountRange {
    assert(MinimumRowCount > 0);
    return QueryResultRowCountRange{ MinimumRowCount, std::numeric_limits<std::size_t>::max() };
}

[[nodiscard]] auto QueryResultRowCountRange::AtMost(const std::size_t MaximumRowCount) noexcept -> QueryResultRowCountRange {
    return QueryResultRowCountRange{ std::size_t{ 0 }, MaximumRowCount };
}

[[nodiscard]] auto QueryResultRowCountRange::Between(const std::size_t MinimumRowCount, const std::size_t MaximumRowCount) noexcept
    -> QueryResultRowCountRange {
    assert(MinimumRowCount < MaximumRowCount);
    return QueryResultRowCountRange{ MinimumRowCount, MaximumRowCount };
}

[[nodiscard]] auto QueryResultRowCountRange::Contains(const std::size_t RowCount) const noexcept -> bool {
    return bool{ RowCount >= this->m_MinimumRowCount and RowCount <= this->m_MaximumRowCount };
}

}
