#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "Layer/Infrastructure/Database/DecodableQueryResultRowMixin.hpp"
#include "Layer/Infrastructure/Database/QueryResultRowCountRange.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class QueryResultDecoder;

template <DecodableQueryResultRow QueryResultRowType>
class [[nodiscard]] DecodedQueryResult final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class QueryResultDecoder;

private:
    std::vector<QueryResultRowType> m_QueryResultRowVector;

    explicit DecodedQueryResult(std::vector<QueryResultRowType>&& QueryResultRowVector)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_QueryResultRowVector{ std::move(QueryResultRowVector) } {
    }

public:
    [[nodiscard]] auto AssertRowCount([[maybe_unused]] const QueryResultRowCountRange& ExpectedQueryResultRowCountRange) && -> std::vector<QueryResultRowType> {
        assert(ExpectedQueryResultRowCountRange.Contains(m_QueryResultRowVector.size()));
        return std::vector<QueryResultRowType>{ std::move(m_QueryResultRowVector) };
    }
};

}
