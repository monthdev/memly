module;

#include <utility>
#include <vector>

export module Memly.Database.DecodedQueryResult;

import Memly.Database.DecodableQueryResultRowMixin;
import Memly.Database.QueryResultRowCountRange;

export namespace Memly::Database {

template <DecodableQueryResultRow QueryResultRowType>
class [[nodiscard]] DecodedQueryResult final {
private:
    std::vector<QueryResultRowType> m_QueryResultRowVector;

public:
    explicit DecodedQueryResult(
        std::vector<QueryResultRowType>&& QueryResultRowVector
    ) : m_QueryResultRowVector{ std::move(QueryResultRowVector) } {
    }

    explicit DecodedQueryResult(const DecodedQueryResult&) = delete;
    DecodedQueryResult&
    operator=(const DecodedQueryResult&) = delete;

    explicit DecodedQueryResult(DecodedQueryResult&&) = delete;
    DecodedQueryResult&
    operator=(DecodedQueryResult&&) = delete;

    ~DecodedQueryResult() noexcept = default;

    [[nodiscard]] std::vector<QueryResultRowType>
    AssertRowCount([[maybe_unused]] const QueryResultRowCountRange
            ExpectedQueryResultRowCountRange) && {
#if not defined(NDEBUG)
        ExpectedQueryResultRowCountRange
            .AssertContains(this->m_QueryResultRowVector.size());
#endif
        return std::vector<QueryResultRowType>{
            std::move(this->m_QueryResultRowVector),
        };
    }
};

}
