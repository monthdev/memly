#pragma once

#include <utility>
#include <vector>

#include "Memly/Database/DecodableQueryResultRowMixin.hpp"
#include "Memly/Database/QueryResultRowCountRange.hpp"

namespace Memly::Database {

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

    [[nodiscard]] auto AssertRowCount([[maybe_unused]] const QueryResultRowCountRange& ExpectedQueryResultRowCountRange) && -> std::vector<QueryResultRowType> {
        ExpectedQueryResultRowCountRange.AssertContains(this->m_QueryResultRowVector.size());
        return std::vector<QueryResultRowType>{ std::move(this->m_QueryResultRowVector) };
    }
};

}
