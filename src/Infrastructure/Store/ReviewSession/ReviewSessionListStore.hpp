#pragma once

#include <duckdb.hpp>

#include <memory>
#include <vector>

#include "Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final {
public:
    explicit ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
        : m_ReadReviewSessionListRowsPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionListRowsSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadReviewSessionListRowsPreparedStatement);
    }

    ~ReviewSessionListStore() noexcept = default;
    ReviewSessionListStore(const ReviewSessionListStore&) = delete;
    ReviewSessionListStore(ReviewSessionListStore&&) = delete;
    ReviewSessionListStore& operator=(const ReviewSessionListStore&) = delete;
    ReviewSessionListStore& operator=(ReviewSessionListStore&&) = delete;

    [[nodiscard]] std::vector<Domain::ReviewSession::ReviewSessionListRow> ReadReviewSessionListRows();

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListRowsPreparedStatement;
};

}
