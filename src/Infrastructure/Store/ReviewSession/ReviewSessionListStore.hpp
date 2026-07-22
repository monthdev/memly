#pragma once

#include <duckdb.hpp>

#include <memory>
#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListRowsPreparedStatement;

public:
    explicit ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReadReviewSessionListRowsPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionListRowsSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadReviewSessionListRowsPreparedStatement);
    }

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow>;
};

}
