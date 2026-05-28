#include "Infrastructure/Store/ReviewSessionListStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>

#include "Infrastructure/Sql/QuerySqlResource.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store {

ReviewSessionListStore::ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
    : m_ReadReviewSessionListPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::ReadReviewSessionListSql()) } {
    Infrastructure::Sql::ThrowUnconditionallyOnPreparedStatementError(*m_ReadReviewSessionListPreparedStatement);
}

ReviewSessionListStore::~ReviewSessionListStore() = default;

[[nodiscard]] QVector<ReviewSessionListStore::ReviewSessionListRow> ReviewSessionListStore::ReadReviewSessionList() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionListPreparedStatement->Execute() };
    Infrastructure::Sql::ThrowUnconditionallyOnQueryResultError(*QueryResult);
    QVector<ReviewSessionListRow> ReviewSessionListRowQVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        ReviewSessionListRowQVector.emplace_back(QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                                                 QString{ QueryResultRow.GetValue<std::string>(1).c_str() },
                                                 static_cast<qint64>(QueryResultRow.GetValue<std::int64_t>(2)),
                                                 QueryResultRow.IsNull(3) ? std::nullopt :
                                                                            std::make_optional(static_cast<qint64>(QueryResultRow.GetValue<std::int64_t>(3))));
    }
    return ReviewSessionListRowQVector;
}

}
