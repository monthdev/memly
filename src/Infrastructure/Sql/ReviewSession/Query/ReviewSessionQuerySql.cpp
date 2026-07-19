#include "ReviewSessionQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::ReviewSession::Query {

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
