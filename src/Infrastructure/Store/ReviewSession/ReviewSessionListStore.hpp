#pragma once

#include <duckdb.hpp>

#include <memory>
#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final {
public:
    explicit ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
        : m_ReadReviewSessionListRowsPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionListRowsSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadReviewSessionListRowsPreparedStatement);
    }

    ~ReviewSessionListStore() noexcept = default;
    ReviewSessionListStore(const ReviewSessionListStore&) = delete;
    ReviewSessionListStore(ReviewSessionListStore&&) = delete;
    ReviewSessionListStore& operator=(const ReviewSessionListStore&) = delete;
    ReviewSessionListStore& operator=(ReviewSessionListStore&&) = delete;

    [[nodiscard]] std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> ReadReviewSessionListRows();

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListRowsPreparedStatement;
};

}
