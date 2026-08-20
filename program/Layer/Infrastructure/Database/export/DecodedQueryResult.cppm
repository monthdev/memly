module;

#include <utility>
#include <vector>

export module Memly.Database:DecodedQueryResult;

import :DecodableQueryResultRowMixin;
import :QueryResultRowCountRange;

export namespace Memly::Database {

template <DecodableQueryResultRow QueryResultRowType>
class [[nodiscard]] DecodedQueryResult final {
private:
    std::vector<QueryResultRowType> m_QueryResultRowVector;

public:
    explicit DecodedQueryResult(std::vector<QueryResultRowType>&& QueryResultRowVector)
        : m_QueryResultRowVector{ std::move(QueryResultRowVector) } {
    }

    explicit DecodedQueryResult(const DecodedQueryResult&) = delete;
    auto operator=(const DecodedQueryResult&) -> DecodedQueryResult& = delete;

    explicit DecodedQueryResult(DecodedQueryResult&&) = delete;
    auto operator=(DecodedQueryResult&&) -> DecodedQueryResult& = delete;

    ~DecodedQueryResult() noexcept = default;

    [[nodiscard]] auto AssertRowCount([[maybe_unused]] const QueryResultRowCountRange ExpectedQueryResultRowCountRange) && -> std::vector<QueryResultRowType> {
#if not defined(NDEBUG)
        ExpectedQueryResultRowCountRange.AssertContains(this->m_QueryResultRowVector.size());
#endif
        return std::vector<QueryResultRowType>{ std::move(this->m_QueryResultRowVector) };
    }
};

}
