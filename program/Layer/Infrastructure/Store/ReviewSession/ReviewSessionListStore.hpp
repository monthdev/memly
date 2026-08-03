// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <vector>

#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_ReadReviewSessionListRowsPreparedStatement;

public:
    explicit ReviewSessionListStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_ReadReviewSessionListRowsPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::Query::ReadReviewSessionListRowsSql())
        } {
    }

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow>;
};

}
#endif
