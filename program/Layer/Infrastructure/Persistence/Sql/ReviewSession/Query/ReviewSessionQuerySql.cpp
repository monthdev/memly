// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "ReviewSessionQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::ReviewSession::Query {

[[nodiscard]] auto ReadReviewSessionListRowsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadReviewSessionListRows.sql");
}

[[nodiscard]] auto ReadDefaultReviewSessionIdByRootDeckIdSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] auto ReadReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadReviewSessionIdByReviewSessionDefinitionKey.sql");
}
}
#endif
