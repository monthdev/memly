#pragma once

#include <duckdb.hpp>

#include <memory>
#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListRowsPreparedStatement;

public:
    explicit ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
        : m_ReadReviewSessionListRowsPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionListRowsSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadReviewSessionListRowsPreparedStatement);
    }

    explicit ReviewSessionListStore(const ReviewSessionListStore&) = delete;
    explicit ReviewSessionListStore(ReviewSessionListStore&&) = delete;
    auto operator=(const ReviewSessionListStore&) -> ReviewSessionListStore& = delete;
    auto operator=(ReviewSessionListStore&&) -> ReviewSessionListStore& = delete;

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow>;
};

}
