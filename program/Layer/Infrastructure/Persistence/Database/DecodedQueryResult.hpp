#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "Layer/Infrastructure/Persistence/Database/DecodableQueryResultRowMixin.hpp"
#include "Layer/Infrastructure/Persistence/Database/QueryResultRowCountRange.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Database {

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
        assert(ExpectedQueryResultRowCountRange.Contains(this->m_QueryResultRowVector.size()));
        return std::vector<QueryResultRowType>{ std::move(this->m_QueryResultRowVector) };
    }
};

}
