#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final {
public:
    struct ReviewSessionListRow {
        std::string m_ReviewSessionId;
        std::string m_ReviewSessionName;
        std::int64_t m_CreatedAtMillisecondsSinceEpoch;
        std::optional<std::int64_t> m_LastCardReviewAtMillisecondsSinceEpoch;
    };

    explicit ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
        : m_ReadReviewSessionListPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionListSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadReviewSessionListPreparedStatement);
    }

    ~ReviewSessionListStore() noexcept = default;
    ReviewSessionListStore(const ReviewSessionListStore&) = delete;
    ReviewSessionListStore(ReviewSessionListStore&&) = delete;
    ReviewSessionListStore& operator=(const ReviewSessionListStore&) = delete;
    ReviewSessionListStore& operator=(ReviewSessionListStore&&) = delete;

    [[nodiscard]] std::vector<ReviewSessionListRow> ReadReviewSessionList();

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListPreparedStatement;
};

}
