#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::ReviewSession {

[[nodiscard]] auto ReviewSessionListStore::ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionListRowsPreparedStatement->Execute() };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> ReviewSessionListRowVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        ReviewSessionListRowVector.emplace_back(QueryResultRow.GetValue<std::string>(0),
                                                QueryResultRow.GetValue<std::string>(1),
                                                QueryResultRow.GetValue<std::int64_t>(2),
                                                QueryResultRow.IsNull(3) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(3)),
                                                QueryResultRow.IsNull(4) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(4)));
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
    return ReviewSessionListRowVector;
}

}
