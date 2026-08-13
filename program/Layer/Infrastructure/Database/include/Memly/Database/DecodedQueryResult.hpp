#pragma once

#include <utility>
#include <vector>

#include "Memly/Database/DecodableQueryResultRowMixin.hpp"
#include "Memly/Database/QueryResultRowCountRange.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

template <DecodableQueryResultRow QueryResultRowType>
class [[nodiscard]] DecodedQueryResult final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::vector<QueryResultRowType> m_QueryResultRowVector;

public:
    explicit DecodedQueryResult(std::vector<QueryResultRowType>&& QueryResultRowVector)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_QueryResultRowVector{ std::move(QueryResultRowVector) } {
    }

    [[nodiscard]] auto AssertRowCount([[maybe_unused]] const QueryResultRowCountRange& ExpectedQueryResultRowCountRange) && -> std::vector<QueryResultRowType> {
        ExpectedQueryResultRowCountRange.AssertContains(this->m_QueryResultRowVector.size());
        return std::vector<QueryResultRowType>{ std::move(this->m_QueryResultRowVector) };
    }
};

}
