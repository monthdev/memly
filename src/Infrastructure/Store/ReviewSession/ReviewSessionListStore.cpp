#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::ReviewSession {

[[nodiscard]] std::vector<Domain::ReviewSession::ReviewSessionListRow> ReviewSessionListStore::ReadReviewSessionListRows() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionListRowsPreparedStatement->Execute() };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    std::vector<Domain::ReviewSession::ReviewSessionListRow> ReviewSessionListRowVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        ReviewSessionListRowVector.emplace_back(QueryResultRow.GetValue<std::string>(0),
                                                QueryResultRow.GetValue<std::string>(1),
                                                QueryResultRow.GetValue<std::int64_t>(2),
                                                QueryResultRow.IsNull(3) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(3)),
                                                QueryResultRow.IsNull(4) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(4)));
    }
    return ReviewSessionListRowVector;
}

}
